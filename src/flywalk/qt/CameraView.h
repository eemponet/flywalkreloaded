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
#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QWidget>

// Local
#include "CaptureThread.h"
#include "ProcessingThread.h"
#include "Structures.h"
#include "SharedImageBuffer.h"
#include "yarp.h"

namespace Ui {
    class CameraView;
}
static int id = 0;
class CameraView : public QWidget
{
    Q_OBJECT

    public:
        explicit CameraView(QWidget *mainwindow, QWidget *parent,int tabId, SharedImageBuffer *sharedImageBuffer);
        ~CameraView();
        bool connectToCamera(int deviceNumber,int idxResolution = 0);
        bool connectToVideo(std::string filename);
        void resetBG();
        
        void setProcessingSettings(std::map<std::string,int> newParams);
        void setLinesSettings(std::vector<int> linesSettings);
        std::map<std::string,int> getProcessingSettings();
        
        void setVideoResolution(int width,int height);
        bool isVideo;
        int widthVideo;
        int heightVideo;
        
        void loadLinesSettings();
        
        std::map<std::string,double> getCamSpecs();
        
        int capW, capH;
    private:
        Ui::CameraView *ui;
        ProcessingThread *processingThread;
        CaptureThread *captureThread;
        SharedImageBuffer *sharedImageBuffer;
        ImageProcessingFlags imageProcessingFlags;
        void stopCaptureThread();
        void stopProcessingThread();
        
        void connectStream();
        
        bool isCameraConnected;
        int tabId;
        
        clsYarp pYarpDataloggerCtrl;
        bool recordingDlg;
        
    public slots:
        void newMouseData(struct MouseData mouseData);
        void updateMouseCursorPosLabel();
        void clearImageBuffer();
        void startStopDatalogger();
        
    private slots:
        void updateFrame(const QImage &frame);
        void updateProcessingThreadStats(struct ThreadStatisticsData statData);
        void updateCaptureThreadStats(struct ThreadStatisticsData statData);
        void handleContextMenuAction(QAction *action);
    signals:
        void setROI(QRect roi);
};

#endif // CAMERAVIEW_H
