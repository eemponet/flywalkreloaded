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
#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

// Qt
#include <QtCore/QTime>
#include <QtCore/QThread>
// OpenCV
#include "highgui.h"
// Local
#include "SharedImageBuffer.h"
#include "Config.h"
#include "Structures.h"

#include "fps.h"
#include "imgproc/tracking2d.h"

using namespace cv;

class ImageBuffer;

class CaptureThread : public QThread
{
	Q_OBJECT
	
public:
        CaptureThread(SharedImageBuffer *sharedImageBuffer,int tabIdx, bool dropFrameIfBufferFull,int id);
        void stop();
        bool connectToCamera(int deviceNumber,int idxResolution);
        bool connectVideo(std::string filename);
        bool disconnectCamera();
        // bool isCameraConnected();
        int getInputSourceWidth();
        int getInputSourceHeight();
        
        clsVideoCapture pVideoCapture;
        
        void setVideoResolution(int w, int h);
        int wVideo, hVideo;
        bool resizeSet;
        
private:
	clsFPS pfps;
	
	int tabId;
	int idx;
        SharedImageBuffer *sharedImageBuffer;
        
        Mat grabbedFrame;
        
        QMutex doStopMutex;
        struct ThreadStatisticsData statsData;
        volatile bool doStop;
        
        bool dropFrameIfBufferFull;
        
        int adjustFrameSpeed;
        
protected:
        void run();
        
signals:
        void updateStatisticsInGUI(struct ThreadStatisticsData);
};

#endif // CAPTURETHREAD_H
