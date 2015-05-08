/*#    
#    This file is part of Flywalk Reloaded. <http://flywalk.eempo.net>
#
#     flywalk@eempo.net - Pedro Gouveia
#
#
#    Flywalk Reloaded is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, version 2.
#
#    Flywalk Reloaded is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Flywalk Reloaded.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "yarp.h"
#include "yarpports.h"
#include "imgproc/particles.h"

clsYarp::clsYarp()
{
	yarpInitiated = false;
	portIsOK = false;
}
clsYarp::~clsYarp()
{
	
}

bool clsYarp::initYarp()
{
	// INFOMSG(("initializing yarp..."));
	yarp.init();
	yarpInitiated = yarp.checkNetwork();
	
	return yarpInitiated;
	// yarpInitiated = true;
	// INFOMSG(("initializing yarp... OK"));
}

bool clsYarp::checkYarp()
{
	if(!yarpInitiated){
		usleep(100);
		#ifdef REQUIRE_YARP
		ERRMSG(("yarp failed to initialize"));
		#else
		WARNMSG(("yarp failed to initialize"));
		#endif
	}
	return yarpInitiated;
}
void clsYarp::connectPorts()
{
	yarp::os::Network::connect("/2D_tracking/0/0",YARPPORT_2DPTS_RECVR,PROTOCOL);
	yarp::os::Network::connect("/2D_tracking/0/1",YARPPORT_2DPTS_RECVR,PROTOCOL);
	
	yarp::os::Network::connect("/2D_tracking/1/0",YARPPORT_2DPTS_RECVR,PROTOCOL);
	yarp::os::Network::connect("/2D_tracking/1/1",YARPPORT_2DPTS_RECVR,PROTOCOL);
	
}

bool clsYarp::init_yarp_2d(int cameraSection,int cameraPosition)
{
	
	initYarp();
	if(!checkYarp()) return false;
	
	char tracking_name[50];
	sprintf(tracking_name, "/2D_tracking/%d/%d",cameraSection,cameraPosition);

	port.open(tracking_name);
	
	connectPorts();
	return true;
}

bool clsYarp::send_2dpts(pts2d point)
{
	//INFOMSG(("SENDING POINT %d %d sec %d pos %d time %d",point.xPx,point.yPx,point.cameraSection,point.cameraPosition,point.timestamp));
	
	yarp::os::Bottle& output = port.prepare();
	output.clear();
	output.addInt(point.timestamp);
	output.addInt(point.cameraSection);
	output.addInt(point.cameraPosition);
	output.addDouble(point.x);
	output.addDouble(point.y);
	output.addInt(point.xPx);
	output.addInt(point.yPx);
	port.write();
	return true;
}

bool clsYarp::initYarp3Dsend()
{
	initYarp();
	
	if(!checkYarp()) return false;
	
	port.open(YARPPORT_2DPTS_RECVR); // RECEIVE
	points3DOutPort.open(YARPPORT_3DPTS_SEND);
	
	connectPorts();
	
	yarp::os::Network::connect(YARPPORT_3DPTS_SEND,YARPPORT_3DPTS_RECVR,PROTOCOL);
	
	return true;
}

bool clsYarp::initYarp3Drcv(char *portName) //YARPPORT_3DPTS_RECVR
{
	initYarp();
	
	if(!checkYarp()) return false;
	
	port.open(portName);
	
	portIsOK = yarp::os::Network::connect(YARPPORT_3DPTS_SEND,portName,PROTOCOL);
	
	return portIsOK;
}

bool clsYarp::send3Dpoint(pts3D point)
{
	
	yarp::os::Bottle& point3DtoSend = points3DOutPort.prepare();
	point3DtoSend.clear();
	point3DtoSend.addInt(getTimestamp());
	point3DtoSend.addDouble(point.xCm);
	point3DtoSend.addDouble(point.yCm);
	point3DtoSend.addDouble(point.zCm);
	point3DtoSend.addInt(point.xPx);
	point3DtoSend.addInt(point.yPx);
	point3DtoSend.addInt(point.zPx);
	
	point3DtoSend.addInt(getTimestamp());
	point3DtoSend.addInt(point.sideSection);
	point3DtoSend.addInt(point.topSection);
	
	for(int s=0;s<2;s++)
	{
		for(int p=0;p<2;p++)
		{
			point3DtoSend.addInt(point.camTimestamp[s][p]);
			point3DtoSend.addInt(point.camxPx[s][p]);
			point3DtoSend.addInt(point.camxPy[s][p]);
			point3DtoSend.addDouble(point.fps[s][p]);
			
		}
	}
	
	points3DOutPort.write();
	
	return true;
	
}
bool clsYarp::rcv3Dpoint(pts3D *point)
{
	
	int i = 0;
	input = port.read(false);
	if (input!=NULL) {
		
		input->get(i++).asInt();
		point->xCm = input->get(i++).asDouble();
		point->yCm = input->get(i++).asDouble();
		point->zCm = input->get(i++).asDouble();
		point->xPx = input->get(i++).asInt();
		point->yPx = input->get(i++).asInt();
		point->zPx = input->get(i++).asInt();
		
		point->timestampReconstr= input->get(i++).asInt();
		point->sideSection = input->get(i++).asInt();
		point->topSection = input->get(i++).asInt();
		
		for(int s=0;s<2;s++)
		{
			for(int p=0;p<2;p++)
			{
				point->camTimestamp[s][p] = input->get(i++).asInt();
				point->camxPx[s][p] = input->get(i++).asInt();
				point->camxPy[s][p] = input->get(i++).asInt();
				point->fps[s][p] = input->get(i++).asDouble();
				
			}
		}
		
		return true;
	}
	
	return false;
	
}
bool clsYarp::rcv_2dpts(pts2d *point)
{

	input = port.read(false);
	if (input!=NULL) {
		point->timestamp = input->get(0).asInt();
		
		point->cameraSection = input->get(1).asInt();
		point->cameraPosition = input->get(2).asInt();
		
		point->x = input->get(3).asDouble();
		point->y = input->get(4).asDouble();
		
		point->xPx = input->get(5).asInt();
		point->yPx = input->get(6).asInt();
		//INFOMSG(("RCV POINT %d %d sec %d pos %d time %d",point->xPx,point->yPx,point->cameraSection,point->cameraPosition,point->timestamp ));		
		return true;
	}
	
	
	return false;
}


bool clsYarp::rcv2DpointLock(pts2d *point)
{
	if(portIsOK)
	{
		input = port.read(true);
		if (input!=NULL) {
			point->timestamp = input->get(0).asInt();
		
			point->cameraSection = input->get(1).asInt();
			point->cameraPosition = input->get(2).asInt();
		
			point->x = input->get(3).asDouble();
			point->y = input->get(4).asDouble();
		
			point->xPx = input->get(5).asInt();
			point->yPx = input->get(6).asInt();
		
			return true;
		}
	}
	
	return false;
}
#ifdef YARP_WITH_CV
bool clsYarp::startCameraStreamIn(char *yarpPortName)
{
	
	INFOMSG(("losing %s",yarpPortName));
	cameraStreamInPort.open(yarpPortName);
	INFOMSG(("losing2"));
	firstAllocationRcv = true;
	
	cvImage = 0;
	
	return true;
}

bool clsYarp::cameraStreamRcv()
{
	if(portIsOK)
	{
		cameraStreamInYarpImage = cameraStreamInPort.read(false);
		if (cameraStreamInYarpImage==NULL && cvImage == NULL)
		{
			return false;
		}
		if(cameraStreamInYarpImage==NULL)
		{
			return true;
		}
		
		if(firstAllocationRcv)
		{
			DEBUGMSG(("receiving 1st image.."));
			cvImage = cvCreateImage(cvSize(cameraStreamInYarpImage->width(),cameraStreamInYarpImage->height()),
				IPL_DEPTH_8U, 3 );
			firstAllocationRcv = false;
		}
		
		cvCvtColor((IplImage*)cameraStreamInYarpImage->getIplImage(),cvImage, CV_RGB2BGR);
		
		return true;
	}
	return false;
}


bool clsYarp::cameraStreamRcvLocked()
{
	if(!cameraStreamInPort.isClosed())
	{
		
		cameraStreamInYarpImage = cameraStreamInPort.read(false);
		if (cameraStreamInYarpImage==NULL)
		{
			return false;
		}
		
		if(firstAllocationRcv)
		{
			DEBUGMSG(("receiving 1st image.."));
			cvImage = cvCreateImage(cvSize(cameraStreamInYarpImage->width(),cameraStreamInYarpImage->height()),
				IPL_DEPTH_8U, 3 );
			firstAllocationRcv = false;
		}
		
		cvCvtColor((IplImage*)cameraStreamInYarpImage->getIplImage(),cvImage, CV_RGB2BGR);
		
		return true;
	}
	return false;
}


bool clsYarp::cameraStreamRcvLocked2()
{
	if(!cameraStreamInPort.isClosed())
	{
		
		cameraStreamInYarpImage = cameraStreamInPort.read(true);
		if (cameraStreamInYarpImage==NULL)
		{
			return false;
		}
		
		if(firstAllocationRcv)
		{
			DEBUGMSG(("receiving 1st image.."));
			cvImage = cvCreateImage(cvSize(cameraStreamInYarpImage->width(),cameraStreamInYarpImage->height()),
				IPL_DEPTH_8U, 3 );
			firstAllocationRcv = false;
		}
		
		cvCvtColor((IplImage*)cameraStreamInYarpImage->getIplImage(),cvImage, CV_RGB2BGR);
		
		return true;
	}
	return false;
}

bool clsYarp::cameraStreamSend(IplImage *cvImage)
{
	ImageOf<PixelRgb> &yarpSendImage = cameraStreamOutPort.prepare();
	yarpSendImage.wrapIplImage(cvImage);
	cameraStreamOutPort.write();
	
	return true;
}
int clsYarp::connectCalibrate3DCamera(int cameraSection,int cameraPosition)
{
	char camPortIn[128],camPortOut[128];
	
	portIsOK = false;
	INFOMSG(("lost1"));
	sprintf(camPortOut,YARPPORT_IMG_RECVR_CALIBRATE_3D, cameraSection,cameraPosition);
	startCameraStreamIn(camPortOut);
	INFOMSG(("lost2"));
	
	sprintf(camPortIn,YARPPORT_IMGCALIB_SENDER, cameraSection,cameraPosition);
	INFOMSG(("lost"));
	if(yarp::os::Network::exists(camPortIn))
	{
		portIsOK = yarp::os::Network::connect(camPortIn,camPortOut,PROTOCOL);
	}
	
	if(!portIsOK)
	{
		ERRMSG(("Port not connected"));
		return 1;
	}
	
	if(!cameraStreamRcvLocked2())
	{
		ERRMSG(("Failed to get 1st image"));
		return 1;
	}
	
	return 0;
}
#endif
bool clsYarp::startCameraStreamOut(char *yarpPortName)
{
	// INFOMSG(("initiating start camera stream to send..."));
	initYarp();
	if(!checkYarp()) return false;
	
	cameraStreamOutPort.open(yarpPortName);
	
	return true;
}



void clsYarp::connectRemoteCamera(int cameraSection, int cameraPosition)
{
	yarp::os::Network::disconnect(YARPPORT_IMG_SENDER_00,YARPPORT_IMG_RECVR);
	yarp::os::Network::disconnect(YARPPORT_IMG_SENDER_01,YARPPORT_IMG_RECVR);
	yarp::os::Network::disconnect(YARPPORT_IMG_SENDER_10,YARPPORT_IMG_RECVR);
	yarp::os::Network::disconnect(YARPPORT_IMG_SENDER_11,YARPPORT_IMG_RECVR);
	
	
	switch(cameraSection)
	{
	case 0:
		if(cameraPosition == SIDE_CAMERA)
		{
			yarp::os::Network::connect(YARPPORT_IMG_SENDER_00,YARPPORT_IMG_RECVR,PROTOCOL);
		}
		if(cameraPosition == TOP_CAMERA)
		{
			yarp::os::Network::connect(YARPPORT_IMG_SENDER_01,YARPPORT_IMG_RECVR,PROTOCOL);
		}
		
		break;
	case 1:
		if(cameraPosition == SIDE_CAMERA)
		{
			yarp::os::Network::connect(YARPPORT_IMG_SENDER_10,YARPPORT_IMG_RECVR,PROTOCOL);
		}
		if(cameraPosition == TOP_CAMERA)
		{
			yarp::os::Network::connect(YARPPORT_IMG_SENDER_11,YARPPORT_IMG_RECVR,PROTOCOL);
		}
		
		break;
	}
}


bool clsYarp::connectPort(char *portIn,char *portOut, char *protocol)
{
	portIsOK = yarp::os::Network::connect(portIn,portOut,protocol);
	
	if(!portIsOK)
	{
	        WARNMSG(("failed to connect %s to %s with %s",portIn,portOut,protocol));
	        return false;
	}
	INFOMSG(("connected %s to %s with %s",portIn,portOut,protocol));
	return true;
}

void clsYarp::listCameras()
{
	char camPortIn[128];
	int camYarpId = 0;
	for(int section=0;section<2;section++)
	{
		for(int position=0;position<2;position++)
		{
			sprintf(camPortIn,YARPPORT_IMGCALIB_SENDER, section,position);
			if(yarp::os::Network::exists(camPortIn))
			{
				printf("0 %d %d ",camYarpId,section+1);
				if(position == SIDE_CAMERA)
				{
					printf("FRONT %d",section+1);
				}
				if(position == TOP_CAMERA)
				{
					printf("TOP %d",section+1);
				}
			}else
			{
				// printf("OFF %s\n",camPortIn);
			}
			printf("\n");
			camYarpId++;
		}
	}
	
}

bool clsYarp::connectCameraCalibrate(int cameraNr,int *cameraSection,int *cameraPosition)
{
	int camYarpId = 0;
	char camPortIn[128];
	INFOMSG(("OLA"));
	for(int section=0;section<2;section++)
	{
		for(int position=0;position<2;position++)
		{
			if(camYarpId == cameraNr)
			{
				sprintf(camPortIn,YARPPORT_IMGCALIB_SENDER, section,position);
				INFOMSG(("They are the same, trying: %s",camPortIn));
				if(yarp::os::Network::exists(camPortIn))
				{
					portIsOK = yarp::os::Network::connect(camPortIn,YARPPORT_IMGCALIB_RECVR,PROTOCOL);
					*cameraSection = section;
					*cameraPosition = position;
					INFOMSG(("connected %s to %s",camPortIn,YARPPORT_IMGCALIB_RECVR));
					return true;
				}
				return false;
			}
			INFOMSG(("connected id: %d sec: %d", camYarpId,cameraNr));
			camYarpId++;
		}
	}
	
	return false;
}

bool clsYarp::initPort(char* name)
{
	if(yarp::os::Network::exists(name))
	{
		DEBUGMSG(("there is already a port with this name..."));
		return false;
	}
	initYarp();
	if(!checkYarp()) return false;
	
	return port.open(name);
}

bool clsYarp::initPortRcv(char* portRcv,char *portSend, char* protocol)
{
	if(!initPort(portRcv))
	{
	        WARNMSG(("Failed to initPort %s",portRcv));
		return false;
	}
	
	return connectPort(portSend,portRcv,protocol);
}

bool clsYarp::initPortSend(char* portSend,char *portRcv, char* protocol)
{
	if(!initPort(portSend))
	{
		return false;
	}
	
	return connectPort(portSend,portRcv,protocol);
}

bool clsYarp::sendInt(int milisecs)
{
	yarp::os::Bottle& input = port.prepare();
	input.clear();
	input.addInt(milisecs);
	port.write();
	
	return true;
}
bool clsYarp::rcvInt(int *milis)
{
	if(port.getPendingReads() > 0)
	{
		input = port.read(false);
		if (input!=NULL) {
			*milis = input->get(0).asInt();
			return true;
		}
	}
	return false;
}

bool clsYarp::sendCommand(std::string cmd,int val,int nparams, std::string param[])
{
	yarp::os::Bottle& input = port.prepare();
	input.clear();
	input.addString(cmd.c_str());
	input.addInt(val);
	input.addInt(nparams);
	
	int t = nparams;
	while(t < nparams)
	{
	        input.addString(param[t].c_str());
	        t++;
	}
	
	port.write();
	
	return true;
}

bool clsYarp::rcvCommand(strRemoteCommand *cmd)
{
	if(pendingReads() > 0)
	{
	        int i = 0;
		input = port.read(false);
		if (input!=NULL) {
		        cmd->cmd = input->get(i).asString();
		        cmd->val = input->get(++i).asInt();
		        cmd->nparams = input->get(++i).asInt();
		        
		        int t = 0;
		        std::string tmpstr;
		        cmd->params.clear();
			while(t < cmd->nparams)
			{
			        tmpstr = input->get(++i).asString();
			        cmd->params.push_back( tmpstr );
			        t++;
			}
			return true;
		}
	}
	return false;
}
bool clsYarp::pendingReads()
{
        if(port.getPendingReads() > 0)
	{
	        return true;
	}
	return false;
}

bool clsYarp::send(std::string str)
{
	yarp::os::Bottle& input = port.prepare();
	input.clear();
	input.addString(str.c_str());
	port.write();
	
	return true;
}
bool clsYarp::rcv(std::string *str,bool locked)
{
	if(port.getPendingReads() > 0)
	{
		input = port.read(locked);
		if (input!=NULL) {
			*str = input->get(0).asString();
			return true;
		}
	}
	return false;
}

bool clsYarp::checkConnection(std::string portIn,std::string portOut)
{
        return yarp::os::NetworkBase::isConnected(portIn.c_str(),portOut.c_str(),true);
}
