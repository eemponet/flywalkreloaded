/*#    This file is part of Flywalk Reloaded. <http://flywalk.eempo.net>
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
#include "CaptureThread.h"
#include "camerav4l.h"

CaptureThread::CaptureThread(SharedImageBuffer *sharedImageBuffer,int tabIdx, bool dropFrameIfBufferFull,int id) : QThread(), sharedImageBuffer(sharedImageBuffer)
{
	// Save passed parameters
	this->dropFrameIfBufferFull=dropFrameIfBufferFull;
	
	// Initialize variables(s)
	doStop=false;
	
	tabId = tabIdx;
	
	statsData.averageFPS=0;
	statsData.nFramesProcessed=0;
	
	pfps = clsFPS();
	
	idx = id;
	
	resizeSet = false;
	
	adjustFrameSpeed = 500;
}

void CaptureThread::run()
{
	pfps.resetCount();
	DEBUGMSG(("."));
	while(1)
	{
	        // Inform GUI of updated statistics
		emit updateStatisticsInGUI(statsData);
		DEBUGMSG(("."));
		////////////////////////////////
		// Stop thread if doStop=TRUE //
		////////////////////////////////
		doStopMutex.lock();
		if(doStop)
		{
			doStop=false;
			doStopMutex.unlock();
			break;
		}
		doStopMutex.unlock();
		DEBUGMSG(("."));
		/////////////////////////////////
		/////////////////////////////////
		
		//get frame
		if(!pVideoCapture.capture())
		{
		        usleep(5000);
			WARNMSG(("Capture failed"));
			DEBUGMSG(("."));
			continue;
		}
		DEBUGMSG(("."));
		grabbedFrame = pVideoCapture.getCaptureImage();
		DEBUGMSG(("."));
		if(resizeSet)
		{
		        cv::resize(grabbedFrame, grabbedFrame, cvSize( wVideo,hVideo));
		        DEBUGMSG(("."));
		}
		DEBUGMSG(("."));
		// Resize src to img size
		// cv::resize(grabbedFrame, dst, cvSize( 800,600));
		// cv::resize(grabbedFrame, dst, cvSize( 640,480));

		// Add frame to buffer
		// sharedImageBuffer->add(idx,dst);
		if(sharedImageBuffer->getSize(idx) > sharedImageBuffer->maxSize()-2)
		{
		        DEBUGMSG(("."));
		        usleep(adjustFrameSpeed);
		        INFOMSG(("increasing sleep btw frames: %d",adjustFrameSpeed));
		        //adjustFrameSpeed = adjustFrameSpeed + 10000;
		        //sharedImageBuffer->get(idx);
		        continue;
		}
		DEBUGMSG(("."));
		sharedImageBuffer->add(idx,grabbedFrame);
		DEBUGMSG(("."));
		// Update statistics
		statsData.averageFPS = pfps.updateFPS();
		statsData.nFramesProcessed++;
		DEBUGMSG(("."));
	}
	INFOMSG(("Stopping capture thread... %d",idx));
}

bool CaptureThread::connectToCamera(int deviceNumber,int idxResolution)
{
	int width, height;
	std::string res[100];
	
	if(!getResolution(deviceNumber,idxResolution,width,height))
	{
	        WARNMSG(("could not load resolution"));
	}
	INFOMSG(("Connecting new camera with res %dx%d",width,height));
	
	return pVideoCapture.startVideoCapture(deviceNumber,width,height);
}
bool CaptureThread::connectVideo(std::string filename)
{
	return pVideoCapture.startVideo(filename);
}
bool CaptureThread::disconnectCamera()
{
        INFOMSG(("releasing camera..."));
	return pVideoCapture.close();
}

void CaptureThread::stop()
{
        // INFOMSG(("called stop() capture thread... %d",idx));
        // pVideoCapture.close();
	QMutexLocker locker(&doStopMutex);
	doStop=true;
}

int CaptureThread::getInputSourceWidth()
{
	return pVideoCapture.width();
}

int CaptureThread::getInputSourceHeight()
{
	return pVideoCapture.height();
}

void CaptureThread::setVideoResolution(int w,int h)
{
        wVideo = w;
        hVideo = h;
        
        resizeSet = true;
}
