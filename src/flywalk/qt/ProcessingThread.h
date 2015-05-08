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
#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#define REQUIRE_YARP

// Qt
#include <QtCore/QThread>
#include <QtCore/QTime>
#include <QtCore/QQueue>
// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>
// Local
#include "Structures.h"
#include "Config.h"
#include "MatToQImage.h"
#include "SharedImageBuffer.h"

#include "fps.h"
#include "imgproc/tracking2d.h"
#include "yarp.h"

#include "imgproc/particles.h"

using namespace cv;

class ProcessingThread : public QThread
{
    Q_OBJECT

    public:
        ProcessingThread(SharedImageBuffer *sharedImageBuffer, int deviceNumber,int id);
        QRect getCurrentROI();
        void stop();
        
        clsTracking2D pTracking2D;
        clsYarp pYarpDatalogger;
        void setSettings(std::map<std::string,int> newParams);
        void setLinesSettings(std::vector<int> linesSettings);
        std::map<std::string,int> getSettings();
        
        void resetTracking();
        
        bool resizeToFitWindow;
    private:
        void setROI();
        void resetROI();
        SharedImageBuffer *sharedImageBuffer;
        Mat currentFrame;
        Rect currentROI;
        QImage frame;
        QMutex doStopMutex;
        QMutex processingMutex;
        Size frameSize;
        Point framePoint;
        struct ThreadStatisticsData statsData;
        volatile bool doStop;
        int tabId;
        bool enableFrameProcessing;
        int idx;
        clsFPS pfps;
        
        vector<clsParticle> currentParticles;
        
    protected:
        void run();

    private slots:
        void setROI(QRect roi);

    signals:
        void newFrame(const QImage &frame);
        void updateStatisticsInGUI(struct ThreadStatisticsData);
};

#endif // PROCESSINGTHREAD_H
