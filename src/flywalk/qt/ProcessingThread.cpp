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
#include "ProcessingThread.h"
#include "yarpports.h"

ProcessingThread::ProcessingThread(SharedImageBuffer *sharedImageBuffer, int tabIdx,int id) : QThread(), sharedImageBuffer(sharedImageBuffer)
{
	// Save Device Number
	tabId=tabIdx;
	// Initialize members
	doStop=false;
	
	pfps = clsFPS();
	
	statsData.averageFPS=0;
	statsData.nFramesProcessed=0;
	
	idx = id;
	
	resizeToFitWindow = false;
}

void ProcessingThread::run()
{
	pfps.resetCount();
	
	if(!pYarpDatalogger.initPort(YARPPORT_SEND_DATALOGGER))
	{
	        ERRMSG(("Error on initializing yarp, is yarp running?"));
	}
	
	pYarpDatalogger.connectPort(YARPPORT_SEND_DATALOGGER,YARPPORT_RCV_DATALOGGER,"udp");
	
	while(1)
	{
		////////////////////////////////
		// Stop thread if doStop=TRUE //
		////////////////////////////////
		DEBUGMSG(("."));
		doStopMutex.lock();
		if(doStop)
		{
			doStop=false;
			doStopMutex.unlock();
			break;
		}
		
		doStopMutex.unlock();
		/////////////////////////////////
		/////////////////////////////////
		DEBUGMSG(("."));
		if(sharedImageBuffer->getSize(idx) > 0)
		{
		        DEBUGMSG(("."));
		        currentFrame=Mat(sharedImageBuffer->get(idx));
		        DEBUGMSG(("."));
		        processingMutex.lock();
		        DEBUGMSG(("."));
		        ////////////////////////////////////
		        // PERFORM IMAGE PROCESSING BELOW //
		        ////////////////////////////////////
		        pTracking2D.setFrameCaptured(currentFrame);
		        currentFrame = pTracking2D.getProcessedImage();
		        ////////////////////////////////////
		        // PERFORM IMAGE PROCESSING ABOVE //
		        ////////////////////////////////////
		        DEBUGMSG(("."));
		        std::string particlesStr;
		        // 
		        // if(pTracking2D.getParticles().size() > 0)pYarpDatalogger.send(pTracking2D.getParticles());
		        if(pTracking2D.getParticlesToString(&particlesStr))
		        {
		                DEBUGMSG(("."));
		                pTracking2D.getParticlesToString(&particlesStr);
		                DEBUGMSG(("."));
		                pYarpDatalogger.send(particlesStr);
		        }
		        DEBUGMSG(("."));
		        // Convert Mat to QImage
		        if(resizeToFitWindow)
		        {
		                DEBUGMSG(("."));
		                cv::resize(currentFrame, currentFrame, cvSize( 640,480));
		        }
		        DEBUGMSG(("."));
		        frame=MatToQImage(currentFrame);
		        
		        processingMutex.unlock();
		        DEBUGMSG(("."));
		        
		        
		        // Inform GUI thread of new frame (QImage)
		        emit newFrame(frame);
		        DEBUGMSG(("."));
		        // Update statistics
		        statsData.averageFPS = pfps.updateFPS();
		        statsData.nFramesProcessed++;
		        DEBUGMSG(("."));
		        // Inform GUI of updated statistics
		        emit updateStatisticsInGUI(statsData);
		        DEBUGMSG(("."));
		}
		DEBUGMSG(("."));
	}
	DEBUGMSG(("."));
	INFOMSG(( "Stopping processing thread... %d",idx));
}

void ProcessingThread::stop()
{
	QMutexLocker locker(&doStopMutex);
	doStop=true;
}
void ProcessingThread::resetTracking()
{
        processingMutex.lock();
        
        pTracking2D.resetTracking();
        
        processingMutex.unlock();
}
void ProcessingThread::setROI(QRect roi)
{
	processingMutex.lock();
	
	currentROI.x = roi.x();
	currentROI.y = roi.y();
	currentROI.width = roi.width();
	currentROI.height = roi.height();
	
	if(currentROI.x < 0)
	{
	        currentROI.x = 0;
	}
	if(currentROI.y < 0)
	{
	        currentROI.y = 0;
	}
	
	pTracking2D.setROI(roi.x(),roi.y(),roi.width(),roi.height());
	processingMutex.unlock();
}

QRect ProcessingThread::getCurrentROI()
{
	return QRect(currentROI.x, currentROI.y, currentROI.width, currentROI.height);
}

void ProcessingThread::setSettings(std::map<std::string,int> newParams)
{
        currentROI.x = newParams["roiX"];
        currentROI.y = newParams["roiY"];
        currentROI.width = newParams["roiW"];
        currentROI.height = newParams["roiH"];
        
	pTracking2D.setSettings(newParams);
}

std::map<std::string,int> ProcessingThread::getSettings()
{
	return pTracking2D.getSettings();
}

void ProcessingThread::setLinesSettings(std::vector<int> linesSets)
{
        pTracking2D.setLines(linesSets);
}
