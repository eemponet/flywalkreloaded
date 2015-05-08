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
#include "CameraView.h"
#include "ui_CameraView.h"
#include "yarpports.h"
#include "Settings.h"
// Qt
#include <QMessageBox>
#include <QSignalMapper>


CameraView::CameraView(QWidget *mainwindow, QWidget *parent,int tabIdx, SharedImageBuffer *sharedImageBuffer) :
QWidget(parent),
ui(new Ui::CameraView),
sharedImageBuffer(sharedImageBuffer)
{
	// Setup UI
	ui->setupUi(this);
	
	tabId = tabIdx;
	// Initialize internal flag
	isCameraConnected=false;
	// Set initial GUI state
	ui->frameLabel->setText("No camera connected.");
	ui->imageBufferBar->setValue(0);
	ui->imageBufferLabel->setText("[000/000]");
	ui->captureRateLabel->setText("");
	ui->processingRateLabel->setText("");
	ui->deviceNumberLabel->setText("");
	ui->cameraResolutionLabel->setText("");
	ui->roiLabel->setText("");
	ui->mouseCursorPosLabel->setText("");
	ui->clearImageBufferButton->setDisabled(true);
	// Initialize ImageProcessingFlags structure
	imageProcessingFlags.grayscaleOn=false;
	imageProcessingFlags.smoothOn=false;
	imageProcessingFlags.dilateOn=false;
	imageProcessingFlags.erodeOn=false;
	imageProcessingFlags.flipOn=false;
	imageProcessingFlags.cannyOn=false;
	// Connect signals/slots
	connect(ui->frameLabel, SIGNAL(onMouseMoveEvent()), this, SLOT(updateMouseCursorPosLabel()));
	connect(ui->clearImageBufferButton, SIGNAL(released()), this, SLOT(clearImageBuffer()));
	connect(ui->frameLabel->menu, SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));
	connect(ui->startStopDatalogger, SIGNAL(released()), this, SLOT(startStopDatalogger()));
	
	connect(this, SIGNAL(setROI(QRect)), mainwindow, SLOT(newROI(QRect)));
	
	// Register type
	qRegisterMetaType<struct ThreadStatisticsData>("ThreadStatisticsData");
	
	id++;
	
	pYarpDataloggerCtrl.initPortSend(YARPPORT_SEND_DATALOGGERCTRL,YARPPORT_RCV_DATALOGGERCTRL,"tcp");
	recordingDlg = false;
	
	isVideo = false;
}

CameraView::~CameraView()
{
        
	DEBUGMSG(("starting to stop cameraView..."));
	if(isCameraConnected)
	{
		// Stop processing thread
		if(processingThread->isRunning())
			stopProcessingThread();
		// Stop capture thread
		if(captureThread->isRunning())
			stopCaptureThread();
		
		// Remove from shared buffer
		sharedImageBuffer->remove(id);
		
		
		// Disconnect camera
		if(captureThread->disconnectCamera()){
			// INFOMSG(("[ %d ]Camera successfully disconnected.",id));
		}else{
			// INFOMSG(( "[ %d ] WARNING: Camera already disconnected.",id));
		}
	}
	while(captureThread->isRunning())
	{
	        usleep(2000);
	        INFOMSG(("."));
	}
	// Delete UI
	delete ui;
}


