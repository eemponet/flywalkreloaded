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
#include "MainWindow.h"
#include "ui_MainWindow.h"
// Qt
#include <QLabel>
#include <QMessageBox>
#include <QSignalMapper>
#include <QSettings>
#include <QToolBar>
#include <QRadioButton>
#include <QFileDialog>
#include <QPixmap> 
#include <QVBoxLayout>
#include <sys/stat.h>

#include "Settings.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
        
	// Setup UI
	ui->setupUi(this);
	
	this->setWindowTitle(QString(FLYWALK_VERSION));
	// Set start tab as blank
	currentTabId = -1;
	
	QLabel *newTab = new QLabel(ui->tabWidget);
	newTab->setText("No camera connected.");
	newTab->setAlignment(Qt::AlignCenter);
	ui->tabWidget->addTab(newTab, "");
	ui->tabWidget->setTabsClosable(false);
	// Add "Connect to Camera" button to tab
	// connectToCameraButton->setText("Connect to Camera...");
	// ui->tabWidget->setCornerWidget(connectToCameraButton, Qt::TopLeftCorner);
	// connect(connectToCameraButton,SIGNAL(released()),this, SLOT(connectToCamera()));
	connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this, SLOT(disconnectCamera(int)));
	// Set focus on button
	// connectToCameraButton->setFocus();
	// Connect other signals/slots
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
	connect(ui->actionSaveSettings, SIGNAL(triggered()), this, SLOT(writeSettings()));
	connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setCurrentTab(int)));
	connect(ui->menuSetResolution, SIGNAL(triggered(QAction*)), this, SLOT(handleResolutionChange(QAction*)));
	connect(ui->toggleProcessingAdjust, SIGNAL(triggered()), this, SLOT(toggleProcessingAdjustToolbar()));
	connect(ui->toggleCameraAdjust, SIGNAL(triggered()), this, SLOT(toggleCameraAdjustToolbar()));
	connect(ui->menuConfig, SIGNAL(aboutToShow()), this, SLOT(updateConfigMenu()));
	connect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(setCamerasMenu()));
	connect(ui->actionOpenFile, SIGNAL(triggered()), this, SLOT(openFile()));
	
	connect(ui->toolbarCameraAdjust,SIGNAL(visibilityChanged(bool)),this,SLOT(updateCameraAdjustToolbar(bool)));
	
	toggleProcessingAdjustToolbar();
	toggleCameraAdjustToolbar();
	
	// Create SharedImageBuffer object
	sharedImageBuffer = new SharedImageBuffer();
	
	processingSettingsFile = SETTINGS_PROC_FILE;
	
	readSettings();
	updateToolbar();
	autoStartConfiguration();
	// startRemoteControl();
	
	// showAboutDialog();
}


MainWindow::~MainWindow()
{
	// writeSettings();
	// stopRemoteControl();
	
        
	delete ui;
}

