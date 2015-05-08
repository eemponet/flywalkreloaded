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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
#include <QPushButton>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>

//Boost
#include <boost/lexical_cast.hpp>

// Local
#include "CameraView.h"
#include "SharedImageBuffer.h"
#include "camerav4l.h"

#define VIDEO_CAPTURE 0
#define VIDEO_FILE    1
#define MAX_VIDEOS 1

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;
        QPushButton *connectToCameraButton;
        // QMap<int, int> deviceNumberMap;
        // QMap<int, int> tabVideoType;
        
        QMap<int, CameraView*> cameraViewMap; //TAB ID - cameraView
        std::string openedVideos[MAX_VIDEOS];
        SharedImageBuffer *sharedImageBuffer;
        
        QMap<int, int> deviceConfigMap;
        QMap<int, int> tabDeviceMap;
        
        // QString defaultCamDev;
        // QString settingsFile;
        int currentTabId;
        
        
        bool removeFromMapByTabIndex(QMap<int, int>& map, int tabIndex);
        void updateMapValues(QMap<int, int>& map, int tabIndex);
        void setTabCloseToolTips(QTabWidget *tabs, QString tooltip);
        
        void readSettings();
        
        std::map<std::string,int>  processingSettings;
        std::string processingSettingsFile;
        std::string currentVideo;
        void keyPressEvent( QKeyEvent *event );
         
        int newTab(QString tabLabel);
        void startVideo(std::string filename);
        void updateToolbar();
        void autoStartConfiguration();
        
        std::vector<resolution> resolutions;
        
        QSpinBox *spinboxXmm;
        QSpinBox *spinboxYmm;
        
        QSpinBox *spinboxRoiX;
        QSpinBox *spinboxRoiY;
        QSpinBox *spinboxRoiW;
        QSpinBox *spinboxRoiH;
        
        QGroupBox *bgCodeBook;
        QGroupBox *bgColorSubtraction;
        QGroupBox *bgColorSegmentation;
        
        std::vector<videoCapControl> camControls;
        QMap<QString, int> suffixIdCamControls;
        
        void validateROI();
        
        void loadCameraAdjustSettings(int deviceId,int cfgId);
        
    public slots:
        void disconnectCamera(int index);
        void showAboutDialog();
        void startCam(int deviceNumber);
        void updateConfigMenu();
        void handleResolutionChange(QAction *action);
        void toggleProcessingAdjustToolbar();
        void toggleCameraAdjustToolbar();
        void updateBlur(int value);
        void resetBG();
        void updateBGcalc(int value);
        void updateErosion(int value);
        void updatemaxLimboFrameLost(int value);
        void updatemaxDistanceNeigbor(int value);
        void openFile();
        void changeToDebugFrame(bool val);
        void restartVideo();
        void updateMinThreshold(int value);
        void updateMaxThreshold(int value);
        void updatemaxDistanceNeigborX(int value);
        void updatemaxDistanceNeigborY(int value);
        void changeTraces(bool val);
        void writeSettings();
        void updateFrameRate(int value);
        void setCurrentTab(int tabId);
        void setCalibrationX(int val);
        void setCalibrationY(int val);
        void setCamerasMenu();
        void loadLinesSettings();
        void quit();
        void updateCameraAdjustToolbar(bool visible);
        
        void resetROI();
        void setRoiX(int x);
        void setRoiY(int val);
        void setRoiW(int val);
        void setRoiH(int val);
        
        void newROI(QRect roi);
        
        void updateCameraSetting(int val);
        
        void updateMinH(int val);
        void updateMinS(int val);
        void updateMinL(int val);
private slots:
};

#endif // MAINWINDOW_H