bool CameraView::connectToCamera(int deviceNumber,int idxResolution)
{
	// Set frame label text
	ui->frameLabel->setText("Connecting to camera...");
	
	captureThread = new CaptureThread(sharedImageBuffer, tabId,true,id);
	sharedImageBuffer->start(id,false);
	
	ui->deviceNumberLabel->setText(QString("/dev/video%1").arg(deviceNumber));
	
	if(captureThread->connectToCamera(deviceNumber,idxResolution))
	{
	        isVideo = false;
		connectStream();
		
		capW = captureThread->getInputSourceWidth();
                capH = captureThread->getInputSourceHeight();
                
		return true;
	}
	return false;
}
void CameraView::connectStream()
{
	
	// Create processing thread
	processingThread = new ProcessingThread(sharedImageBuffer, tabId,id);
	
	// Setup signal/slot connections
	connect(processingThread, SIGNAL(newFrame(QImage)), this, SLOT(updateFrame(QImage)));
	connect(processingThread, SIGNAL(updateStatisticsInGUI(struct ThreadStatisticsData)), this, SLOT(updateProcessingThreadStats(struct ThreadStatisticsData)));
	connect(captureThread, SIGNAL(updateStatisticsInGUI(struct ThreadStatisticsData)), this, SLOT(updateCaptureThreadStats(struct ThreadStatisticsData)));
	connect(this, SIGNAL(setROI(QRect)), processingThread, SLOT(setROI(QRect)));
	// Only enable ROI setting/resetting if frame processing is enabled
	connect(ui->frameLabel, SIGNAL(newMouseData(struct MouseData)), this, SLOT(newMouseData(struct MouseData)));
	
	// Set initial data in processing thread
	// emit setROI(QRect(0, 0, captureThread->getInputSourceWidth(), captureThread->getInputSourceHeight()));
	
	// Start capturing frames from camera
	captureThread->start((QThread::Priority)QThread::NormalPriority);
	// Start processing captured frames (if enabled)
	
	processingThread->start((QThread::Priority)QThread::TimeCriticalPriority);
	
	// Setup imageBufferBar with minimum and maximum values
	ui->imageBufferBar->setMinimum(0);
	ui->imageBufferBar->setMaximum(sharedImageBuffer->maxSize());
	// Enable/disable appropriate GUI items
	//ui->imageProcessingMenu->setEnabled(enableFrameProcessing);
	//ui->imageProcessingSettingsAction->setEnabled(enableFrameProcessing);
	// Enable "Clear Image Buffer" push button
	ui->clearImageBufferButton->setEnabled(true);
	// Set text in labels
	// ui->deviceNumberLabel->setNum(tabId);
	ui->cameraResolutionLabel->setText(
	        QString::number(captureThread->getInputSourceWidth())+
	        QString("x")+
	        QString::number(captureThread->getInputSourceHeight())+
	        QString(" (")+
	        QString::number(ui->frameLabel->width())+
	        QString("x")+
	        QString::number(ui->frameLabel->height())+
	        QString(")")
	        );
	// Set internal flag and return
	isCameraConnected=true;
	// Set frame label text
	
	
	loadLinesSettings();
	
	if(captureThread->getInputSourceWidth() > ui->frameLabel->width())
	{
	        processingThread->resizeToFitWindow = true;
	        INFOMSG(("resizing to fit window"));
	}
}

void CameraView::loadLinesSettings()
{
        std::vector<int> linesSets;
	loadLinesConfiguration(linesSets);
	processingThread->setLinesSettings(linesSets);
}
void CameraView::stopCaptureThread()
{
	// INFOMSG(("[ %d ]About to stop capture thread...",id));
	captureThread->stop();
	sharedImageBuffer->wakeAll(); // This allows the thread to be stopped if it is in a wait-state
	// Take one frame off a FULL queue to allow the capture thread to finish
	
	captureThread->wait();
	// INFOMSG(("[ %d ]Capture thread successfully stopped.",id));
}

void CameraView::stopProcessingThread()
{
	// INFOMSG(("[ %d ]About to stop processing thread...",id));
	processingThread->stop();
	sharedImageBuffer->wakeAll(); // This allows the thread to be stopped if it is in a wait-state
	processingThread->wait();
	// INFOMSG(("[ %d ]Processing thread successfully stopped.",id));
}

void CameraView::updateCaptureThreadStats(struct ThreadStatisticsData statData)
{
        
	// Show [number of images in buffer / image buffer size] in imageBufferLabel
	ui->imageBufferLabel->setText(QString("[")+QString::number(sharedImageBuffer->getSize(id))+
		QString("/")+QString::number(sharedImageBuffer->maxSize())+QString("]"));
	
	// Show percentage of image bufffer full in imageBufferBar
	ui->imageBufferBar->setValue((sharedImageBuffer->getSize(id)));
	// Show processing rate in captureRateLabel
	ui->captureRateLabel->setText(QString::number(statData.averageFPS)+" fps");
	// Show number of frames captured in nFramesCapturedLabel
	ui->nFramesCapturedLabel->setText(QString("[") + QString::number(statData.nFramesProcessed) + QString("]"));
}