void MainWindow::updateToolbar()
{
	// QSlider *slider;
	QPushButton *button;
	QLabel *label;
	QSpinBox *spinbox;
	QComboBox *combobox;
        QVBoxLayout *vbox;
	/*slider = new QSlider (Qt::Horizontal);
	slider->setMinimum(0);
	slider->setMaximum(60);
	slider->setValue(processingSettings["maxLimboFrameLost"]);
	ui->toolbarProcessingAdjust->addWidget(new QLabel("maxLimboFrameLost"));
	ui->toolbarProcessingAdjust->addWidget(slider);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updatemaxLimboFrameLost(int)));
	*/
	
	/*slider = new QSlider (Qt::Horizontal);
	slider->setMinimum(4);
	slider->setMaximum(40);
	ui->toolbarProcessingAdjust->addWidget(new QLabel("maxDistanceNeigbor"));
	slider->setValue(processingSettings["maxDistanceNeigbor"]);
	ui->toolbarProcessingAdjust->addWidget(slider);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updatemaxDistanceNeigbor(int)));
	*/
	
	label = new QLabel(this);
	label->setText("\nOutput Controls:\n");
	label->setAlignment(Qt::AlignLeft);
	ui->toolbarProcessingAdjust->addWidget(label);
	
	button = new QPushButton("Toggle Foreground");
	button->setCheckable(true);
	ui->toolbarProcessingAdjust->addWidget(button);
	connect(button, SIGNAL(toggled(bool)), this, SLOT(changeToDebugFrame(bool)));
	
	button = new QPushButton("Toggle traces");
	button->setCheckable(true);
	ui->toolbarProcessingAdjust->addWidget(button);
	if(processingSettings["showTraces"] > 0) button->setChecked(true);
	connect(button, SIGNAL(toggled(bool)), this, SLOT(changeTraces(bool)));
	
	label = new QLabel(this);
	label->setText("\nBackground Calibration:\n");
	label->setAlignment(Qt::AlignLeft);
	ui->toolbarProcessingAdjust->addWidget(label);
	
	combobox = new QComboBox(this);
	// combobox->setText("\nBackground :\n");
	combobox->addItem("BG_BGCodeBookModel");
	combobox->addItem("BG_COLORSUBTRACTION");
	combobox->addItem("BG_SEG");
	combobox->setCurrentIndex(processingSettings["bg"]);
	ui->toolbarProcessingAdjust->addWidget(combobox);
	connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBGcalc(int)));
	
	/**
	BG CODE BOOK 
	**/
	
	/**
	BG Color Subtraction 
	**/
	vbox = new QVBoxLayout();
	bgColorSegmentation = new QGroupBox();
	
	// spinbox = new QSpinBox();
	// spinbox->setMinimum(0);
	// spinbox->setMaximum(50);
	// spinbox->setPrefix(QString("Blur: "));
	// spinbox->setValue(processingSettings["blur"]);
	// // ui->toolbarProcessingAdjust->addWidget(spinbox);
	// vbox->addWidget(spinbox);
	// connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateBlur(int)));
	
	// spinbox = new QSpinBox();
	// spinbox->setMinimum(0);
	// spinbox->setMaximum(255);
	// spinbox->setPrefix(QString("Erosion: "));
	// spinbox->setValue(processingSettings["erosion"]);
	// // ui->toolbarProcessingAdjust->addWidget(spinbox);
	// vbox->addWidget(spinbox);
	// connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateErosion(int)));
	
	spinbox = new QSpinBox();
	spinbox->setMinimum(0);
	spinbox->setMaximum(255);
	spinbox->setPrefix(QString("minH: "));
	spinbox->setValue(processingSettings["minH"]);
	// ui->toolbarProcessingAdjust->addWidget(spinbox);
	vbox->addWidget(spinbox);
	connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateMinH(int)));
	
	spinbox = new QSpinBox();
	spinbox->setMinimum(0);
	spinbox->setMaximum(255);
	spinbox->setPrefix(QString("minS: "));
	spinbox->setValue(processingSettings["minS"]);
	// ui->toolbarProcessingAdjust->addWidget(spinbox);
	vbox->addWidget(spinbox);
	connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateMinS(int)));
	
	spinbox = new QSpinBox();
	spinbox->setMinimum(0);
	spinbox->setMaximum(255);
	spinbox->setPrefix(QString("minL: "));
	spinbox->setValue(processingSettings["minL"]);
	// ui->toolbarProcessingAdjust->addWidget(spinbox);
	vbox->addWidget(spinbox);
	connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateMinL(int)));
	
	vbox->addStretch(1);
	bgColorSegmentation->setLayout(vbox);
	ui->toolbarProcessingAdjust->addWidget(bgColorSegmentation);
	/**
	BG SEGMENTATION 
	**/
	vbox = new QVBoxLayout();
	bgColorSegmentation = new QGroupBox();
	
	spinbox = new QSpinBox();
	spinbox->setMinimum(0);
	spinbox->setMaximum(50);
	spinbox->setPrefix(QString("Blur: "));
	spinbox->setValue(processingSettings["blur"]);
	// ui->toolbarProcessingAdjust->addWidget(spinbox);
	vbox->addWidget(spinbox);
	connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateBlur(int)));
	
	spinbox = new QSpinBox();
	spinbox->setMinimum(0);
	spinbox->setMaximum(255);
	spinbox->setPrefix(QString("Erosion: "));
	spinbox->setValue(processingSettings["erosion"]);
	// ui->toolbarProcessingAdjust->addWidget(spinbox);
	vbox->addWidget(spinbox);
	connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateErosion(int)));
	
	spinbox = new QSpinBox();
	spinbox->setMinimum(0);
	spinbox->setMaximum(255);
	spinbox->setPrefix(QString("Threshold min: "));
	spinbox->setValue(processingSettings["minThreshold"]);
	// ui->toolbarProcessingAdjust->addWidget(spinbox);
	vbox->addWidget(spinbox);
	connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateMinThreshold(int)));
	
	spinbox = new QSpinBox();
	spinbox->setMinimum(0);
	spinbox->setMaximum(255);
	spinbox->setPrefix(QString("Threshold max: "));
	spinbox->setValue(processingSettings["maxThreshold"]);
	// ui->toolbarProcessingAdjust->addWidget(spinbox);
	vbox->addWidget(spinbox);
	connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateMaxThreshold(int)));
	
	vbox->addStretch(1);
	bgColorSegmentation->setLayout(vbox);
	ui->toolbarProcessingAdjust->addWidget(bgColorSegmentation);
	
	
	
	
	button = new QPushButton("Reset background");
	ui->toolbarProcessingAdjust->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(resetBG()));
	
	label = new QLabel(this);
	label->setText("\nOther Calibrations:\n");
	label->setAlignment(Qt::AlignLeft);
	ui->toolbarProcessingAdjust->addWidget(label);
	
	button = new QPushButton("Reload lines calibration");
	ui->toolbarProcessingAdjust->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(loadLinesSettings()));
	
	button = new QPushButton("Reset ROI");
	ui->toolbarProcessingAdjust->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(resetROI()));
	
	spinboxRoiX = new QSpinBox();
	// ui->toolbarProcessingAdjust->addWidget(new QLabel("ROI x"));
	spinboxRoiX->setPrefix(QString("ROI x: "));
	spinboxRoiX->setSuffix(QString("px"));
	ui->toolbarProcessingAdjust->addWidget(spinboxRoiX);
	spinboxRoiX->setMaximum(6000);
	spinboxRoiX->setValue(processingSettings["roiX"]);
	connect(spinboxRoiX, SIGNAL(valueChanged(int)), this, SLOT(setRoiX(int)));
	
	spinboxRoiY = new QSpinBox();
	// ui->toolbarProcessingAdjust->addWidget(new QLabel("ROI y"));
	spinboxRoiY->setPrefix(QString("ROI y: "));
	spinboxRoiY->setSuffix(QString("px"));
	ui->toolbarProcessingAdjust->addWidget(spinboxRoiY);
	spinboxRoiY->setMaximum(6000);
	spinboxRoiY->setValue(processingSettings["roiY"]);
	connect(spinboxRoiY, SIGNAL(valueChanged(int)), this, SLOT(setRoiY(int)));
	
	spinboxRoiW = new QSpinBox();
	// ui->toolbarProcessingAdjust->addWidget(new QLabel("ROI width"));
	spinboxRoiW->setPrefix(QString("ROI width: "));
	spinboxRoiW->setSuffix(QString("px"));
	ui->toolbarProcessingAdjust->addWidget(spinboxRoiW);
	spinboxRoiW->setMaximum(6000);
	spinboxRoiW->setValue(processingSettings["roiW"]);
	connect(spinboxRoiW, SIGNAL(valueChanged(int)), this, SLOT(setRoiW(int)));
	
	spinboxRoiH = new QSpinBox();
	// ui->toolbarProcessingAdjust->addWidget(new QLabel("Roi height"));
	spinboxRoiH->setPrefix(QString("ROI height: "));
	spinboxRoiH->setSuffix(QString("px"));
	ui->toolbarProcessingAdjust->addWidget(spinboxRoiH);
	spinboxRoiH->setMaximum(6000);
	spinboxRoiH->setValue(processingSettings["roiH"]);
	connect(spinboxRoiH, SIGNAL(valueChanged(int)), this, SLOT(setRoiH(int)));
	
	// ui->toolbarProcessingAdjust->addWidget(new QLabel("x Milimiters"));
	spinboxXmm = new QSpinBox();
	spinboxXmm->setPrefix(QString("ROI width: "));
	spinboxXmm->setSuffix(QString("mm"));
	spinboxXmm->setMaximum(50000);
	spinboxXmm->setValue(processingSettings["xMm"]);
	ui->toolbarProcessingAdjust->addWidget(spinboxXmm);
	connect(spinboxXmm, SIGNAL(valueChanged(int)), this, SLOT(setCalibrationX(int)));
	
	// ui->toolbarProcessingAdjust->addWidget(new QLabel("y Milimiters"));
	spinboxYmm = new QSpinBox();
	spinboxYmm->setPrefix(QString("ROI height: "));
	spinboxYmm->setSuffix(QString("mm"));
	spinboxYmm->setMaximum(50000);
	spinboxYmm->setValue(processingSettings["yMm"]);
	ui->toolbarProcessingAdjust->addWidget(spinboxYmm);
	connect(spinboxYmm, SIGNAL(valueChanged(int)), this, SLOT(setCalibrationY(int)));
	
}
void MainWindow::disconnectCamera(int index)
{
	
	// Save number of tabs
	int nTabs = ui->tabWidget->count();
	// Close tab
	ui->tabWidget->removeTab(index);
	
	DEBUGMSG(("starting to stop cameraView...!! "));
	
	// Delete widget (CameraView) contained in tab
	delete cameraViewMap[index];
	cameraViewMap.remove(index);
	
	// If start tab, set tab as blank
	if(nTabs==1)
	{
		QLabel *newTab = new QLabel(ui->tabWidget);
		newTab->setText("No camera connected.");
		newTab->setAlignment(Qt::AlignCenter);
		ui->tabWidget->addTab(newTab, "");
		ui->tabWidget->setTabsClosable(false);
		ui->actionSynchronizeStreams->setEnabled(true);
		
		currentTabId = -1;
	}
	
}

