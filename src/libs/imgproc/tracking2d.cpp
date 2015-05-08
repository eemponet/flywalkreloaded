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
#include "tracking2d.h"

extern std::map<std::string,int> paramsInt;

clsTracking2D::clsTracking2D()
{
        frameCount = 0;
        
        initializedProcess = false;
        
        BgCbstarted = false;
        BGcolorstarted = false;
        updateBgModel = true;
        bgsStarted = false;
        readyForParticles = false;
        startTracking = true;
        
        allocatedInitFrames = false;
        allocatedInitFramesBGCodeBook = false;
        
        paramsInt["roiX"] = -1;
        paramsInt["roiY"] = -1;
        paramsInt["roiW"] = -1;
        paramsInt["roiH"] = -1;
        
        paramsInt["frameRate"] = 50;
        
        paramsInt["maxNumberParticles"] = 30;
        
        fpsTimer.reset();
        fpsTimer.start();
        // cvNamedWindow("fg");
        
        
}

clsTracking2D::~clsTracking2D()
{}

bool clsTracking2D::getPoints()
{
        return true;
}

void clsTracking2D::initializeProcessing()
{
        if(allocatedInitFrames)
        {
        //         cvReleaseImage(&rawImageI);
                cvReleaseImage(&paintedFrame);
                cvReleaseImage(&foregroundImage);
                cvReleaseImage(&debugFrame);
                
                INFOMSG(("released memory.. general"));
        }
        
        cameraResolutions["width"] = rawImage.cols;
        cameraResolutions["height"] = rawImage.rows;
        cameraResolutions["roiX"] = paramsInt["roiX"];
        cameraResolutions["roiY"] = paramsInt["roiY"];
        cameraResolutions["roiW"] = paramsInt["roiW"];
        cameraResolutions["roiH"] = paramsInt["roiH"];
        
        // INFOMSG(("loading ROI(%dx%d[%dx%d]) rawImage(%d,%d)",cameraResolutions["roiX"], cameraResolutions["roiY"], cameraResolutions["roiW"],  cameraResolutions["roiH"],rawImage.cols,rawImage.rows));
        
        
        
        // INFOMSG(("loaded ROI(%dx%d[%dx%d]) rawImage(%d,%d)",cameraResolutions["roiX"], cameraResolutions["roiY"], cameraResolutions["roiW"],  cameraResolutions["roiH"],rawImage.cols,rawImage.rows));
        
        paintedFrame = cvCreateImage( cvSize(cameraResolutions["width"],cameraResolutions["height"]), IPL_DEPTH_8U, 3 );
        foregroundImage = cvCreateImage( cvSize(cameraResolutions["roiW"],cameraResolutions["roiH"]), IPL_DEPTH_8U, 1 );
        
        debugFrame = cvCreateImage(  cvSize(cameraResolutions["roiW"],cameraResolutions["roiH"]), IPL_DEPTH_8U, 1 );
        
        initializedProcess = true;
        BgCbstarted = false;
        bgsStarted = false;
        pParticles.processParticlesStarted = false;
        
        readyForParticles = false;
        allocatedInitFrames = true;
}

