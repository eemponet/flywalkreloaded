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
#include "videorecord.h"

clsVideoRecord::clsVideoRecord()
{
	isRecording = false;
	videoWriterFile = 0;
	
	for(int i = 0;i<2;i++)
	{
		for(int j = 0;j<2;j++)
		{
			pYarpStream[i][j] = 0;
		}
	}
	
	fpsTimer.reset();
	fpsTimer.start();
	
}
clsVideoRecord::~clsVideoRecord()
{
	for(int i = 0;i<2;i++)
	{
		for(int j = 0;j<2;j++)
		{
			if(pYarpStream[i][j] != 0)
			{
				delete pYarpStream[i][j];
			}
		}
	}
}


bool clsVideoRecord::createNewFile(char *filename,CvSize imgSz)
{
	videoWriterFile = 0;
	snprintf(this->filename,MAXFILENAME_DATALOGGER,"%s",filename);
	// if((videoWriterFile = cvCreateVideoWriter(filename,CV_FOURCC('P','I','M','1'),fps,imgSz,1)) != 0)
	// {
		// DEBUGMSG(("USING 'P','I','M','1' filename: %s",filename));
		// return true;
	// }
	
	if((videoWriterFile = cvCreateVideoWriter(filename,CV_FOURCC('D','I','B',' '),fps,imgSz,1)) != 0)
	{
		DEBUGMSG(("USING 'D','I','B',' ' filename: %s",filename));
		return true;
	}
	
	if((videoWriterFile = cvCreateVideoWriter(filename,CV_FOURCC('M','P','4','A'),fps,imgSz,1)) != 0)
	{
		DEBUGMSG(("USING 'M','P','4','A' filename: %s",filename));
		return true;
	}
	
	if((videoWriterFile = cvCreateVideoWriter(filename,CV_FOURCC('M', 'J', 'P', 'G') ,fps,imgSz,1)) != 0)
	{
		DEBUGMSG(("USING 'M', 'J', 'P', 'G' filename: %s",filename));
		return true;
	}
	
	if((videoWriterFile = cvCreateVideoWriter(filename,CV_FOURCC('I','4','2','0') ,fps,imgSz,1)) != 0)
	{
		DEBUGMSG(("USING 'I','4','2','0' filename: %s",filename));
		return true;
	}
	
	
	ERRMSG(("Failed to createNewFile()")); 
	
	
}
bool clsVideoRecord::closeFile()
{
	return true;
}
bool clsVideoRecord::addNewFrame(IplImage *img)
{
	cvWriteFrame(videoWriterFile,img);
	
	// int timeElapsed = fpsTimer.getElapsedTime();
	// int timeToSleep = ((1000.0/(double)fps)-timeElapsed)*1000;
	// if(timeToSleep > 0)
	// {
		// usleep(timeToSleep);
	// }else
	// {
		// ERRMSG(("failed fps on video recording %s ... %d ",filename,timeElapsed));
	// }
	// 
	// fpsTimer.reset();
	// fpsTimer.start();
	
	return true;
}

void clsVideoRecord::record(char *filename,CvSize imgSz,int fpss)
{
	if(!isRecording)
	{
		fps = fpss;
		createNewFile(filename,imgSz);
	}
	
	isRecording = true;
}
void clsVideoRecord::stop()
{
	DEBUGMSG(("Stopping videorecorder"));
	if(isRecording)
	{
		if(videoWriterFile != 0)
		{
			cvReleaseVideoWriter(&videoWriterFile);
			videoWriterFile = 0;
			
			DEBUGMSG(("check your file in: %s",this->filename));
		}
	}
	isRecording = false;
	
}

void clsVideoRecord::composeAllCameraImages(IplImage *img)
{
	//cvSet(img,CV_RGB(0,0,0));
	
	/*for(int s = 0;s<2;s++)
	{
		for(int p = 0;p<2;p++)
		{
			if(pYarpStream[s][p]->cameraStreamRcvLocked())
			{
				if(s == 0)
				{
					cvSetImageROI(img, cvRect(CAMREC_WIDTH,CAMREC_HEIGHT*p,CAMREC_WIDTH,CAMREC_HEIGHT));
				}else
				{
					cvSetImageROI(img, cvRect(CAMREC_WIDTH,CAMREC_HEIGHT*p,CAMREC_WIDTH,CAMREC_HEIGHT));
				}
				
				cvResize(pYarpStream[s][p]->cvImage, img);
				cvResetImageROI(img);
			}
		}
	}
	return ;
	*/
	if(pYarpStream[0][0]->cameraStreamRcvLocked())
	{
		cvSetImageROI(img, cvRect(CAMREC_WIDTH,CAMREC_HEIGHT*0,CAMREC_WIDTH,CAMREC_HEIGHT));
		cvResize(pYarpStream[0][0]->cvImage, img);
		cvResetImageROI(img);
	}
	if(pYarpStream[0][1]->cameraStreamRcvLocked())
	{
		cvSetImageROI(img, cvRect(CAMREC_WIDTH,CAMREC_HEIGHT,CAMREC_WIDTH,CAMREC_HEIGHT));
		cvResize(pYarpStream[0][1]->cvImage, img);
		cvResetImageROI(img);
	}
	if(pYarpStream[1][0]->cameraStreamRcvLocked())
	{
		cvSetImageROI(img, cvRect(CAMREC_WIDTH*0,CAMREC_HEIGHT*0,CAMREC_WIDTH,CAMREC_HEIGHT));
		cvResize(pYarpStream[1][0]->cvImage, img);
		cvResetImageROI(img);
	}
	if(pYarpStream[1][1]->cameraStreamRcvLocked())
	{			
		cvSetImageROI(img, cvRect(CAMREC_WIDTH*0,CAMREC_HEIGHT,CAMREC_WIDTH,CAMREC_HEIGHT));
		cvResize(pYarpStream[1][1]->cvImage, img);
		cvResetImageROI(img);
	}
	
	
}

void clsVideoRecord::connectCameraImages(char *portIN, char *portOUT)
{
	char camPortOut[128],camPortIn[128];
	for(int i = 0;i<2;i++)
	{
		for(int j = 0;j<2;j++)
		{
			pYarpStream[i][j] = new clsYarp();
			sprintf(camPortOut,portOUT,i,j);
			
			if(!pYarpStream[i][j]->startCameraStreamIn(camPortOut)) {
				ERRMSG(("Failed to initiate yarpImgReceiver"));
			}
			sprintf(camPortIn,portIN,i,j);
			pYarpStream[i][j]->connectPort(camPortIn,camPortOut);
			INFOMSG(("new port created %s and connected to %s ",camPortOut,camPortIn));
		}
	}
}
void clsVideoRecord::addTimerTrial(IplImage *img,char *str)
{
	CvFont font;
	//cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX, 0.5,0.5,0,1);
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX , 1,0.1,1,1,4);
	cvPutText(img,str,cvPoint((CAMREC_WIDTH*2)-100,(CAMREC_HEIGHT*2)-30),&font,cvScalar(255,0,0));
}

// bool clsVideoRecord::addNewFrameOnMovement(IplImage *img)
// {
	// if(isRecording && videoWriterFile != 0)
	// {
		// cvWriteFrame(videoWriterFile,img);
		// return true;
	// }
	// 
	// return false;
// }