void MainWindow::showAboutDialog()
{
	// QMessageBox about(QMessageBox::NoIcon,"About", QString("Software: ").append(
	        // FLYWALK_VERSION).append("\nCopyright: Electricidade Em Pó\nAuthor: Pedro Gouveia\n\nContact: pgouveia@eempo.net\n"));
	
	QLabel* label = new QLabel();
	QImage image(QString(":/eep.png"));
	label->setPixmap(QPixmap::fromImage(image));
	
	QWidget *widget = new QWidget();
	widget->setWindowTitle(QString("About..."));
	QVBoxLayout * layout = new QVBoxLayout( widget );
	layout->addWidget( new QLabel(QString("Software: ").append(FLYWALK_VERSION)),Qt::AlignHCenter );
	layout->addWidget( new QLabel(QString("Copyright: Electricidade Em Pó")),Qt::AlignHCenter );
	layout->addWidget( new QLabel(QString("Contact:")),Qt::AlignHCenter );
	layout->addWidget( label );
	 
	widget->show();
}

void MainWindow::updateMapValues(QMap<int, int> &map, int tabIndex)
{
	QMutableMapIterator<int, int> i(map);
	while (i.hasNext())
	{
		i.next();
		if(i.value()>tabIndex)
			i.setValue(i.value()-1);
	}
}