void clsTracking2D::startCodeBook(IplImage *image)
{
        if(allocatedInitFramesBGCodeBook)
        {
                INFOMSG(("released memory.. BGCodeBook"));
                cvReleaseImage(&bgfgImage);
                cvReleaseImage(&BgCbImaskCodeBook);
                cvReleaseImage(&BgCbImaskCodeBookCC);
        }
        
        BGCodeBookModel = cvCreateBGCodeBookModel();
        
        //Set color thresholds to default values
        BGCodeBookModel->modMin[0] = 3;
        BGCodeBookModel->modMin[1] = BGCodeBookModel->modMin[2] = 3;
        BGCodeBookModel->modMax[0] = 10;
        BGCodeBookModel->modMax[1] = BGCodeBookModel->modMax[2] = 10;
        BGCodeBookModel->cbBounds[0] = BGCodeBookModel->cbBounds[1] = BGCodeBookModel->cbBounds[2] = 10;
        
        BgCbImaskCodeBook = cvCreateImage( cvSize(cameraResolutions["roiW"],cameraResolutions["roiH"]), IPL_DEPTH_8U, 1 );
        BgCbImaskCodeBookCC = cvCreateImage(cvSize(cameraResolutions["roiW"],cameraResolutions["roiH"]), IPL_DEPTH_8U, 1 );
        bgfgImage = cvCreateImage( cvSize(cameraResolutions["roiW"],cameraResolutions["roiH"]),image->depth, image->nChannels );
        
        cvSet(BgCbImaskCodeBook,cvScalar(255));
        
        BgCbNrframes = 0;
        
        allocatedInitFramesBGCodeBook = true;
        
        INFOMSG(("codebook started."));
}


cv::Mat clsTracking2D::getProcessedImage()
{
        
        if(isVideo)
                usleep(VIDEOFRAME_SLEEP);
        
        if(!initializedProcess || resolutionChanged()){
                initializeProcessing();
        }
        
        cv::Rect roi( cameraResolutions["roiX"], cameraResolutions["roiY"], cameraResolutions["roiW"],  cameraResolutions["roiH"]);
        rawMatROI = rawImage(roi);
        
        rawImageROIval = rawMatROI;
        rawImageROI = &rawImageROIval;
        
        rawImageIval = rawImage;
        rawImageI = &rawImageIval;
        
        // cvRectangle(rawImageI, cvPoint( paramsInt["roiX"], paramsInt["roiY"]),cvPoint( paramsInt["roiX"]+ paramsInt["roiW"], paramsInt["roiY"]+ paramsInt["roiH"]), CV_RGB(0,0,255), 1, 40, 0);
        
        frameCount++;
        
        //process background subtraction:
        switch(paramsInt["bg"] )
        {
        case BG_BGCodeBookModel:
        	INFOMSG(("calcCodeBook"));
                calcCodeBook(rawImageROI,foregroundImage,debugFrame);
                break;
        case BG_COLORSUBTRACTION:
        	INFOMSG(("calcBGsubtraction"));
                calcBGsubtraction(rawImageROI,foregroundImage);
                break;
        case BG_SEG:
        	INFOMSG(("calcBgSegmentation"));
                calcBgSegmentation(rawImageROI,foregroundImage);
                break;
        default:
                DEBUGMSG(("no bg model found"));
        }
        
        cvCopy(rawImageI,paintedFrame);
        
        paintROI(paintedFrame);
        
        if(readyForParticles && startTracking)
        {
                pParticles.processManyParticles(foregroundImage);
                                
                if(paramsInt["showTraces"] == 1)
                        pParticles.drawManyParticles(paintedFrame);
        }
        
        if(paramsInt["showProcessed"] == 1)
                rawImage = foregroundImage;
        else
                rawImage = paintedFrame;
        
        
        if(paramsInt["frameRate"] > 0 )
        {
                double timeElapsed = fpsTimer.getElapsedTime();
		double timeToSleep = ((1000.0/(double)paramsInt["frameRate"])-timeElapsed)*1000;
		if(timeToSleep > 0)
		{
			usleep(timeToSleep);
		}
		
		fpsTimer.reset();
		fpsTimer.start();
		
        }
        while(paramsInt["frameRate"] == 0)
        {
                usleep(3000);
        }
        
        return rawImage;
}