void CameraView::updateProcessingThreadStats(struct ThreadStatisticsData statData)
{
	// Show processing rate in processingRateLabel
	ui->processingRateLabel->setText(QString::number(statData.averageFPS)+" fps");
	// Show ROI information in roiLabel
	ui->roiLabel->setText(QString("(")+QString::number(processingThread->getCurrentROI().x())+QString(",")+
		QString::number(processingThread->getCurrentROI().y())+QString(") ")+
		QString::number(processingThread->getCurrentROI().width())+
		QString("x")+QString::number(processingThread->getCurrentROI().height()));
	// Show number of frames processed in nFramesProcessedLabel
	ui->nFramesProcessedLabel->setText(QString("[") + QString::number(statData.nFramesProcessed) + QString("]"));
}

void CameraView::updateFrame(const QImage &frame)
{
	// Display frame
	// ui->frameLabel->setPixmap(QPixmap::fromImage(frame));
	ui->frameLabel->updatePixmap(QPixmap::fromImage(frame));
}

void CameraView::clearImageBuffer()
{
        sharedImageBuffer->remove(id);
	if(sharedImageBuffer->getSize(id) == 0){
		INFOMSG(( "[ %d ] Image buffer successfully cleared.",id));
	}else{
		INFOMSG(( "[ %d ] WARNING: Could not clear image buffer.",id));
	}
}

void CameraView::updateMouseCursorPosLabel()
{
        int capW = captureThread->getInputSourceWidth();
        int capH = captureThread->getInputSourceHeight();
        
        int frameW = ui->frameLabel->width();
        int frameH = ui->frameLabel->height();
        
        double ratioW = (double)capW/(double)frameW;
        double ratioH = (double)capH/(double)frameH;
        
	// Update mouse cursor position in mouseCursorPosLabel
	ui->mouseCursorPosLabel->setText(QString("(")+QString::number(ui->frameLabel->getMouseCursorPos().x())+
		QString(",")+QString::number(ui->frameLabel->getMouseCursorPos().y())+
		QString(")"));
	
	// Show ROI-adjusted cursor position if camera is connected
	if(isCameraConnected)
        ui->mouseCursorPosLabel->setText(ui->mouseCursorPosLabel->text()+
        	QString(" [")+QString::number((int) (ui->frameLabel->getMouseCursorPos().x()*ratioW))+
        	QString(",")+QString::number((int)(ui->frameLabel->getMouseCursorPos().y()*ratioH))+
        	QString("]"));
        
}