void MainWindow::setTabCloseToolTips(QTabWidget *tabs, QString tooltip)
{
	QList<QAbstractButton*> allPButtons = tabs->findChildren<QAbstractButton*>();
	for (int ind = 0; ind < allPButtons.size(); ind++)
	{
		QAbstractButton* item = allPButtons.at(ind);
		if (item->inherits("CloseButton"))
			item->setToolTip(tooltip);
	}
}

void MainWindow::setCamerasMenu()
{
	// QMenu* submenuCams = ui->menuFile->addMenu("Start new camera");
	QAction* submenuCam[MAXNUM_CAMS];
	char tmpstr[50];
	int i = 0;
	ui->menuStartNewCamera->clear();
	
	camsMap cards;
	getCamDevices(cards);
	QSignalMapper *signalMapper = new QSignalMapper( this );
	
	for (camsMap::iterator iter = cards.begin();iter != cards.end(); ++iter) {
		sprintf(tmpstr,"%s (/dev/video%d)",iter->second.c_str(),iter->first);
		submenuCam[i] = new QAction(this);
		submenuCam[i] = ui->menuStartNewCamera->addAction( tmpstr );
		signalMapper->setMapping( submenuCam[i], iter->first);
		connect( submenuCam[i], SIGNAL(triggered()), signalMapper, SLOT(map()) );
		i++;
	}
	
	connect( signalMapper, SIGNAL(mapped(int)), SLOT(startCam(int)) );
}
void MainWindow::restartVideo()
{
        if(cameraViewMap[currentTabId]->isVideo)
        {
                if(strlen(currentVideo.c_str()) > 1)
                        startVideo(currentVideo);
	}
}
void MainWindow::startVideo(std::string filename)
{
	currentVideo = filename;
	
	int newTabId;
	
	newTabId = newTab(  QString::fromStdString(filename) );
	
        DEBUGMSG(("loading video: %s", filename.c_str() ));
	
	if(!cameraViewMap[newTabId]->connectToVideo(filename))
	{
	        ERRMSG(("Failed to connect video"));
	}
	
	cameraViewMap[newTabId]->setProcessingSettings(processingSettings);
	
	DEBUGMSG(("Connected video: %s", filename.c_str() ));
}

void MainWindow::startCam(int deviceNumber)
{
	
	int newTabId;
	int idxResolution;
	
	std::string tabLabl;
	getCamBusName(deviceNumber,&tabLabl[0]);
	
	loadSetting("resolution",deviceConfigMap[deviceNumber] ,idxResolution);
	loadCameraAdjustSettings(deviceNumber,deviceConfigMap[deviceNumber]);
	
	newTabId = newTab(QString("%1 ( /dev/video%2 )").arg(tabLabl.c_str()).arg(deviceNumber));
	
	tabDeviceMap[newTabId] = deviceNumber;
	cameraViewMap[newTabId]->connectToCamera(deviceNumber,idxResolution);
	validateROI();
	cameraViewMap[newTabId]->setProcessingSettings(processingSettings);
	
	
	// updateConfigMenu(newTabId);
	setCurrentTab(newTabId);
	DEBUGMSG(("Connected camera: %s", qPrintable(QString("%1 ( /dev/video%2 )").arg(tabLabl.c_str()).arg(deviceNumber)) ));
	
	
}

int MainWindow::newTab(QString tabLabel)
{
        if(cameraViewMap.size() > 0)
                disconnectCamera(0); //restrict to 1 tab!!
	
	int nextTabIndex = cameraViewMap.size();
	
	cameraViewMap[nextTabIndex] = new CameraView(this,ui->tabWidget, nextTabIndex, sharedImageBuffer);
	
	// Allow tabs to be closed
	ui->tabWidget->setTabsClosable(true);
	// If start tab, remove
	if(nextTabIndex==0)
		ui->tabWidget->removeTab(0);
	
	ui->tabWidget->addTab(cameraViewMap[nextTabIndex], tabLabel);
	
	ui->tabWidget->setCurrentWidget(cameraViewMap[nextTabIndex]);
	
	return nextTabIndex;
}