void clsTracking2D::calcCodeBook(IplImage *image, IplImage *foreground, IplImage *debugImage)
{
        
        if(!BgCbstarted )
        {
                // usleep(1000);
                startCodeBook(image);
                BgCbstarted = true;
                BgCbNrframes = 0;
        }
        
        cvCopy(image,bgfgImage);
        
        if(paramsInt["blur"] > 0)
                cvSmooth(bgfgImage, bgfgImage, CV_BLUR, paramsInt["blur"], paramsInt["blur"], 0, 0);
        
        cvCvtColor( bgfgImage, bgfgImage, CV_BGR2YCrCb );
        
        //POR CORES: cvInRangeS(frame_buffer, cvScalar(minH, minS, minL), cvScalar(maxH, maxS, maxL), frame_threshold);
        
        try
        {
                
                if( BgCbNrframes-1 < paramsInt["bgCodeBookNFrames"]  )
                {
                        cvBGCodeBookUpdate( BGCodeBookModel, bgfgImage );
                        BgCbNrframes++;
                        DEBUGMSG(("adding frames..%d",BgCbNrframes ));
                }
                
                
                if( BgCbNrframes-1 == paramsInt["bgCodeBookNFrames"] )
                {
                        cvBGCodeBookClearStale( BGCodeBookModel, BGCodeBookModel->t/2 );
                        
                        BgCbNrframes++;
                        readyForParticles = true;
                }
                
                //Find the foreground if any
                if( BgCbNrframes-1 >= paramsInt["bgCodeBookNFrames"]  )
                {
                        
                        // Find foreground by codebook method
                        cvBGCodeBookDiff( BGCodeBookModel, bgfgImage, BgCbImaskCodeBook );
                        
                        // This part just to visualize bounding boxes and centers if desired
                        
                        cvCopy(BgCbImaskCodeBook,BgCbImaskCodeBookCC);
                        
                        cvSegmentFGMask( BgCbImaskCodeBookCC );
                        
                        // readyTo2Dtrack = true;
                        
                }
                
        }
        catch(Exception e)
        {
                ERRMSG(("EXCEPTION:: trying to restart the BGFGCodeBook"));
                BgCbstarted = false;
                
                
        }
        
        //process erode dilate
        //removing loose points
        if(paramsInt["erosion"] > 0)
        {
                cvErode(BgCbImaskCodeBook, BgCbImaskCodeBook, NULL,paramsInt["erosion"] );
        }
        //augmenting neighbour points
        // cvDilate(BgCbImaskCodeBook, BgCbImaskCodeBook, NULL,1);
        
        // cvCanny(BgCbImaskCodeBook,BgCbImaskCodeBook,10,100,3);
        
        cvCopy(BgCbImaskCodeBook,foreground);
        
        if(debugImage != NULL)
                cvCopy(BgCbImaskCodeBook,debugImage);
}