void CameraView::newMouseData(struct MouseData mouseData)
{
        
        QRect selectionBox;
        
        // Set ROI
        if(mouseData.leftButtonRelease)
        {       
                int roiX = mouseData.selectionBox.x();
                int roiY = mouseData.selectionBox.y();
                int roiW = mouseData.selectionBox.width();
                int roiH = mouseData.selectionBox.height();
                
                int frameW = ui->frameLabel->width();
                int frameH = ui->frameLabel->height();
                // Copy box dimensions from mouseData to taskData
                
                if(roiW < 0)
                {
                        roiX = roiX + roiW;
                        roiW = roiW*-1;
                }
                if(roiH < 0)
                {
                        roiY = roiY + roiH;
                        roiH = roiH*-1;
                }
                
                
                selectionBox.setX(roiX-(frameW-capW)/2);
                selectionBox.setY(roiY-(frameH-capH)/2);
                selectionBox.setWidth(roiW);
                selectionBox.setHeight(roiH);
                INFOMSG((">>> froi(%d,%d[%d,%d]) roi(%d,%d[%d,%d]) cap(%d,%d) frame(%d,%d)",selectionBox.x(),selectionBox.y(),selectionBox.width(),selectionBox.height(),roiX,roiY,roiW,roiH,capW,capH,frameW,frameH));
                if(processingThread->resizeToFitWindow)
                {
                        double ratioW = (double)capW/(double)frameW;
                        double ratioH = (double)capH/(double)frameH;
                        
                        selectionBox.setX((double)roiX*ratioW);
                        selectionBox.setY((double)roiY*ratioH);
                        selectionBox.setWidth((double)roiW*ratioW);
                        selectionBox.setHeight((double)roiH*ratioH);
                        INFOMSG((">>> froi(%d,%d[%d,%d]) roi(%d,%d[%d,%d]) cap(%d,%d) frame(%d,%d)",selectionBox.x(),selectionBox.y(),selectionBox.width(),selectionBox.height(),roiX,roiY,roiW,roiH,capW,capH,frameW,frameH));
                }
                
                
                if((selectionBox.x()<0)||(selectionBox.y()<0)||
                        ((selectionBox.x()+selectionBox.width())>capW)||
                ((selectionBox.y()+selectionBox.height())>capH) || roiW <=0 || roiH <= 0)
                {
                        // Display error message
                        // QMessageBox::warning(this,"ERROR:","Selection box outside range. Please try again.");
                        WARNMSG(("ROI out of range"));
                }else{
                        
                        INFOMSG(("NEW ROI x,y(%d,%d) w,h(%d,%d)",selectionBox.x(),selectionBox.y(),selectionBox.width(),selectionBox.height()));
                        emit setROI(selectionBox);
                        
                }
        }
        
}
void CameraView::handleContextMenuAction(QAction *action)
{
	if(action->text()=="Reset ROI")
	{
		emit setROI(QRect(0, 0, captureThread->getInputSourceWidth(), captureThread->getInputSourceHeight()));
	}
	
	
	printf("action: %s\n",qPrintable(action->text()));
}

void CameraView::resetBG()
{
	processingThread->pTracking2D.resetBG();
	processingThread->pTracking2D.resetTracking();
}

bool CameraView::connectToVideo(std::string filename)
{
	// Set frame label text
	ui->frameLabel->setText("Loading video...");
	ui->deviceNumberLabel->setText( QString::fromStdString(filename.substr(filename.length()-30,filename.length())));
	
	captureThread = new CaptureThread(sharedImageBuffer, tabId,true,id);
	sharedImageBuffer->start(id,true);
	if(captureThread->connectVideo(filename))
	{
	        isVideo = true;
	        widthVideo = captureThread->getInputSourceWidth();
	        heightVideo = captureThread->getInputSourceHeight();
	        
		connectStream();
		return true;
	}
	
	return false;
}

void CameraView::setProcessingSettings(std::map<std::string,int> newParams)
{
	processingThread->setSettings(newParams);
}

std::map<std::string,int> CameraView::getProcessingSettings()
{
	return processingThread->getSettings();
}

void CameraView::startStopDatalogger()
{
        // INFOMSG(("starting / stopping datalogger"));
        
        if(!recordingDlg)
        {
                recordingDlg = true;
                pYarpDataloggerCtrl.send("start");
                ui->startStopDatalogger->setText("Stop datalogger");
        }else{
                recordingDlg = false;
                pYarpDataloggerCtrl.send("stop");
                ui->startStopDatalogger->setText("Start datalogger");
        }
}

void CameraView::setVideoResolution(int width,int height)
{
        if(isVideo)
        {
                INFOMSG(("setting new resolution %dx%d",width,height));
                // captureThread->pVideoCapture.setImageSize(width,height);
                captureThread->setVideoResolution(width,height);
                clearImageBuffer();
                // delete processingThread;
                
                // sharedImageBuffer->clear(tabId);
                // if(processingThread->isRunning())
                // stopProcessingThread();
		
		// while(!processingThread->isRunning())
		// {
		// usleep(500);
		// }
                // processingThread = new ProcessingThread(sharedImageBuffer, tabId,id,pSharedParticles);
                // processingThread->resetTracking();
        }
}
std::map<std::string,double> CameraView::getCamSpecs()
{
        return captureThread->pVideoCapture.camSpecs();
}