void MainWindow::writeSettings()
{
        INFOMSG(("Saving new settings"));
        std::map<std::string,int> paramsFrom2dTrack = cameraViewMap[currentTabId]->getProcessingSettings();
        for (std::map<std::string,int>::iterator itr = paramsFrom2dTrack.begin();itr != paramsFrom2dTrack.end(); ++itr) 
        {
                processingSettings[itr->first] = itr->second;
        }
        
	saveProcessingSettings(processingSettingsFile,processingSettings);
	
	char setting[10];
	for (QMap<int,int>::iterator itr = deviceConfigMap.begin();itr != deviceConfigMap.end(); ++itr) 
        {
                int cfgId = itr.value();
                int deviceId = itr.key();
                if(tabDeviceMap[currentTabId] == deviceId)
                {
                        INFOMSG(("cfg: %d device: %d enabled",cfgId,deviceId));
                        saveSetting("enabled",cfgId,1);
                        
                        
                        for(unsigned int i = 0;i<camControls.size();i++)
                        {
                                INFOMSG(("saving settings: %d %d %d",camControls[i].id,cfgId,camControls[i].value));
                                sprintf(setting,"%d",camControls[i].id);
                                saveSetting(setting,cfgId,camControls[i].value);
                        }
                }else
                {
                        INFOMSG(("cfg: %d device: %d disabled",cfgId,deviceId));
                        saveSetting("enabled",cfgId,0);
                }
                
        }
	// cameraViewMap
}

void MainWindow::readSettings()
{
	struct stat sts;
	if ( (stat(processingSettingsFile.c_str(), &sts)) == -1)
	{
		INFOMSG(("Settings for processing file not found... creating a new one"));
		processingSettings["blur"] = 4;
		processingSettings["erosion"] = 0;
		processingSettings["bg"] = 2;
		
		// processingSettings["minH"]=69;
		// processingSettings["maxH"]= 179;
		
		// processingSettings["minS"]= 0;
		// processingSettings["maxS"]= 193;
		
		// processingSettings["minL"]= 0;
		// processingSettings["maxL"]= 255;
		
		// processingSettings["minAreaBlob"]= 20;
		// processingSettings["maxAreaBlob"]= 20;
		processingSettings["maxSizeTrail"]= 100;
		// processingSettings["maxDistanceNeigbor"]= 15;
		// processingSettings["maxLimboFrameLost"] = 5; 
		
		processingSettings["showProcessed"] = 0;
		processingSettings["showTraces"] = 1;
		
		// processingSettings["bgCodeBookNFrames"] = 200;
		
		processingSettings["maxDistanceNeigborX"]= 100;
		processingSettings["maxDistanceNeigborY"]= 15;
		
		processingSettings["minThreshold"] = 10;
		processingSettings["maxThreshold"] = 20;
		processingSettings["frameRate"] = 50;
		
		processingSettings["skipTailFPS"] = 5;
		processingSettings["timenotfoundPaintRedFPS"] = 100;
		
		saveProcessingSettings(processingSettingsFile,processingSettings);
	}
	
	processingSettings = loadProcessingSettings(processingSettingsFile);
}

void MainWindow::autoStartConfiguration()
{
        camsMap cards;
	getCamDevices(cards);
	
	int deviceNumber = -1;
	int cfgId = -1;
	for (camsMap::iterator iter = cards.begin();iter != cards.end(); ++iter) {
		deviceNumber = iter->first;
		cfgId = loadCameraId(iter->second);
		if(cfgId > -1)
		{
		        DEBUGMSG(("Camera already configured %s -> %d",iter->second.c_str(),cfgId));
		}else
		{
		        cfgId = getNumberCfgCams(); //get last cfg cam
		        cfgId++; //increment one value!
		        
		        INFOMSG(("Camera not found, creating new configuration %s %d",iter->second.c_str(),cfgId));
		        
		        //save new config...
		        char tmp[50];
		        getCamBusName(deviceNumber,&tmp[0]);
		        saveSetting("id",cfgId,cfgId);
		        saveSetting("busname",cfgId,tmp);
		        saveSetting("enabled",cfgId,0);
		        
		}
		
		deviceConfigMap[deviceNumber] = cfgId;
		int enabled;
		loadSetting("enabled",cfgId,enabled);
		if(enabled > 0)
		{
		        DEBUGMSG(("Starting new enabled cam %d",cfgId));
		        startCam(deviceNumber);
		}
	}
}
void MainWindow::setCurrentTab(int tabId)
{
        currentTabId = tabId;
}
void MainWindow::updateConfigMenu()
{
        
        if(currentTabId < 0)
        {
                return;
        }
        int deviceNumber = tabDeviceMap[currentTabId];
        
        
        INFOMSG(("updating config menu... %d",deviceNumber));
	ui->menuSetResolution->clear();
	resolutions.clear();
	
	// int sizeRes = 0;
	resolution tmpRes;
	if(cameraViewMap[currentTabId]->isVideo)
        {
                int minWidth = 100;
                int i = 1;
                while(tmpRes.w > minWidth)
                {
                        
                        tmpRes.w = cameraViewMap[currentTabId]->widthVideo / i;
                        tmpRes.h = cameraViewMap[currentTabId]->heightVideo / i;
                        tmpRes.caption = boost::lexical_cast<std::string>(tmpRes.w)+"x"+boost::lexical_cast<std::string>(tmpRes.h);
                        
                        resolutions.push_back(tmpRes);
                        INFOMSG(("%d %s",i,tmpRes.caption.c_str()));
                        i++;
                }
        }else
        {
                getResolutions(deviceNumber,resolutions);
        }
	
	
	
	for(unsigned int i = 0;i<resolutions.size();i++)
	{
		QAction *action = new QAction(this);
		// action->setText( QString::fromStdString(resolutions[i]));
		action->setText( QString::fromStdString(resolutions[i].caption));
		action->setData(i);
		ui->menuSetResolution->addAction(action);
	}
}