void clsTracking2D::calcBgSegmentation(IplImage *frame, IplImage *foreground)
{
        if(!bgsStarted)
        {
                bgsGray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
                cvCvtColor(frame, bgsGray, CV_BGR2GRAY);
                bgfgImage = cvCreateImage( cvGetSize(frame),frame->depth, frame->nChannels );
                bgsBufferedFrame = cvCloneImage(bgsGray);
                bgsBufferedFrame2 = cvCloneImage(bgsBufferedFrame);
                bgsBufferedFrame3 = cvCloneImage(bgsBufferedFrame);
                
                bgsStarted = true;
                INFOMSG(("initializing calcBgSegmentaion IplImage,IplImage"));
                
                framesBgSegmentation = 0;
                return;
        }
        
        cvCopy(frame,bgfgImage);
        
        if(paramsInt["blur"] > 0)
                cvSmooth(bgfgImage, bgfgImage, CV_BLUR, paramsInt["blur"], paramsInt["blur"], 0, 0);
        
        
        cvCvtColor(bgfgImage, bgsGray, CV_BGR2GRAY);
        cvAbsDiff(bgsBufferedFrame2,bgsGray, foreground);
        
        cvThreshold(foreground, foreground, paramsInt["minThreshold"], paramsInt["maxThreshold"], CV_THRESH_BINARY);
        
        if(paramsInt["erosion"] > 0)
        {
                cvErode(foreground, foreground, NULL,paramsInt["erosion"] );
        }
        
        cvDilate(foreground, foreground, NULL,1);
        
        cvReleaseImage(&bgsBufferedFrame3);
        bgsBufferedFrame3 = cvCloneImage(bgsBufferedFrame2);
        
        cvReleaseImage(&bgsBufferedFrame2);
        bgsBufferedFrame2 = cvCloneImage(bgsBufferedFrame);
        cvReleaseImage(&bgsBufferedFrame);
        bgsBufferedFrame = cvCloneImage(bgsGray);
        
        if(!readyForParticles)
        {
                framesBgSegmentation++;
                if(framesBgSegmentation > 4)
                {
                        readyForParticles = true;
                }
        }
        /*cvReleaseImage(&bgsBufferedFrame3);
        bgsBufferedFrame3 = cvCloneImage(bgsBufferedFrame2);
        
        cvReleaseImage(&bgsBufferedFrame2);
        bgsBufferedFrame2 = cvCloneImage(bgsBufferedFrame);
        
        cvReleaseImage(&bgsBufferedFrame);
        bgsBufferedFrame = cvCloneImage(bgsGray);*/
        
        // cvShowImage("bg",foreground);
        
        
}
void clsTracking2D::calcBgSegmentation(Mat img, IplImage *foreground)
{
        // cap >> img;
        
        //
        
        // return;
        if( img.empty() )
        {
                INFOMSG(("Img is empty()"));
                return;
        }
        
        if( fgimg.empty() )
                fgimg.create(img.size(), img.type());
        
        //update the model
        bg_model(img, fgmask, updateBgModel ? -1 : 0);
        
        fgimg = Scalar::all(0);
        img.copyTo(fgimg, fgmask);
        
        Mat bgimg;
        bg_model.getBackgroundImage(bgimg);
        
        // imshow("image", img);
        // imshow("foreground mask", fgmask);
        // imshow("foreground image", fgimg);
        // imshow("bg image", bgimg);
        
        // if(foregroung != null)
        // {
        //         cvReleaseImage(&foreground);
        // }
        // foreground=cvCloneImage(&(IplImage)fgmask);
        
        
}

void clsTracking2D::calcBGsubtraction(IplImage *image, IplImage *foreground, IplImage *debugImage)
{
        if(!BGcolorstarted)
        {
                bgcolorImage = cvCreateImage( cvSize( image->width, image->height ),image->depth, image->nChannels );
                bgsGray = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
                cvCvtColor(image, bgsGray, CV_BGR2GRAY);
                readyForParticles = true;
                BGcolorstarted = true;
        }
        
        cvCopy(image,bgcolorImage);
        
        if(paramsInt["blur"] > 0)
                cvSmooth(image, bgcolorImage, CV_BLUR, paramsInt["blur"], paramsInt["blur"], 0, 0);
        
        //converting to HSV
        // cvCvtColor(bgcolorImage, bgcolorImage, CV_BGR2HSV);
        
        //filtrating by color... good on this video!
        cvInRangeS(bgcolorImage, cvScalar(paramsInt["minH"], paramsInt["minS"], paramsInt["minL"]), cvScalar(paramsInt["maxH"], paramsInt["maxS"], paramsInt["maxL"]), foreground);
        // cv::invert(bgcolorImage,bgcolorImage);
        // cvCvtColor(bgcolorImage, bgsGray, CV_BGR2GRAY);
        // cvThreshold(bgsGray, foreground, paramsInt["minThreshold"], paramsInt["maxThreshold"], CV_THRESH_BINARY );
        
        // cvThreshold(bgsGray, foreground, paramsInt["minThreshold"], paramsInt["maxThreshold"], CV_THRESH_BINARY);
        // cvThreshold(bgsGray, foreground, paramsInt["minThreshold"], paramsInt["maxThreshold"], CV_THRESH_BINARY_INV);
        
        // cvThreshold(foreground, foreground, 5,255, 1 );
        //removing loose points
        cvErode(foreground, foreground, NULL,1);
        
        //augmenting neighbour points
        cvDilate(foreground, foreground, NULL,1);
        
        //calculating edges
        // cvCanny(foreground,foreground,10,100,3);
        
        
}

