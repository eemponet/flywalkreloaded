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
#ifndef YARP_H

#define YARP_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#ifdef YARP_WITH_CV
        #include <cv.h>
        using namespace cv;
#endif

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
#include "utils.h"
#include "../remotecontrol/remotedefs.h"

// using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::sig::file;
using namespace yarp::dev;

using namespace std;

class clsParticle;
/** ATENCAO: **/
/**the defines of the PORTS, changed to yarpports.h, incompabitlities with older versions.. should include yarpports.h in the src file **/

// #define YARPPORT_IMGCALIB_RECVR "/calibrationcamera/IMG"
// #define YARPPORT_IMGCALIB_SENDER "/2Dtrack/calibIMG/%d/%d"

// #define YARPPORT_IMG_SENDER "/2D_tracking_IMG/%d%d"
// #define YARPPORT_IMG_TRACING_SENDER "/2D_tracking_IMG_TRACING/%d%d"

// #define YARPPORT_IMG_SENDER_00 "/2D_tracking_IMG/00"
// #define YARPPORT_IMG_SENDER_01 "/2D_tracking_IMG/01"
// #define YARPPORT_IMG_SENDER_10 "/2D_tracking_IMG/10"
// #define YARPPORT_IMG_SENDER_11 "/2D_tracking_IMG/11"

// #define YARPPORT_IMG_RECVR_CALIBRATE "/2D_tracking_IMG/calibrate"
// #define YARPPORT_IMG_RECVR_CALIBRATE_3D "/calibrate3d/cameraStreamIn/%d%d"

// #define YARPPORT_IMG_RECVR "/3dreconstr/camerasImgs"
// #define YARPPORT_IMG_RECVR_RECCAM "/2D_tracking_IMG/inREC/%d%d"
// #define YARPPORT_IMG_RECVR_RECCAM2 "/2D_tracking_IMG/inREC2/%d%d"
// #define YARPPORT_IMG_RECVR_STREAMCAM "/2D_tracking_IMG/inSTREAM/%d%d"

// #define YARPPORT_2DPTS_RECVR "/2DPointsStream/in"

// #define YARPPORT_3DPTS_SEND "/3DPointsStream/out"
// #define YARPPORT_3DPTS_RECVR "/3DPointsStream/in"
// #define YARPPORT_3DPTS_RECVR_PLOT "/3DPointsStream/inPLot"

// #define YARPPORT_SEND_MILISECS "/3dctrlGTK/timestamp"

// #define YARPPORT_3DCTRLGTK_SEND_CENTRALCMD "/3dctrlGTK/centralcmd"
// #define YARPPORT_2DTRACK_RECVR_CENTRALCMD "/2dtrack/centralcmd/%d%d"

// #define YARPPORT_3DCTRLGTK_SEND_TIMESTAMP "/3dctrlGTK/timestamp"
// #define YARPPORT_TARGET_RECVR_TIMESTAMP "/target/timestamp/%d"

#define CMD_3DCTRLGTK_RESETBG 2
#define CMD_3DCTRLGTK_RECORD 3
#define CMD_3DCTRLGTK_RECORDTRACING 4
#define CMD_3DCTRLGTK_STOPRECORD 5

#define PROTOCOL "tcp"

class clsYarp
{
public:
	clsYarp();
	~clsYarp();

	bool initYarp();
	bool checkYarp();
	void connectPorts();
	
	bool init_yarp_2d(int sectionName,int cameraPosition);
	
	bool initYarp3Dsend();
	bool initYarp3Drcv(char *portName);
	
	bool send3Dpoint(pts3D point);
	bool rcv3Dpoint(pts3D *point);
	
	bool send_2dpts(pts2d point);
	bool rcv_2dpts(pts2d *point);
	bool rcv2DpointLock(pts2d *point);

	
	bool startCameraStreamOut(char *yarpPortName);
	
	#ifdef YARP_WITH_CV
	        bool startCameraStreamIn(char *yarpPortName);
	        bool cameraStreamRcv();
	        bool cameraStreamRcvLocked();
	        bool cameraStreamRcvLocked2();
	        bool cameraStreamSend(IplImage *cvImage);
	        IplImage *cvImage;
	        
	        int connectCalibrate3DCamera(int cameraSection,int cameraPosition);
	#endif
	
	void connectRemoteCamera(int cameraSection, int cameraPosition);
	
	bool connectPort(char *portIn,char *portOut,char *protocol= PROTOCOL);
	
	void listCameras();
	bool connectCameraCalibrate(int cameraNr,int *cameraSection,int *cameraPosition);
	

	bool initPort(char* name);
	bool initPortRcv(char* name,char *connectTo,char* protocol = PROTOCOL);
	bool initPortSend(char* name,char *connectTo,char* protocol = PROTOCOL);
	bool pendingReads();
	
	bool sendInt(int milisecs);
	bool rcvInt(int *mili);
	
	
	bool portIsOK;
	
	int milisecs;
	
	bool sendCommand(std::string cmd,int val = -1,int nparams = 0, std::string param[] = NULL);
	// bool rcvCommand(std::string *cmd,int *val,int *nparams, vector<std::string> *param);
	bool rcvCommand(strRemoteCommand *cmd);
	
	bool send(std::string str);
	bool rcv(std::string *str,bool locked = false);
	
	#ifdef YARP_USE_PARTICLES
	bool send(std::vector<clsParticle> particles);
	bool rcv(std::vector<clsParticle> *particles,bool locked = false);
	#endif
	
	bool checkConnection(std::string portIn,std::string portOut);
private:
	yarp::os::Network yarp;
	bool yarpInitiated;
	
	yarp::os::BufferedPort<yarp::os::Bottle> port;
	yarp::os::Bottle *input;
	
	yarp::os::BufferedPort<ImageOf<PixelRgb> > cameraStreamInPort;
	ImageOf<PixelRgb> *cameraStreamInYarpImage;
	bool firstAllocationRcv;
	
	yarp::os::BufferedPort<ImageOf<PixelRgb> > cameraStreamOutPort;
	
	yarp::os::BufferedPort<yarp::os::Bottle> points3DOutPort;
	yarp::os::BufferedPort<yarp::os::Bottle> points3DInPort;
	
protected:

};

#endif