void MainWindow::handleResolutionChange(QAction *action)
{
        int idxResolution = action->data().toInt();
        
        if(cameraViewMap[currentTabId]->isVideo)
        {
                cameraViewMap[currentTabId]->setVideoResolution(resolutions[idxResolution].w,resolutions[idxResolution].h);
        }else{
                
                
                
                int deviceNumber = tabDeviceMap[currentTabId];
                // getResolution(deviceNumber,idxResolution,width,height);
                
                INFOMSG(("Loading new resolution %d for /dev/video%d %dx%d",idxResolution,deviceNumber,resolutions[idxResolution].w,resolutions[idxResolution].h));
                
                disconnectCamera(currentTabId);
                
                saveSetting("resolution",deviceConfigMap[deviceNumber],idxResolution);
                
                startCam(deviceNumber);
	}
}

void MainWindow::toggleProcessingAdjustToolbar()
{
        if(ui->toggleCameraAdjust->isChecked())
        {
                ui->toggleCameraAdjust->setChecked(false);
                ui->toolbarCameraAdjust->hide();
                // resize(ui->tabWidget->width(),height());
        }
        
        if(ui->toggleProcessingAdjust->isChecked())
	{
	        ui->toolbarProcessingAdjust->show();
	        resize(ui->tabWidget->width()+ui->toolbarProcessingAdjust->width(),height());
	}else
	{
	        ui->toolbarProcessingAdjust->hide();
	        resize(ui->tabWidget->width(),height());
	}
        
        
        /*
	if(ui->toggleProcessingAdjust->isChecked())
	{
	        ui->toolbarProcessingAdjust->show();
	        resize(ui->tabWidget->width()+ui->toolbarProcessingAdjust->width(),height());
	        
	}else
	{
	        ui->toolbarProcessingAdjust->hide();
		resize(ui->tabWidget->width(),height());
	}*/
}

void MainWindow::toggleCameraAdjustToolbar()
{
        if(ui->toggleProcessingAdjust->isChecked())
        {
                ui->toggleProcessingAdjust->setChecked(false);
                ui->toolbarProcessingAdjust->hide();
        }
        
        if(ui->toggleCameraAdjust->isChecked())
	{
	        ui->toolbarCameraAdjust->show();
	        resize(ui->tabWidget->width()+ui->toolbarCameraAdjust->width(),height());
	}else
	{
	        ui->toolbarCameraAdjust->hide();
	        resize(ui->tabWidget->width(),height());
	}
        
        
        /*
	if(ui->toggleCameraAdjust->isChecked())
	{
	        ui->toolbarCameraAdjust->show();
	        resize(ui->tabWidget->width()+ui->toolbarCameraAdjust->width(),height());
	}else
	{
	        ui->toolbarCameraAdjust->hide();
		resize(ui->tabWidget->width(),height());
	}*/
}

void MainWindow::keyPressEvent( QKeyEvent *event )
{
	if(event->key() == Qt::Key_Escape)
	{
		quit();
	}
	
	 QString	keyText = event->text().toUpper();
	 if(keyText=="T")
	 {
	 	 ui->toggleProcessingAdjust->setChecked( !ui->toggleProcessingAdjust->isChecked() );
	 	 toggleProcessingAdjustToolbar();
	 }
	
	
}

void MainWindow::updatemaxLimboFrameLost(int value)
{
	processingSettings["maxLimboFrameLost"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("maxLimboFrameLost set to: %d",value));
}

void MainWindow::updatemaxDistanceNeigbor(int value)
{
	processingSettings["maxDistanceNeigbor"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("maxDistanceNeigbor set to: %d",value));
}

