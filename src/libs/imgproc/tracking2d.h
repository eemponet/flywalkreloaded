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
#ifndef TRACKING2D_H
#define TRACKING2D_H

#include "utils.h"
#include "def.h"
#include "fps.h"
#include "timers.h"

#include "videocapture.h"
#include "particles.h"

#include <highgui.h>
#include <cvaux.h>

using namespace cv;

#define BG_BGCodeBookModel 0
#define BG_COLORSUBTRACTION 1
#define BG_SEG 2

#define BGCB_NFRAMES 30



class clsTracking2D : public clsVideoCapture
{
public:
	clsTracking2D();
	~clsTracking2D();
	
	cv::Mat getProcessedImage();
	
	void resetBG();
	
	void setROI(int x, int y,int w, int h);
	
	void setSettings(std::map<std::string,int> newParams);
	void setLines(std::vector<int> linesCfg);
	
	std::map<std::string,int> getSettings();
	
	vector<clsParticle> getParticles();
	
	bool getParticlesToString(std::string *str);
	
	bool resetTracking();
protected:
	bool getPoints();
private:
	
	
	//INIT PROCESS
	bool initializedProcess;
	void initializeProcessing();
	IplImage *rawImageI;
	IplImage rawImageIval;
	IplImage *foregroundImage;
	IplImage *paintedFrame;
	IplImage *debugFrame;
	bool startTracking;
	bool allocatedInitFrames; 
	
	clsTimers fpsTimer;
	
	//ALL CODE BOOK RELATED:
	void startCodeBook(IplImage *rawImage);
	void calcCodeBook(IplImage *image,IplImage *foreground,IplImage *debugImage=NULL);
	
	CvBGCodeBookModel* BGCodeBookModel;
	IplImage *BgCbImaskCodeBook,*BgCbImaskCodeBookCC;
	IplImage *bgfgImage;
	int BgCbNrframes;
	bool BgCbstarted;
	
	
	//ALL BGCOLORSUBTRACTION RELATED:
	void calcBGsubtraction(IplImage *image,IplImage *foreground,IplImage *debugImage=NULL);
	IplImage *bgcolorImage;
	bool BGcolorstarted;
	bool allocatedInitFramesBGCodeBook; 
	
	//BG SEGMENTATION :
	void calcBgSegmentation(Mat img, IplImage *foreground);
	void calcBgSegmentation(IplImage *img, IplImage *foreground);
	bool updateBgModel;
	BackgroundSubtractorMOG2 bg_model;
	Mat fgmask, fgimg;
	// v2:
	bool bgsStarted;
	IplImage *bgsGray,*bgsBufferedFrame,*bgsBufferedFrame2,*bgsBufferedFrame3;
	int framesBgSegmentation;
	
	//processingManyParticles
	clsParticles pParticles;
	bool readyForParticles;
	
	//ROI
	IplImage rawImageROIval;
	IplImage *rawImageROI;
	cv::Mat rawMatROI;
	
	void paintROI(IplImage *img);
	
	std::map<std::string,int> cameraResolutions;
	
	bool resolutionChanged();
};

#endif