void clsTracking2D::resetBG()
{
        resetTracking();
        /*if(paramsInt["bg"] == BG_BGCodeBookModel)
        {
                INFOMSG(("background resetted"));
                BgCbNrframes = 0;
        }
        else
        {
                if(paramsInt["bg"] == BG_SEG)
                {
                        if(updateBgModel)
                        {
                                INFOMSG(("Background update is on\n"));
                        }
                        else
                        {
                                INFOMSG(("Background update is off\n"));
                        }
                        updateBgModel = !updateBgModel;
                }
                else
                {
                        INFOMSG(("Nothing to do."));
                }
        }*/
}

void clsTracking2D::setSettings(std::map<std::string,int> newParams)
{
        // INFOMSG(("New settings loaded:: "));
        for (std::map<std::string,int>::iterator itr = newParams.begin();itr != newParams.end(); ++itr) 
        {
                // INFOMSG(("%s = %d",itr->first.c_str(),itr->second));
                paramsInt[itr->first] = itr->second;
        }
}
void clsTracking2D::setLines(std::vector<int> linesCfg)
{
        pParticles.startLines(linesCfg);
}

std::map<std::string,int> clsTracking2D::getSettings()
{
        
        return paramsInt;
}

void clsTracking2D::setROI(int x, int y,int w, int h)
{
        
        paramsInt["roiX"] = x;
        paramsInt["roiY"] = y;
        paramsInt["roiW"] = w;
        paramsInt["roiH"] = h;
        
        startTracking = true;
        this->resetTracking();
        // BGcolorstarted = false;
        // BgCbstarted = false;
        
        // initializedProcess = false;            
        // bgsStarted = false;
        // readyForParticles = false;
        // pParticles.reinitialize();
}

std::vector<clsParticle> clsTracking2D::getParticles()
{
        return pParticles.toVector();
}

bool clsTracking2D::getParticlesToString(std::string *str)
{
        return pParticles.toString(str,paramsInt["maxNumberParticles"]);
}

bool clsTracking2D::resetTracking()
{
        BGcolorstarted = false;
        BgCbstarted = false;
        BGcolorstarted = false;
        
        initializedProcess = false;            
        bgsStarted = false;
        readyForParticles = false;
        startTracking = true;
        pParticles.reinitialize();
        
        return true;
}

void clsTracking2D::paintROI(IplImage *img)
{
        CvPoint rect1 = cvPoint(paramsInt["roiX"],paramsInt["roiY"]);
        CvPoint rect2 = cvPoint(paramsInt["roiX"]+paramsInt["roiW"],paramsInt["roiY"]+paramsInt["roiH"]);
        
        cvRectangle(img, rect1,rect2, CV_RGB(0,0,255), 2, 40, 0);
}
  
bool clsTracking2D::resolutionChanged()
{
        if(
                cameraResolutions["roiX"] != paramsInt["roiX"] ||
                cameraResolutions["roiY"] != paramsInt["roiY"] ||
                cameraResolutions["roiW"] != paramsInt["roiW"] ||
                cameraResolutions["roiH"] != paramsInt["roiH"] ||
                cameraResolutions["width"]  != rawImage.cols  ||
                cameraResolutions["height"] != rawImage.rows)
        {
                INFOMSG(("Resolution/ROI changed from: cam:%dx%d ROI:%dx%d[%dx%d] to cam:%dx%d ROI:%dx%d[%dx%d]",
                        cameraResolutions["width"] ,cameraResolutions["height"],
                        cameraResolutions["roiX"],cameraResolutions["roiY"],cameraResolutions["roiW"],cameraResolutions["roiH"], 
                        rawImage.cols,rawImage.rows,
                        paramsInt["roiX"],paramsInt["roiY"],paramsInt["roiW"],paramsInt["roiH"]
                        ));

                return true;
        }
        return false;
}