void MainWindow::updateBlur(int value)
{
	processingSettings["blur"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("blur set to: %d",value));
}
void MainWindow::updateBGcalc(int value)
{
	processingSettings["bg"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("bg set to: %d",value));
	
	
	switch(value )
        {
        case BG_BGCodeBookModel:
        	
        	// bgCodeBook->setChecked(true);
        	break;
        case BG_COLORSUBTRACTION:
        	// bgColorSubtraction->setChecked(true);
                break;
        case BG_SEG:
        	
                break;
        }
}

void MainWindow::updateErosion(int value)
{
	processingSettings["erosion"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("erosion set to: %d",value));
}

void MainWindow::updateMinThreshold(int value)
{
	processingSettings["minThreshold"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("minThreshold set to: %d",value));
}

void MainWindow::updateMaxThreshold(int value)
{
	processingSettings["maxThreshold"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("maxThreshold set to: %d",value));
}

void MainWindow::updatemaxDistanceNeigborX(int value)
{
	processingSettings["maxDistanceNeigborX"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("maxDistanceNeigborX set to: %d",value));
}

void MainWindow::updatemaxDistanceNeigborY(int value)
{
	processingSettings["maxDistanceNeigborY"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("maxDistanceNeigborY set to: %d",value));
}

void MainWindow::updateFrameRate(int value)
{
        processingSettings["frameRate"] = value;
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
	INFOMSG(("frameRate set to: %d",value));
}
        

void MainWindow::resetBG()
{
	cameraViewMap[currentTabId]->resetBG();
}

void MainWindow::openFile()
{
	QString path;
	
	path = QFileDialog::getOpenFileName(this,"Choose a file to open",QString::null,QString::null);
	
	DEBUGMSG(( "ficheiro: %s",qPrintable(path) ));
	
	startVideo(path.toStdString());
}
void MainWindow::changeToDebugFrame(bool val)
{
	if(val)
	{
		processingSettings["showProcessed"] = 1;
	}else
	{
		processingSettings["showProcessed"] = 0;
	}
	
	
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}

void MainWindow::changeTraces(bool val)
{
	if(val)
	{
		processingSettings["showTraces"] = 1;
	}else
	{
		processingSettings["showTraces"] = 0;
	}
	
	cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}

void MainWindow::setCalibrationX(int val)
{
        processingSettings["xMm"] = val;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}
void MainWindow::setCalibrationY(int val)
{
        processingSettings["yMm"] = val;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}

void MainWindow::loadLinesSettings()
{
        cameraViewMap[currentTabId]->loadLinesSettings();
}
void MainWindow::quit()
{
        switch(QMessageBox::question( 
                this, 
                tr("Quit"), 
                tr("Are you sure you want to quit?"), 
                
                QMessageBox::Yes | 
                QMessageBox::No | 
                QMessageBox::Cancel, 
                
                QMessageBox::Cancel ) )
        {
        case QMessageBox::Yes:
                close();
                break;
        case QMessageBox::No:
        case QMessageBox::Cancel:
        default:
                return;
        }
        
}

void MainWindow::resetROI()
{
        
        INFOMSG(("ROI resetted"));
        processingSettings["roiX"] = 0;
        processingSettings["roiY"] = 0;
        processingSettings["roiH"] = 0;
        processingSettings["roiW"] = 0;
        
        validateROI();
        
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}

void MainWindow::setRoiX(int x)
{
        validateROI();
        processingSettings["roiX"] = x;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}

void MainWindow::setRoiY(int val)
{
        validateROI();
        processingSettings["roiY"] = val;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}
void MainWindow::setRoiW(int val)
{
        validateROI();
        processingSettings["roiW"] = val;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}
void MainWindow::setRoiH(int val)
{
        validateROI();
        
        processingSettings["roiH"] = val;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}

void MainWindow::newROI(QRect roi)
{
        
        processingSettings["roiX"] = roi.x();
        processingSettings["roiY"] = roi.y();
        processingSettings["roiH"] = roi.height();
        processingSettings["roiW"] = roi.width();
        
        validateROI();
}

void MainWindow::validateROI()
{
        if(processingSettings["roiX"] < 1 || processingSettings["roiX"]  > cameraViewMap[currentTabId]->capW) 
        {
                processingSettings["roiX"] = 0;
        }
        if( processingSettings["roiY"] < 1 || processingSettings["roiY"]  > cameraViewMap[currentTabId]->capH)
        {
                processingSettings["roiY"] = 0;
        }
        if(processingSettings["roiX"]+processingSettings["roiW"] > cameraViewMap[currentTabId]->capW || processingSettings["roiW"] < 1)
        {
                processingSettings["roiW"] = cameraViewMap[currentTabId]->capW-processingSettings["roiX"];
        }
        if(processingSettings["roiY"] +processingSettings["roiH"] > cameraViewMap[currentTabId]->capH || processingSettings["roiH"] < 1)
        {
                processingSettings["roiH"] = cameraViewMap[currentTabId]->capH-processingSettings["roiY"];
        }
        
        spinboxRoiX->setValue(processingSettings["roiX"]);
        spinboxRoiY->setValue(processingSettings["roiY"]);
        spinboxRoiW->setValue(processingSettings["roiW"]);
        spinboxRoiH->setValue(processingSettings["roiH"]);
}
void MainWindow::updateCameraAdjustToolbar(bool visible)
{
        if(visible)
        { 
                std::vector<videoCapControl> v;
                camControls.swap(v);
                videoCapGetControls(camControls, tabDeviceMap[currentTabId]);
                
                ui->toolbarCameraAdjust->clear();
                QSpinBox *spinbox;
                // QSignalMapper *mapper = new QSignalMapper(this);
                
                for(unsigned int i = 0;i<camControls.size();i++)
                {
                       
                                
                        spinbox = new QSpinBox();
                        if(camControls[i].typeControl == ctrlBoolean)
                        {
                                spinbox->setMinimum(0);
                                spinbox->setMaximum(1);
                        }else
                        {
                                spinbox->setMinimum(camControls[i].min);
                                spinbox->setMaximum(camControls[i].max);
                        }
                        spinbox->setValue(camControls[i].value);
                        spinbox->setSuffix(QString::fromStdString(" "+camControls[i].name));
                        
                        ui->toolbarCameraAdjust->addWidget(spinbox);
                        connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateCameraSetting(int)));
                        suffixIdCamControls[spinbox->suffix()] = camControls[i].id;
                        
                }
                
                // connect( signalMapper, SIGNAL(mapped(int)), SLOT(videoCapSetControl(int)) );
        }
}
void MainWindow::updateCameraSetting(int val)
{
        QSpinBox *spinbox = qobject_cast<QSpinBox *>(sender());
        
        if(spinbox != 0){
                // rest of code
                
                int deviceId = tabDeviceMap[currentTabId];
                int controlId = suffixIdCamControls[spinbox->suffix()];
                
                if(!videoCapSetControl(deviceId ,controlId,val))
                {
                        for(unsigned int i = 0;i<camControls.size();i++)
                        {
                                if(controlId == camControls[i].id)
                                {
                                        INFOMSG(("set new camera setting: %s %d to: %d",camControls[i].name.c_str(),camControls[i].id,val ));
                                        spinbox->setValue(camControls[i].value);
                                }
                        }
                        WARNMSG(("Error on setting the new value"));
                }else
                {
                        for(unsigned int i = 0;i<camControls.size();i++)
                        {
                                if(controlId == camControls[i].id)
                                {
                                        camControls[i].value = val;
                                }
                        }
                }
                
        }
        
}

void MainWindow::loadCameraAdjustSettings(int deviceId,int cfgId)
{
        int val;
        
        videoCapGetControls(camControls, deviceId);
        bool success = false;
        char setting[10];
        // for(unsigned int i = 0;i<camControls.size();i++)
        // for(int j = 0; j < 2;j++)
        // {
                for(unsigned int i = camControls.size()-1;i>0;i--)
                {
                        success = false;
                        sprintf(setting,"%d",camControls[i].id);
                        if(loadSetting(setting,cfgId,val))
                        {
                                if(videoCapSetControl(deviceId ,camControls[i].id,val))
                                {
                                        success = true;
                                        INFOMSG(("Setting loaded: %d %s %d",deviceId,camControls[i].name.c_str(),val));
                                }
                        }
                        if(!success)
                        {
                                WARNMSG(("Failed to load setting: /dev/video%d %s %d",deviceId,camControls[i].name.c_str(),val));
                        }
                        usleep(500);
                }
                usleep(1500);
        // }
        /*
        for(unsigned int i = 0;i<camControls.size();i++)
        {
                if(camControls[i].name.find("gain") != std::string::npos)
                {
                        INFOMSG(("Loading %s again...",camControls[i].name.c_str()));
                        sprintf(setting,"%d",camControls[i].id);
                        loadSetting(setting,cfgId,val);
                        videoCapSetControl(deviceId ,camControls[i].id,val);
                }
                if(camControls[i].name.find("brightness") != std::string::npos)
                {
                        INFOMSG(("Loading %s again...",camControls[i].name.c_str()));
                        sprintf(setting,"%d",camControls[i].id);
                        loadSetting(setting,cfgId,val);
                        videoCapSetControl(deviceId ,camControls[i].id,val);
                }
                usleep(200);
        }*/
}
void MainWindow::updateMinH(int val)
{
        processingSettings["minH"] = val;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}
void MainWindow::updateMinS(int val)
{
        processingSettings["minS"] = val;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}
void MainWindow::updateMinL(int val)
{
        processingSettings["minL"] = val;
        cameraViewMap[currentTabId]->setProcessingSettings(processingSettings);
}
