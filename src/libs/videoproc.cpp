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
#include "videoproc.h"    
#include <vector>
#include "../CONFIGS.h"

/** TEST MODEEEE **/
pthread_t trecording;
pthread_mutex_t MUTEX_bgfg;
pthread_mutex_t MUTEX_subtr ;
pthread_mutex_t MUTEX_frame ;
bool recordingIMGs;
IplImage *frame_output_subtraction_thrs;
IplImage *frame_output_bgfg_thrs;
IplImage *frame_raw;
/** END OF TEST MODE**/

void PERSPECTIVE_MATRICES_mouseHandler(int event, int x, int y, int flags, void *param);
void perspectiveLinesMouseHandler(int event, int x, int y, int flags, void *param);

void pick_color_mouse( int event, int x, int y, int flags, void* param );

void pickXandY(int event, int x, int y,int flags, void* param);

int PERSPECTIVE_MATRICES_nrSelectedPoints;
vector<CvPoint> PERSPECTIVE_MATRICES_pointsSelected;
int dragBall = -1;
CvPoint pointsPerspectiveLines[4];
int clickedX, clickedY;

bool zooming = false;
int zoomedOut = false;
int zoomedIn = false;

clsVideo::clsVideo()
{
	pause = false;
	
	minH = 10;
	maxH = 40;
	
	minS = 0;
	maxS = 193;
	
	minL = 0;
	maxL = 255;
	
	maxThrs = 255;
	
	minThrs = 15;
	erodeThreshold = 1;
	sqr_blur = 4;
	cfgnframes = BACKGROUND_NFRAMES;
	minAreaToTrack = MIN_AREA_TO_TRACK;
	
	// droppingframes = FRAMES_TO_DROP*2;
	print_now = 1;
	
	camera = false;
	save = false;
	gui = false;
	
	readyTo2Dtrack = false;
	
	remoteCamera = false;
	
	options = (options2dTrack *)malloc(sizeof(options2dTrack));
	
	lockedSendOutImage = IMG_SENT;
	
	setRawVideo = false;
	GUImode = 2;
	
	// cvInitFont(&fpsFont,CV_FONT_HERSHEY_SIMPLEX, 0.5,0.5,0,1);
	cvInitFont(&fpsFont,CV_FONT_HERSHEY_SIMPLEX , 1,0.1,1,1,4);
	
	refZoom = 40;
	
	pfps = clsFPS();
}
clsVideo::~clsVideo()
{
	// cvCreateImage 	cvReleaseImage
	// cvCreateImageHeader 	cvReleaseImageHeader
	// cvCreateMat 	cvReleaseMat
	// cvCreateMatND 	cvReleaseMatND
	// cvCreateData 	cvReleaseData
	// cvCreateSparseMat 	cvReleaseSparseMat
	// cvCreateMemStorage 	cvReleaseMemStorage
	// cvCreateGraphScanner 	cvReleaseGraphScanner
	// cvOpenFileStorage 	cvReleaseFileStorage
	// cvAlloc 	cvFree
	cvReleaseImage(&frame);
	cvReleaseImage(&frame_smooth);
	cvReleaseImage(&frame_debug);
	cvReleaseImage(&frame_output);
	cvReleaseImage(&frame_to_send);
	cvReleaseImage(&frame_to_send_tracing);
	cvReleaseImage(&frame_to_send_calib);
	cvReleaseImage(&frame_buffer);
	cvReleaseImage(&frame_buffer2);
	cvReleaseImage(&frame_threshold);
	cvReleaseImage(&frame_countors);
	cvReleaseImage(&frame_sub);
	cvReleaseCapture(&capture);
        
}

bool clsVideo::init_video(int cameraId)
{
	INFOMSG(("opening device %d...",cameraId));
	cameraIdx = cameraId;
	camera = true;
	
	capture = cvCreateCameraCapture(cameraId);
	// capture = cvCreateCameraCapture_V4L(cameraId);
	return init_video();
}

bool clsVideo::init_video(char *filename)
{
	INFOMSG(("reading from file %s...\n",filename));
	//capture = cvCreateFileCapture(filename);
	capture = cvCaptureFromAVI(filename);
	
	strcpy(myFilename,filename);
	return init_video();
}

bool clsVideo::init_video()
{
	// 30 fps: !!
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,800);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,600);
	
	frame = cvQueryFrame(capture);
	if(frame == NULL)
	{
		ERRMSG(("failed to start video...\n"));
		return false;
	}
	
	// camSpecs();
	
	
	INFOMSG(("Image size: %d %d",frame->width,frame->height));
	return true;
}
bool clsVideo::camSpecs()
{
	INFOMSG(("CV_CAP_PROP_POS_MSEC        %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_POS_MSEC      )));
	INFOMSG(("CV_CAP_PROP_POS_FRAMES      %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES    )));
	INFOMSG(("CV_CAP_PROP_POS_AVI_RATIO   %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_POS_AVI_RATIO )));
	INFOMSG(("CV_CAP_PROP_FRAME_WIDTH     %.10f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH   )));
	INFOMSG(("CV_CAP_PROP_FRAME_HEIGHT    %.10f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT  )));
	INFOMSG(("CV_CAP_PROP_FPS             %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_FPS           )));
	INFOMSG(("CV_CAP_PROP_FOURCC          %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_FOURCC        )));
	INFOMSG(("CV_CAP_PROP_FRAME_COUNT     %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT   )));
	INFOMSG(("CV_CAP_PROP_FORMAT          %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_FORMAT        )));
	INFOMSG(("CV_CAP_PROP_MODE            %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_MODE          )));
	INFOMSG(("CV_CAP_PROP_BRIGHTNESS      %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_BRIGHTNESS    )));
	INFOMSG(("CV_CAP_PROP_CONTRAST        %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_CONTRAST      )));
	INFOMSG(("CV_CAP_PROP_SATURATION      %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_SATURATION    )));
	INFOMSG(("CV_CAP_PROP_HUE             %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_HUE           )));
	INFOMSG(("CV_CAP_PROP_GAIN            %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_GAIN          )));
	INFOMSG(("CV_CAP_PROP_EXPOSURE        %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_EXPOSURE      )));
	INFOMSG(("CV_CAP_PROP_CONVERT_RGB     %.4f\n",cvGetCaptureProperty(capture,CV_CAP_PROP_CONVERT_RGB   )));
	
	return true;
}
bool clsVideo::start_2dpts(options2dTrack *currOptions)
{
	
	camera2DCFG camera2DCFGs;
	loadHARDCODEDOPTIONS(&camera2DCFGs,currOptions->cameraSection,currOptions->cameraPosition);
	
	
	chg_mode = camera2DCFGs.type2Dtracking;
	minThrs = camera2DCFGs.minThrs;
	erodeThreshold =camera2DCFGs.erode;
	sqr_blur =camera2DCFGs.sqr_blur;
	
	INFOMSG(("Loading settings mode: %d thrs: %d erode: %d blur: %d",chg_mode,minThrs,erodeThreshold,sqr_blur));
	
	options = currOptions;
	gui = options->gui;
	//chg_mode = SIMPLE_FRAMESUBSTRACTION;
	if(gui || options->clickTracking)
	{
		
		cvNamedWindow(WINDOW_NAME,CV_WINDOW_AUTOSIZE); //resizable window;
		// cvNamedWindow(WINDOW_NAME_DEBUG,CV_WINDOW_AUTOSIZE); //resizable window;
		// cvNamedWindow(WINDOW_NAME_DEBUG2,CV_WINDOW_AUTOSIZE); //resizable window;
		
		cvSetMouseCallback(WINDOW_NAME, pickXandY,NULL);
		
		if(options->showGUIsettings)
		{
			
			cvCreateTrackbar("min", WINDOW_NAME, &minThrs, 255, trackbar_callback);
			cvCreateTrackbar("erode", WINDOW_NAME, &erodeThreshold, 20, trackbar_callback);			
			cvCreateTrackbar("blur", WINDOW_NAME, &sqr_blur, 25, trackbar_callback);
			cvCreateTrackbar("nrframes", WINDOW_NAME, &cfgnframes, 255, trackbar_callback);
			cvCreateTrackbar("minArea", WINDOW_NAME, &minAreaToTrack, 20, trackbar_callback);
			
		}
	}
	
        frame_buffer = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_buffer2 = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_output = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        
        frame_to_send = cvCreateImage( cvSize( CAMREC_WIDTH, CAMREC_HEIGHT ),
        	IPL_DEPTH_8U, 3  );
        frame_to_send_buffer_tracing = cvCreateImage( cvSize( frame->width, frame->height ),
        	IPL_DEPTH_8U, 3  );
        frame_to_send_tracing = cvCreateImage( cvSize( CAMREC_WIDTH, CAMREC_HEIGHT ),
        	IPL_DEPTH_8U, 3  );
        frame_to_send_calib = cvCreateImage( cvSize(  frame->width, frame->height ),
        	IPL_DEPTH_8U, 3  );
        frame_debug = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_bufferbgfg = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        sprintf(frame_to_send->colorModel,"RGB");
        sprintf(frame_to_send->channelSeq,"RGB");
        
        sprintf(frame_to_send_calib->colorModel,"RGB");
        sprintf(frame_to_send_calib->channelSeq,"RGB");
        
        sprintf(frame_to_send_tracing->colorModel,"RGB");
        sprintf(frame_to_send_tracing->channelSeq,"RGB");
        
        
        frame_threshold = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, 1);
        frame_countors = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, 1);
        frame_smooth = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels);
        
        frame_debug  = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, 1);
        frame_sub =  cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        cvCopy(frame,frame_buffer);
        
        particle_curr = new clsParticle(0,0);
        particle_curr->color = CV_RGB(0,255,0);
        
        readyTo2Dtrack = true;
        
        if(chg_mode == BGFG_CODEBOOK )
        {
        	startBGFGCodeBook();
        	readyTo2Dtrack = false;
        }
        
        pfps.resetCount();
        
        return true;
}
bool clsVideo::startRemoteCamera(IplImage *remoteFrame)
{
	frame = cvCreateImage( cvSize( remoteFrame->width, remoteFrame->height ),
        	remoteFrame->depth, remoteFrame->nChannels );
	
	return true;
}
void clsVideo::setCurrentFrame(IplImage *remoteframe)
{
	cvCopy(remoteframe,frame);
}

void *recordIMG(void *)
{
	
	clsVideoRecord *recordSegmentations;
	IplImage *frame_out_record =  cvCreateImage( cvSize( frame_raw->width, frame_raw->height*3 ),
        	frame_raw->depth, frame_raw->nChannels );
	
	
	recordSegmentations = new clsVideoRecord();
        recordSegmentations->record((char *)"SEGMENTATIONS_COMPARE.avi",cvGetSize(frame_out_record),30);
        usleep(60000);
	while(recordingIMGs)
	{
		/*
		cvSetImageROI(frame_out_record, cvRect(0,0,frame_raw->width, frame_raw->height));
		pthread_mutex_lock(&MUTEX_subtr);
		cvResize(frame_output_subtraction_thrs, frame_out_record);
		pthread_mutex_unlock(&MUTEX_subtr);
		cvResetImageROI(frame_out_record);
		*/
		cvSetImageROI(frame_out_record, cvRect(0, frame_raw->height,frame_raw->width, frame_raw->height));
		pthread_mutex_lock(&MUTEX_bgfg);
		cvResize(frame_output_bgfg_thrs, frame_out_record);
		pthread_mutex_unlock(&MUTEX_bgfg);
		cvResetImageROI(frame_out_record);
		
		cvSetImageROI(frame_out_record, cvRect(0,0,frame_raw->width, frame_raw->height));
		pthread_mutex_lock(&MUTEX_frame);
		cvResize(frame_raw, frame_out_record);
		pthread_mutex_unlock(&MUTEX_frame);
		cvResetImageROI(frame_out_record);
		
		recordSegmentations->addNewFrame(frame_out_record);
		
	}
	
	recordSegmentations->stop();
	
	return 0;
}

bool clsVideo::allocOneParticleTrackingTest()
{

	frame_output_subtraction_thrs = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
	frame_output_bgfg_thrs = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_raw = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        
        particlesubtraction = new clsParticle();
        particlebgfg = new clsParticle();
        
        particlebgfg->color = CV_RGB(0,255,0);
        
        particlesubtraction->color = CV_RGB(0,0,255);
        
        recordingIMGs = true;
        
        pthread_create(&trecording,NULL,recordIMG,NULL);
        
        
	minThrs = 20;
	erodeThreshold = 2;
	sqr_blur = 7;
	cfgnframes = 100;
	
        return true;
}

bool clsVideo::oneParticleTrackingTest()
{
	if(!pause)
		frame = cvQueryFrame(capture);
	
	/**
	**
	FRAME SUBTRACTION SECTION
	**
	**/
	/*
	cvSmooth(frame, frame_smooth, CV_BLUR, sqr_blur+1, sqr_blur+1, 0, 0);
	framesubtraction();
	
	//removing loose points
	cvErode(frame_threshold, frame_threshold, NULL,erodeThreshold);
	
	//augmenting neighbour points
	cvDilate(frame_threshold, frame_threshold, NULL,1);
	
	cvCopy(frame_threshold,frame_countors);
	
	findParticle(frame_countors,&point);
	particlesubtraction->set_new_coords(point.x,point.y);
	particlesubtraction->surroundRect1 = particle_curr->surroundRect1;
	particlesubtraction->surroundRect2 = particle_curr->surroundRect2;
	
	pthread_mutex_lock(&MUTEX_subtr);
	cvCvtColor( frame_threshold, frame_output_subtraction_thrs,CV_GRAY2BGR );
	// particlesubtraction->draw_trail(frame_output_subtraction_thrs);
	pthread_mutex_unlock(&MUTEX_subtr);*/
	/**
	**
	BG FG code book
	**
	**/
	
	cvSmooth(frame, frame_smooth, CV_BLUR, sqr_blur+1, sqr_blur+1, 0, 0);
	bgfgCodeBook();
	// 
	// //removing loose points
	cvErode(frame_threshold, frame_threshold, NULL,erodeThreshold);
	
	//augmenting neighbour points
	cvDilate(frame_threshold, frame_threshold, NULL,1);
	
	cvCopy(frame_threshold,frame_countors);
	
	findParticle(frame_countors,&point);
	
	particlebgfg->set_new_coords(point.x,point.y);
	particlebgfg->surroundRect1 = particle_curr->surroundRect1;
	particlebgfg->surroundRect2 = particle_curr->surroundRect2;
	
	pthread_mutex_lock(&MUTEX_bgfg);
	cvCvtColor( frame_threshold, frame_output_bgfg_thrs,CV_GRAY2RGB );
	particlebgfg->draw_trail(frame_output_bgfg_thrs);
	pthread_mutex_unlock(&MUTEX_bgfg);
	
	cvCopy(frame,frame_output);
	particlebgfg->draw_trail(frame_output);
	
	pthread_mutex_lock(&MUTEX_frame);
	particlesubtraction->draw_trail(frame_output);
	cvResize(frame_output, frame_raw);
	pthread_mutex_unlock(&MUTEX_frame);
	
	cvShowImage(WINDOW_NAME, frame_output);
	cvShowImage("ANOTHER", frame_output_subtraction_thrs);
	cvShowImage("BGFG", frame_output_bgfg_thrs);
	
	char key;
	
	key = (char)cvWaitKey(1); //delay N millis, usually long enough to display and capture input
	switch (key) {
	case ' ':
		INFOMSG(("pause"));
		pause = !pause;
		break;
	case 'r':
		DEBUGMSG(("Restarting BGFG_CodeBook model"));
		nframes = 0;
		break;
	case 'g':
		GUImode++;
		if(GUImode > 2)
		{
			GUImode = 0;
		}
		INFOMSG(("changed to guimode: %d",GUImode));
		break;
	case 'k':
		chg_mode++;
		if(chg_mode > 2)
		{
			chg_mode = 0;
		}
		INFOMSG(("changed to mode %d",chg_mode));
		break;
	case 'h':
		setRawVideo = !setRawVideo;
		break;
	case 'q':
	case 'Q':
	case 27: //escape key
		
		recordingIMGs = false;
		pthread_join(trecording,NULL);
		
		return false;
	default:
		break;
		
	
	}
	return true;
}
bool clsVideo::oneParticleTracking(pts2d *points)
{	
	if(!pause){
		
		frame_output = cvQueryFrame(capture);
		
		if(!frame_output) {
			INFOMSG(("end of file..."));
			init_video(myFilename);
			return true;
		}
		cvResize(frame_output,frame_to_send_buffer_tracing);
		cvResize(frame_output,frame_to_send);
		
	}
	
	
	//smoothing the frame
	cvSmooth(frame_output, frame_smooth, CV_BLUR, sqr_blur+1, sqr_blur+1, 0, 0);

	switch(chg_mode)
	{
	case COLOR_THRESHOLD:
		colorThreshold();
		break;
	case SIMPLE_FRAMESUBSTRACTION:
		framesubtraction();
		break;
	case BGFG_CODEBOOK:
		bgfgCodeBook();
		break;	
	}
	
	
	//removing loose points
	cvErode(frame_threshold, frame_threshold, NULL,erodeThreshold);
	if(GUImode == 0 || GUImode == 1)
	{
		
		cvCvtColor( frame_threshold, frame_output, CV_GRAY2RGB );
		
	}
	
	//augmenting neighbour points
	cvDilate(frame_threshold, frame_threshold, NULL,1);
	
	cvCopy(frame_threshold,frame_countors);
	
	findParticle(frame_countors,&points[0]);
	
	
	//cvCopy(frame_to_send,frame_to_send_buffer_tracing);
	particle_curr->draw_trail(frame_to_send_buffer_tracing);
	sprintf(tmpmsg,"xPx: %d yPx: %d",particle_curr->x,particle_curr->y);
	cvPutText(frame_to_send_buffer_tracing,tmpmsg,cvPoint(10,20),&fpsFont,cvScalar(255,0,0));
	
	cvResize(frame_to_send_buffer_tracing,frame_to_send_tracing);
	
	if(!gui )
	{
		if(fps < 25)
		{
			INFOMSG(("LOW FRAME RATE: %.4f",fps));
		}
	}else{
		
		if(GUImode == 0 || GUImode == 2)
		{
			particle_curr->draw_trail(frame_output);
			
			if(!pause){
				
				sprintf(tmpmsg,"%.2f fps - xPx: %d yPx: %d",fps,particle_curr->x,particle_curr->y);
				if(fps < 25)
					cvPutText(frame_output,tmpmsg,cvPoint(frame_output->width-300,frame_output->height-20),&fpsFont,CV_RGB(255,0,0));
				else
					cvPutText(frame_output,tmpmsg,cvPoint(frame_output->width-300,frame_output->height-20),&fpsFont,cvScalar(255,0,0));
			}
		}
		
		
		cvShowImage(WINDOW_NAME, frame_output);
		
		char key;
		
		key = (char)cvWaitKey(1); //delay N millis, usually long enough to display and capture input
		switch (key) {
		//case ' ':
		//	INFOMSG(("pause"));
		//	pause = !pause;
		//	break;
		case 'r':
			DEBUGMSG(("Restarting BGFG_CodeBook model"));
			nframes = 0;
			break;
		case 'g':
			GUImode++;
			if(GUImode > 3)
			{
				GUImode = 0;
			}
			INFOMSG(("changed to guimode: %d",GUImode));
			break;
		case 'k':
			chg_mode++;
			if(chg_mode > 2)
			{
				chg_mode = 1;
			}
			break;
		//case 'h':
		//	setRawVideo = !setRawVideo;
		//	break;
		case 'q':
		case 'Q':
		case 27: //escape key
			return false;
		default:
			break;
		}
	}
	
	if(!pause){
		fps = pfps.updateFPS();
	}

	
	cvCvtColor(frame_to_send_tracing,frame_to_send_tracing, CV_BGR2RGB);
	cvCvtColor(frame_to_send,frame_to_send, CV_BGR2RGB);
	return true;
}

bool clsVideo::clickTracking(pts2d *points)
{
	if(!pause){
		
		frame = cvQueryFrame(capture);
		
		if(!frame) {
			INFOMSG(("end of file..."));
			init_video(myFilename);
			return true;
		}
		
		if(clickedX > 0 ){
			INFOMSG(("%d %d",clickedX,clickedY));
			points[0].x = (double)clickedX;
			points[0].y = (double)clickedY;
			points[0].xPx = clickedX;
			points[0].yPx = clickedY;
			points[0].timestamp = getTimestamp();
			
			particle_curr->set_new_coords(clickedX,clickedY);
		}
		cvCopy(frame,frame_output);
	}
	
	particle_curr->draw_trail(frame_output);
	
	cvShowImage(WINDOW_NAME, frame_output);
	
	char key = (char)cvWaitKey(FRAME_SLEEP); //delay N millis, usually long enough to display and capture input
	switch (key) {
	case 'q':
	case 'Q':
	case 27: //escape key
		return false;
	default:
		break;
	}
	
	fps = pfps.updateFPS();
	
	// if(points[0].x > 0){
	// /** REMOVEEEEEE THISSS ***/
	// pts2d *thisPoint = (pts2d *)malloc(sizeof(pts2d));
	// *thisPoint = points[0];
	// 
	// printf("%d %d - BF: %.2f %.2f -> ",thisPoint->cameraSection,thisPoint->cameraPosition,thisPoint->x,thisPoint->y);
	// pixelsToArenaCms(thisPoint);
	// printf("AF: %.2f %.2f\n",thisPoint->x,thisPoint->y);
	// 
	// /** REMOVEEEEEE THISSS ***/
	// }
	
	
	return true;
}

bool clsVideo::showVideo()
{
	// !capture.read(frame)
	frame = cvQueryFrame(capture);
	
	if(frame == NULL) {
		INFOMSG(("end of file..."));
		init_video((char *)"self.avi");
		return false;
	}
	sprintf(tmpmsg,"%.2f",fps);
	cvPutText(frame,tmpmsg,cvPoint(10,20),&fpsFont,cvScalar(255,0,0));
	
	cvCvtColor(frame,frame_to_send_calib, CV_BGR2RGB);
	fps = pfps.updateFPS();
	// cvShowImage(WINDOW_NAME, frame);
	
	// char key = (char)cvWaitKey(10); //delay N millis, usually long enough to display and capture input
	// switch (key) {
	// case 'q':
	// case 'Q':
	// case 27: //escape key
		// return true;
	// default:
		// break;
	// }
	
	// persAddNewSample = true;
	
	return true;
}


bool clsVideo::startCalibratePerspectiveMatrices(int cameraSection,int cameraPosition)
{
	INFOMSG(("Starting to calibrate the perspective matricessss"));
	
	#ifndef DO_NOT_USE_INTRINSIC
	//load intrinsic params
	PERSPECTIVE_MATRICES_intrinsicParams = cvCreateMat(3, 3, CV_32FC1);
	if(!loadIntrinsic(PERSPECTIVE_MATRICES_intrinsicParams,cameraSection,cameraPosition))
	{
		INFOMSG(("failed to load intrinsic params"));
		return false;
	}
	PrintMat(PERSPECTIVE_MATRICES_intrinsicParams);
	
	
	//load distortion params
	PERSPECTIVE_MATRICES_distortionCoefficients = cvCreateMat(5, 1, CV_32FC1);
	if(!loadDistortionCoeffs(PERSPECTIVE_MATRICES_distortionCoefficients,cameraSection,cameraPosition))
	{
		INFOMSG(("failed to load distortion coefffs"));
		return false;
	}
	PrintMat(PERSPECTIVE_MATRICES_distortionCoefficients);
	
	
	//undistort image
	PERSPECTIVE_MATRICES_mx = cvCreateMat(imageSize.width, imageSize.height , CV_32FC1);
	PERSPECTIVE_MATRICES_my = cvCreateMat(imageSize.width, imageSize.height , CV_32FC1);
	cvInitUndistortMap(PERSPECTIVE_MATRICES_intrinsicParams,PERSPECTIVE_MATRICES_distortionCoefficients,PERSPECTIVE_MATRICES_mx,PERSPECTIVE_MATRICES_my);
	INFOMSG(("loaded undistortion map"));
	#endif
	DEBUGMSG(("1"));
	//get sizes in cms!!
	
	
	PERSPECTIVE_MATRICES_arenaPointsInCm = new CvPoint2D32f[4];
	DEBUGMSG(("2"));
	loadArenaInCms(cameraSection,cameraPosition,PERSPECTIVE_MATRICES_arenaPointsInCm);
	DEBUGMSG(("3"));
	DEBUGMSG(("Arena loaded..."));
	//start place to store the perspective matrices calculated
	PERSPECTIVE_MATRICES_perspectiveMatrixes = cvCreateMat(5, 9, CV_32FC1);
	
	//place to store the points of the window
	PERSPECTIVE_MATRICES_pointsSelected.resize(4);
	
	PERSPECTIVE_MATRICES_nrSelectedPoints = 0;
	PERSPECTIVE_MATRICES_calculateMatrix = -1;
	
	//start stuff
	TODO(("GET EVENTS FROM THE MOUSE"));
	cvNamedWindow(WINDOW_NAME);
	cvSetMouseCallback( WINDOW_NAME, PERSPECTIVE_MATRICES_mouseHandler, NULL );
        cvInitFont(&PERSPECTIVE_MATRICES_font,CV_FONT_HERSHEY_SIMPLEX, 0.5,0.5,0,1);
	PERSPECTIVE_MATRICES_perspectiveMatrixes = cvCreateMat(3, 9, CV_32FC1);
	cvCreateTrackbar("zoom", WINDOW_NAME, &refZoom, 100, trackbar_callback);
	
	//FORCING FOR FASTER STUFF
	PERSPECTIVE_MATRICES_pointsSelected[0] = cvPoint(0+50,0+50);
	PERSPECTIVE_MATRICES_pointsSelected[1] = cvPoint(frame->width-50,0+50);
	PERSPECTIVE_MATRICES_pointsSelected[2] = cvPoint(0+50,frame->height-50);
	PERSPECTIVE_MATRICES_pointsSelected[3] = cvPoint(frame->width-50,frame->height-50);
	
	PERSPECTIVE_MATRICES_nrSelectedPoints = 4;
	
	perspectiveMatrix = cvCreateMat(3,3,CV_32FC1);
	
	frame_buffer = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels ); 
	pDst.x = 0;
	pDst.y = 0;
	
	return true;
}

bool clsVideo::calibratePerspectiveMatrices()
{
	
	if(!remoteCamera)
	{
		frame = cvQueryFrame(capture);
	}
	
	// cvUndistort2(frame,frame,intrinsicParams,distortionCoefficients);
	// DO STUFFF
	
	//DRAW SELECTED POINTS WITH NUMBER ? 
	drawMarkers(frame, PERSPECTIVE_MATRICES_pointsSelected);
	//USE COLORS FOR EACH MARK!!! OR ADD A NUMBER IN THE MARK! x)
	zoomHandler(frame);
	if(PERSPECTIVE_MATRICES_nrSelectedPoints == 4 && PERSPECTIVE_MATRICES_calculateMatrix > 0)
	{
		INFOMSG(("Adding new matrix %d",PERSPECTIVE_MATRICES_pointsSelected.size()));
		
		//FIND SUBPIXELS
		CvPoint2D32f* distortedPointsSubPixel = new CvPoint2D32f[ 4 ];
		
		for(int i = 0; i < 4; i++)
		{
			distortedPointsSubPixel[i] = cvPointTo32f(PERSPECTIVE_MATRICES_pointsSelected[i]);
		}
		
		//#ifdef CALCULATE_SUBPIXELS
		IplImage *persFrameGray = cvCreateImage(cvGetSize( frame ), IPL_DEPTH_8U, 1);
		cvCvtColor(frame, persFrameGray, CV_BGR2GRAY);
		cvFindCornerSubPix(persFrameGray, distortedPointsSubPixel, 4,
			cvSize(10, 10), cvSize(-1,-1),
			cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03) // FROM THE BOOK
			// cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ) // FROM OLD CODE
			);
		cvReleaseImage(&persFrameGray);
		
		//#endif
		
		//undistort points
		/**
		// CHOOSE HERE IF UNDISTORTED OR NOT??? WHY CANT BE UNDISTORTED?? FIXME
		TODO(("in here we have to undistort the image!!!"));
		CvMat* distortedPointsSubPixelCvMat   = cvCreateMat(4,1,CV_32FC2);
		CvMat* undistortedPointsSubPixel = cvCreateMat(4,1,CV_32FC2);
		
		for(int i = 0; i < PERSPECTIVE_MATRICES_pointsSelected.size(); i++)
		{
		
		distortedPointsSubPixelCvMat->data.fl[i*2]   = distortedPointsSubPixel[i].x;
		distortedPointsSubPixelCvMat->data.fl[i*2+1] = distortedPointsSubPixel[i].y;
		
		
		}
		
		cvUndistortPoints(distortedPointsSubPixelCvMat,undistortedPointsSubPixel,PERSPECTIVE_MATRICES_intrinsicParams,PERSPECTIVE_MATRICES_distortionCoefficients);
		
		// calculate perspective matrix
		CvPoint2D32f* undistortedPointsSubPixelToPerspective = new CvPoint2D32f[4];
		
		for(int i = 0; i < PERSPECTIVE_MATRICES_pointsSelected.size(); i++){
		undistortedPointsSubPixelToPerspective[i].x = undistortedPointsSubPixel->data.fl[i*2];
		undistortedPointsSubPixelToPerspective[i].y = undistortedPointsSubPixel->data.fl[i*2+1];
		// 
		// CV_MAT_ELEM(mat, elem, row, col).f1;
		// 
		// undistortedPointsSubPixelToPerspective[i].x = CV_MAT_ELEM(undistortedPointsSubPixel, elem, row, col).f1;
		// cvmGet(undistortedPointsSubPixel,i,0).f1;
		// undistortedPointsSubPixelToPerspective[i].y = cvmGet(undistortedPointsSubPixel,i,0).f2;
		}
		**/
		
		// cvGetPerspectiveTransform(undistortedPointsSubPixelToPerspective, pointsInCm , perspectiveMatrix);
		cvGetPerspectiveTransform(distortedPointsSubPixel, PERSPECTIVE_MATRICES_arenaPointsInCm , perspectiveMatrix);
		
		//store the matrix in the array of matrices!!
		int nMatrix;
		nMatrix = PERSPECTIVE_MATRICES_calculateMatrix - 1;
		
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,0,cvmGet(perspectiveMatrix,0,0));
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,1,cvmGet(perspectiveMatrix,0,1));
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,2,cvmGet(perspectiveMatrix,0,2));         
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,3,cvmGet(perspectiveMatrix,1,0));        
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,4,cvmGet(perspectiveMatrix,1,1));
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,5,cvmGet(perspectiveMatrix,1,2));
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,6,cvmGet(perspectiveMatrix,2,0));
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,7,cvmGet(perspectiveMatrix,2,1));
		cvmSet(PERSPECTIVE_MATRICES_perspectiveMatrixes,nMatrix,8,cvmGet(perspectiveMatrix,2,2));
		
		PrintMat(PERSPECTIVE_MATRICES_perspectiveMatrixes);       
		
	}
	
	
	sprintf(tmpmsg,"%.2f %.2f",pDst.x,pDst.y);
	cvPutText(frame,tmpmsg,cvPoint(10,20),&fpsFont,cvScalar(255,0,0));
	
	//FINISHING STUFF
	cvShowImage(WINDOW_NAME, frame);
	char key = (char)cvWaitKey(FRAME_SLEEP); //delay N millis, usually long enough to display and capture input
	switch (key)
	{
	case '1':
		PERSPECTIVE_MATRICES_calculateMatrix = 1;
		break;
	case '2':
		PERSPECTIVE_MATRICES_calculateMatrix = 2;
		break;
	case '3':
		PERSPECTIVE_MATRICES_calculateMatrix = 3;
		break;
	case 's':
	case 'S':
		save = true;
		return true;
		break;
	case 'q':
	case 'Q':
	case 27: //escape key
		return true;
		break;
	default:
		PERSPECTIVE_MATRICES_calculateMatrix = -1;
		break;
	}
	
	if(clickedX >0 && clickedY > 0)
	{
		pts2d pSrc;
		
		pSrc.x = clickedX;
		pSrc.y = clickedY;
		
		CvScalar p1;
		CvMat tsrc, tdst;
		CvMat* src = cvCreateMat( 2, 1,CV_32FC1);
		CvMat* dst = cvCreateMat( 2, 1, CV_32FC1 );
		
		cvmSet(src,0,0,pSrc.x);
		cvmSet(src,1,0,pSrc.y);
		
		cvReshape( src, &tsrc, 2, 0 );
		cvReshape( dst, &tdst, 2, 0 );
		
		cvPerspectiveTransform( &tsrc, &tdst, perspectiveMatrix );
		
		p1=cvGet2D(&tdst,0,0);
		pDst.x=p1.val[0];
		pDst.y=p1.val[1];
		
		//printf("%.2f %.2f ==> %.2f %.2f\n",pSrc.x,pSrc.y,pDst.x,pDst.y);
	}
	return false;
}

bool clsVideo::finishCalibratePerspectiveMatrices(int cameraSection,int cameraPosition )
{
	if(save){
		savePerspectiveMatrices(PERSPECTIVE_MATRICES_perspectiveMatrixes,cameraSection,cameraPosition);
	}
	return true;
}

bool clsVideo::startCalibratePerspectiveLines()
{
	int hInc = frame->height / 3;
	
	pointsPerspectiveLines[0].x = 0;
	pointsPerspectiveLines[0].y = hInc*1;
	
	pointsPerspectiveLines[1].x = frame->width;
	pointsPerspectiveLines[1].y = hInc*1;
	
	pointsPerspectiveLines[2].x = 0;
	pointsPerspectiveLines[2].y = hInc*2;
	
	pointsPerspectiveLines[3].x = frame->width;
	pointsPerspectiveLines[3].y = hInc*2;
	
	
	cvSetMouseCallback( WINDOW_NAME, perspectiveLinesMouseHandler, NULL );
	
	return true;
}
#define RADIUS_REC_PERSPECTIVE 10
bool clsVideo::calibratePerspectiveLines()
{
	for(int i=0;i<4;i+=2)
	{
		cvLine(frame,pointsPerspectiveLines[i],pointsPerspectiveLines[i+1], CV_RGB(0,255,0),1);
		cvRectangle(frame,cvPoint(pointsPerspectiveLines[i].x-RADIUS_REC_PERSPECTIVE,pointsPerspectiveLines[i].y-RADIUS_REC_PERSPECTIVE),cvPoint(pointsPerspectiveLines[i].x+RADIUS_REC_PERSPECTIVE,pointsPerspectiveLines[i].y+RADIUS_REC_PERSPECTIVE), CV_RGB(255,0,0),1);
		cvRectangle(frame,cvPoint(pointsPerspectiveLines[i+1].x-RADIUS_REC_PERSPECTIVE,pointsPerspectiveLines[i+1].y-RADIUS_REC_PERSPECTIVE),cvPoint(pointsPerspectiveLines[i+1].x+RADIUS_REC_PERSPECTIVE,pointsPerspectiveLines[i+1].y+RADIUS_REC_PERSPECTIVE), CV_RGB(255,0,0),1);
	}
	cvShowImage(WINDOW_NAME, frame);
	
	//FINISHING STUFF
	char key = (char)cvWaitKey(FRAME_SLEEP); //delay N millis, usually long enough to display and capture input
	switch (key) {
	case 's':
	case 'S':
		save = true;
		return true;
	case 'q':
	case 'Q':
	case 27: //escape key
		return true;
	default:
		break;
	}
	return false;
}
bool clsVideo::finishCalibratePerspectiveLines(int cameraSection,int cameraPosition )
{
	if(save){
		CvMat *lines;
		lines = cvCreateMat(4, 2, CV_32FC1);
		for(int i=0;i < 4;i++){
			cvmSet(lines,i,0, pointsPerspectiveLines[i].x);
			cvmSet(lines,i,1, pointsPerspectiveLines[i].y);
		}
		
		char filename[128];
		
		snprintf(filename,128,"%s_%d_%d.xml",LINES_MATRICES_NAME,cameraSection,cameraPosition);
		
		cvSave(filename,lines);
		
		INFOMSG(("perspective lines saved to: %s",filename));
		return true;
	}
	return true;
}

void clsVideo::PrintMat(CvMat *A)
{
        int i, j;
        
        // printf("\n %d %d \n",A->rows,A->cols);
        
        for (i = 0; i < A->rows; i++)
        {
        	switch (CV_MAT_DEPTH(A->type))
        	{
        	case CV_32F:
        		for (j = 0; j < A->cols; j++)
        		{
        			// printf("> %d ",i);
        			// printf ("%.3f ",A->data.fl[i*2]);
        			// printf ("%.3f ",A->data.fl[i*2+1]);
        			printf ("%.3f ", (float)cvGetReal2D(A, i, j));
        		}
        		
        		break;
        		// case CV_32F:
        	case CV_64F:
        		for (j = 0; j < A->cols; j++)
        			printf ("%.3f ", (float)cvGetReal2D(A, i, j));
        		break;
        	case CV_8U:
        	case CV_16U:
        		for(j = 0; j < A->cols; j++)
        			printf ("%d",(int)cvGetReal2D(A, i, j));
        		break;
        	case CV_32SC1:
        		for (j = 0; j < A->cols; j++)
        			printf ("%d ",(int)cvGetReal2D(A, i, j));
        		break;
        	default:
        		break;
        	}
        	printf("\n");
        }
        
        printf("\n");
}

void clsVideo::drawMarkers(IplImage *frame, vector<CvPoint> points )
{
	
	char tmp_txt[10];
        for(int i = 0; i < PERSPECTIVE_MATRICES_nrSelectedPoints; i++){
                // cvCircle(frame,points[i],RADIUS_TO_DRAG,CV_RGB(0, 0, 255),2);
                cvCircle(frame,points[i],1, CV_RGB(0, 255, 0),1);
                cvRectangle(frame,cvPoint(points[i].x-RADIUS_REC_PERSPECTIVE,points[i].y-RADIUS_REC_PERSPECTIVE),cvPoint(points[i].x+RADIUS_REC_PERSPECTIVE,points[i].y+RADIUS_REC_PERSPECTIVE),CV_RGB(0, 0, 255),2);
                snprintf(tmp_txt,9,"%d",i+1);
                cvPutText(frame,tmp_txt,points[i],&PERSPECTIVE_MATRICES_font,CV_RGB(255, 0, 0));
        }
        
        cvLine(frame,points[0],points[1],CV_RGB(0,255,0));
        
        cvLine(frame,points[1],points[3],CV_RGB(0,255,0));
        
        cvLine(frame,points[2],points[3],CV_RGB(0,255,0));
        cvLine(frame,points[2],points[0],CV_RGB(0,255,0));
        
}

bool clsVideo::loadIntrinsic(CvMat *matrix,int cameraSection, int cameraPosition)
{
	char filename[128];
	
	snprintf(filename,128,"%s_%d_%d.xml",INTRISIC_MATRICES_NAME,cameraSection,cameraPosition);
	
	if(!fileexists(filename))
	{
		return false;
	}
	
	CvMat *matrix_aux;
	matrix_aux = (CvMat*)cvLoad(filename);
	cvCopy(matrix_aux,matrix);
	
	INFOMSG(("loaded intrinsic: %s",filename));
	return true;
}

bool clsVideo::loadDistortionCoeffs(CvMat *matrix,int cameraSection, int cameraPosition)
{
	char filename[128];
	
	snprintf(filename,128,"%s_%d_%d.xml",DISTORTION_MATRICES_NAME,cameraSection,cameraPosition);
	
	if(!fileexists(filename))
	{
		return false;
	}
	
	CvMat *matrix_aux;
	matrix_aux = (CvMat*)cvLoad(filename);
	cvCopy(matrix_aux,matrix);
	
	INFOMSG(("loaded distortion coeffs: %s",filename));
	return true;
}
bool clsVideo::loadPerspectiveMatrices(CvMat *matrix,int cameraSection, int cameraPosition)
{
	char filename[128];
	
	snprintf(filename,128,"%s_%d_%d.xml",PERSPECTIVE_MATRICES_NAME,cameraSection,cameraPosition);
	
	if(!fileexists(filename))
	{
		return false;
	}
	
	CvMat *matrix_aux;
	matrix_aux = (CvMat*)cvLoad(filename);
	cvCopy(matrix_aux,matrix);
	
	INFOMSG(("loaded perspective matrix: %s",filename));
	
	return true;
}

bool clsVideo::loadPerspectiveLines(CvMat *matrix,int sec,int pos)
{
	char filename[128];
	
	snprintf(filename,128,"%s_%d_%d.xml",LINES_MATRICES_NAME,sec,pos);
	
	if(!fileexists(filename))
	{
		return false;
	}
	
	CvMat *matrix_aux;
	matrix_aux = (CvMat*)cvLoad(filename);
	cvCopy(matrix_aux,matrix);
	
	INFOMSG(("loaded line matrix: %s",filename));
	
	return true;
}


bool clsVideo::saveIntrinsic(CvMat *matrix,int cameraSection, int cameraPosition)
{
	char filename[128];
	
	snprintf(filename,128,"%s_%d_%d.xml",INTRISIC_MATRICES_NAME,cameraSection,cameraPosition);
        
        cvSave(filename,matrix);
        
        INFOMSG(("intrinsic saved to: %s",filename));
        return true;
}

bool clsVideo::saveDistortionCoeffs(CvMat *matrix,int cameraSection, int cameraPosition)
{
	char filename[128];
	
	snprintf(filename,128,"%s_%d_%d.xml",DISTORTION_MATRICES_NAME,cameraSection,cameraPosition);
	
	cvSave(filename,matrix);
	INFOMSG(("distortion coeffs saved to: %s",filename));
	return true;
}

bool clsVideo::savePerspectiveMatrices(CvMat *matrix,int cameraSection, int cameraPosition)
{
	char filename[128];
	
	snprintf(filename,128,"%s_%d_%d.xml",PERSPECTIVE_MATRICES_NAME,cameraSection,cameraPosition);
	
	cvSave(filename,matrix);
	
	INFOMSG(("perspective matrices saved to: %s",filename));
	return true;
}
// 
bool clsVideo::start2DCorrect( )
{
	#ifndef DO_NOT_USE_INTRINSIC
	//load intrinsic
	for(int i = 0; i < 2;i++)
	{
		for(int j = 0; j < 2;j++)
		{
			allMatricesIntrinsic[i][j] = cvCreateMat(3, 3, CV_32FC1);
			if(!loadIntrinsic(allMatricesIntrinsic[i][j],i,j))
			{
				ERRMSG(("failed to load intrisinc matrix of section: %d position: %d",i,j));
				return false;
			}
			PrintMat(allMatricesIntrinsic[i][j]);
		}
	}
	
	//load dist coeffs
	for(int i = 0; i < 2;i++)
	{
		for(int j = 0; j < 2;j++)
		{
			allMatricesDistCoeffs[i][j] = cvCreateMat(5, 1, CV_32FC1);
			
			if(!loadDistortionCoeffs(allMatricesDistCoeffs[i][j],i,j))
			{
				INFOMSG(("failed to load dist matrix of section: %d position: %d",i,j));
				return false;
			}
			PrintMat(allMatricesDistCoeffs[i][j]);
		}
	}
	#endif
	//load perspective
	for(int i = 0; i < 2;i++)
	{
		for(int j = 0; j < 2;j++)
		{
			allMatricesPerspective[i][j] = cvCreateMat(3, 9, CV_32FC1);
			if(!loadPerspectiveMatrices(allMatricesPerspective[i][j],i,j))
			{
				INFOMSG(("failed to load dist matrix of section: %d position: %d",i,j));
				return false;
			}
			PrintMat(allMatricesPerspective[i][j]);
		}
	}
	
	//load perspective lines
	for(int i = 0; i < 2;i++)
	{
		for(int j = 0; j < 2;j++)
		{
			perspectiveLines[i][j] = cvCreateMat(4, 2, CV_32FC1);
			if(!loadPerspectiveLines(perspectiveLines[i][j],i,j))
			{
				INFOMSG(("failed to load perspective line matrix of section: %d position: %d",i,j));
				return false;
			}
			PrintMat(perspectiveLines[i][j]);
		}
	}
	
	arenaSizes = (CvMat*)cvLoad(PERSPECTIVE_MATRICES_CM);
	
	overlapXSideTimer.reset();
	overlapXTopTimer.reset();
	
	return true;
}
// // 
bool clsVideo::pixelsToArenaCms(pts2d *point,pts3D lastPoint)
{
	int perspectiveLine = 2;
	//undistort
	
	
	//select matrix
	int yLim[2];
	
	if(point->cameraPosition == TOP_CAMERA)
	{	
		perspectiveLine = 2;
		
		yLim[0] = cvmGet(perspectiveLines[point->cameraSection][SIDE_CAMERA],0,1);
		yLim[1] = cvmGet(perspectiveLines[point->cameraSection][SIDE_CAMERA],2,1);
		//INFOMSG(("LIMITS y: %d %d - yValue: %d",yLim[0],yLim[1],lastPoint.yPx));
		if(lastPoint.yPx <= yLim[0])
		{
			//INFOMSG(("CHOOSING MATRIX 2"));
			perspectiveLine = 2;
		}else
		if(lastPoint.yPx >= yLim[0] && lastPoint.yPx <= yLim[1])
		{
			//INFOMSG(("CHOOSING MATRIX 1"));
			perspectiveLine = 1;
		}else
		if(lastPoint.yPx >= yLim[1])
		{
			//INFOMSG(("CHOOSING MATRIX 0"));
			perspectiveLine = 0;
		}
		

	}
	if(point->cameraPosition == SIDE_CAMERA)
	{
		yLim[0] = cvmGet(perspectiveLines[point->cameraSection][TOP_CAMERA],0,1);
		yLim[1] = cvmGet(perspectiveLines[point->cameraSection][TOP_CAMERA],2,1);
		//INFOMSG(("LIMITS y: %d %d - zValue: %d",yLim[0],yLim[1],lastPoint.zPx));
		
		if(lastPoint.zPx <= yLim[0])
		{
			//INFOMSG(("CHOOSING MATRIX 0"));
			perspectiveLine = 0;
		}else
		if(lastPoint.zPx >= yLim[0] && lastPoint.zPx <= yLim[1])
		{
			//INFOMSG(("CHOOSING MATRIX 1"));
			perspectiveLine = 1;
		}else
		if(lastPoint.zPx >= yLim[1])
		{
			//INFOMSG(("CHOOSING MATRIX 2"));
			perspectiveLine = 2;
		}
		
	}
	
	CvPoint2D32f pDst;
	
	calculatePerspective(*point,allMatricesPerspective[point->cameraSection][point->cameraPosition],&pDst,perspectiveLine);
		
	point->x = pDst.x;
	point->y = pDst.y;
	
	return true;
}
bool clsVideo::isPointInsideSection(pts2d point)
{
	float xLimMin,xLimMax,yLimMin,yLimMax;
	float x,y;
	
	x = point.x;
	y = point.y;
	
	if(point.cameraPosition == SIDE_CAMERA)
	{
		if(point.cameraSection == 0)
		{
			xLimMin = cvmGet(arenaSizes,2,0);
			xLimMax = cvmGet(arenaSizes,3,0);
			
			yLimMin = cvmGet(arenaSizes,2,1);
			yLimMax = cvmGet(arenaSizes,0,1);
			
		}else if(point.cameraSection == 1)
		{
			
			xLimMin = cvmGet(arenaSizes,4,0);
			xLimMax = cvmGet(arenaSizes,5,0);
			
			yLimMin = cvmGet(arenaSizes,6,1);
			yLimMax = cvmGet(arenaSizes,4,1);
		}else{
			return false;
		}
		
		if(x < xLimMin || x > xLimMax)
		{
			ERRMSG(("SIDE X point out of limits: s%d p%d xLim(%.2f,%.2f) yLim(%.2f,%.2f) point (%.2f,%.2f)",point.cameraSection,point.cameraPosition,xLimMin,xLimMax,yLimMin,yLimMax,x,y));
			return false;
		}
		if(y < yLimMin || y > yLimMax)
		{
			ERRMSG(("SIDE Y point out of limits: s%d p%d xLim(%.2f,%.2f) yLim(%.2f,%.2f) point (%.2f,%.2f)",point.cameraSection,point.cameraPosition,xLimMin,xLimMax,yLimMin,yLimMax,x,y));
			return false;
		}
		
	}else
	if(point.cameraPosition == TOP_CAMERA)
	{
		
		//xLimMin = cvmGet(arenaSizes,15,0);
		//xLimMax = cvmGet(arenaSizes,14,0);
		yLimMin = cvmGet(arenaSizes,12,1);
		yLimMax = cvmGet(arenaSizes,14,1);

		if(y < yLimMin || y > yLimMax)
		{
			ERRMSG(("TOP point out of limits: s%d p%d yLim(%.2f,%.2f) point (%.2f)",point.cameraSection,point.cameraPosition,yLimMin,yLimMax,y));
			return false;
		}
	}else
	{
		return false;
	}
	
	
	return true;
}

bool clsVideo::overlappingSections(pts2d point,pts3D last3Dpoint)
{
	//TOP VIEW:
	//SIDE VIEW:
	
	double middleX = cvmGet(arenaSizes,5,0);
	bool isInOverlapped = false;
	
	//INFOMSG(("middle point = %.2f",middleX));
	
	if(point.cameraPosition == SIDE_CAMERA)
	{
		if(point.x > middleX-(OVERLAP_SIZE_CM/2) && point.x < middleX+(OVERLAP_SIZE_CM/2))
		{
			isInOverlapped = true;
		}
		
	}
	
	if(point.cameraPosition == TOP_CAMERA)
	{
		if(point.x > middleX-(OVERLAP_SIZE_CM/2) && point.x < middleX+(OVERLAP_SIZE_CM/2))
		{
			isInOverlapped = true;
		}
		
	}
	
	//check if point is in overlapped part
	if(isInOverlapped)
	{
		if(last3Dpoint.sideSection!= point.cameraSection)
		{
			if(point.cameraPosition == TOP_CAMERA)
			{
				if(overlapXSideTimer.getElapsedTime() > OVERLAP_TIME_MS)
				{
					INFOMSG(("changing TOP from section %d to %d",last3Dpoint.sideSection,point.cameraSection));
					
					overlapXTopTimer.reset();
					overlapXTopTimer.start();
				}else
				{
					return false;
				}
			}
			if(point.cameraPosition == SIDE_CAMERA)
			{
				if(overlapXSideTimer.getElapsedTime() > OVERLAP_TIME_MS)
				{
					INFOMSG(("changing SIDE from section %d to %d",last3Dpoint.sideSection,point.cameraSection));
					
					overlapXSideTimer.reset();
					overlapXSideTimer.start();
				}else
				{
					return false;
				}
			}
		}
		
	}
	
	//check if its in the limits of the arena!
	int    xLimMax = cvmGet(arenaSizes,1,0);
	int    yLimMax = cvmGet(arenaSizes,0,1);
	int    zLimMax = cvmGet(arenaSizes,10,1);
	
	//INFOMSG(("limits: %d %d %d",xLimMax,yLimMax,zLimMax));
	
	if(point.cameraPosition == SIDE_CAMERA)
	{
		if(point.x < 0 || point.x > xLimMax)
		{
//			INFOMSG(("x is out of the arena %.2f",point.x));
			return false;
		}
		
		if(point.y < 0 || point.y > yLimMax)
		{
//			INFOMSG(("y is out of the arena %.2f",point.y));
			return false;
		}
		
	}
	
	if(point.cameraPosition == TOP_CAMERA)
	{
		if(point.y < 0 || point.y > zLimMax)
		{
//			INFOMSG(("z is out of the arena %.2f",point.y));
			return false;
		}
	}
	
	return true;
	//
	
	
	
	// float xLimMin,xLimMax,yLimMin,yLimMax;
	// float x,y;
	// 
	// x = point.x;
	// y = point.y;
	// 
	// if(point.cameraPosition == SIDE_CAMERA)
	// {
		// if(point.cameraSection == 0)
		// {
			// xLimMin = cvmGet(arenaSizes,2,0);
			// xLimMax = cvmGet(arenaSizes,3,0);
			// 
			// yLimMin = cvmGet(arenaSizes,2,1);
			// yLimMax = cvmGet(arenaSizes,0,1);
			// 
		// }else if(point.cameraSection == 1)
		// {
			// 
			// xLimMin = cvmGet(arenaSizes,4,0);
			// xLimMax = cvmGet(arenaSizes,5,0);
			// 
			// yLimMin = cvmGet(arenaSizes,6,1);
			// yLimMax = cvmGet(arenaSizes,4,1);
		// }else{
			// return false;
		// }
		// 
		// if(x < xLimMin || x > xLimMax)
		// {
			// ERRMSG(("SIDE X point out of limits: s%d p%d xLim(%.2f,%.2f) yLim(%.2f,%.2f) point (%.2f,%.2f)",point.cameraSection,point.cameraPosition,xLimMin,xLimMax,yLimMin,yLimMax,x,y));
			// return false;
		// }
		// if(y < yLimMin || y > yLimMax)
		// {
			// ERRMSG(("SIDE Y point out of limits: s%d p%d xLim(%.2f,%.2f) yLim(%.2f,%.2f) point (%.2f,%.2f)",point.cameraSection,point.cameraPosition,xLimMin,xLimMax,yLimMin,yLimMax,x,y));
			// return false;
		// }
		// 
	// }else
	// if(point.cameraPosition == TOP_CAMERA)
	// {
		// 
		// //xLimMin = cvmGet(arenaSizes,15,0);
		// //xLimMax = cvmGet(arenaSizes,14,0);
		// yLimMin = cvmGet(arenaSizes,12,1);
		// yLimMax = cvmGet(arenaSizes,14,1);
// 
		// if(y < yLimMin || y > yLimMax)
		// {
			// ERRMSG(("TOP point out of limits: s%d p%d yLim(%.2f,%.2f) point (%.2f)",point.cameraSection,point.cameraPosition,yLimMin,yLimMax,y));
			// return false;
		// }
	// }else
	// {
		// return false;
	// }
}
void clsVideo::calculatePerspective(pts2d pSrc, CvMat *perspectiveMatrices,CvPoint2D32f *pDst,int perspectiveLine)
{
	
	CvScalar p1;
        CvMat tsrc, tdst;
        CvMat* src = cvCreateMat( 2, 1,CV_32FC1);
        CvMat* dst = cvCreateMat( 2, 1, CV_32FC1 );
        
#ifndef DO_NOT_USE_INTRINSIC
	
        cvSet2D(src,0,0,cvScalar(pSrc.x));
        cvSet2D(src,1,0,cvScalar(pSrc.y));
        
        cvReshape( src, &tsrc, 2, 0 );
        cvReshape( dst, &tdst, 2, 0 );
        
#else
        
        cvmSet(src,0,0,pSrc.x);
        cvmSet(src,1,0,pSrc.y);
        
        cvReshape( src, &tsrc, 2, 0 );
        cvReshape( dst, &tdst, 2, 0 );
#endif
        
        CvMat *perspectiveMatrix = cvCreateMat(3, 3, CV_32FC1);
        
        cvmSet(perspectiveMatrix,0,0,cvmGet(perspectiveMatrices,perspectiveLine,0));
        cvmSet(perspectiveMatrix,0,1,cvmGet(perspectiveMatrices,perspectiveLine,1));
        cvmSet(perspectiveMatrix,0,2,cvmGet(perspectiveMatrices,perspectiveLine,2));
        cvmSet(perspectiveMatrix,1,0,cvmGet(perspectiveMatrices,perspectiveLine,3));
        cvmSet(perspectiveMatrix,1,1,cvmGet(perspectiveMatrices,perspectiveLine,4));
        cvmSet(perspectiveMatrix,1,2,cvmGet(perspectiveMatrices,perspectiveLine,5));
        cvmSet(perspectiveMatrix,2,0,cvmGet(perspectiveMatrices,perspectiveLine,6));
        cvmSet(perspectiveMatrix,2,1,cvmGet(perspectiveMatrices,perspectiveLine,7));
        cvmSet(perspectiveMatrix,2,2,cvmGet(perspectiveMatrices,perspectiveLine,8));
        
        // PrintMat(perspectiveMatrix);
        // PrintMat(perspectiveMatrices);
        
        cvPerspectiveTransform( &tsrc, &tdst, perspectiveMatrix );
        
        p1=cvGet2D(&tdst,0,0);
        pDst->x=p1.val[0];
        pDst->y=p1.val[1];
        
}
bool clsVideo::loadArenaInCms(int section,int position,CvPoint2D32f *arenaPointsInCms)
{
	
	CvMat *arenaSizeInCms = (CvMat*)cvLoad(PERSPECTIVE_MATRICES_CM);
	if(section == 0)
	{
		if(position == TOP_CAMERA)
		{
			arenaPointsInCms[0] = cvPoint2D32f(cvmGet(arenaSizeInCms,8,0),cvmGet(arenaSizeInCms,8,1));
			arenaPointsInCms[1] = cvPoint2D32f(cvmGet(arenaSizeInCms,9,0),cvmGet(arenaSizeInCms,9,1));
			arenaPointsInCms[2] = cvPoint2D32f(cvmGet(arenaSizeInCms,10,0),cvmGet(arenaSizeInCms,10,1));
			arenaPointsInCms[3] = cvPoint2D32f(cvmGet(arenaSizeInCms,11,0),cvmGet(arenaSizeInCms,11,1));
			return false;
		}else
			if(position == SIDE_CAMERA)
			{
				arenaPointsInCms[0] = cvPoint2D32f(cvmGet(arenaSizeInCms,0,0),cvmGet(arenaSizeInCms,0,1));
				arenaPointsInCms[1] = cvPoint2D32f(cvmGet(arenaSizeInCms,1,0),cvmGet(arenaSizeInCms,1,1));
				arenaPointsInCms[2] = cvPoint2D32f(cvmGet(arenaSizeInCms,2,0),cvmGet(arenaSizeInCms,2,1));
				arenaPointsInCms[3] = cvPoint2D32f(cvmGet(arenaSizeInCms,3,0),cvmGet(arenaSizeInCms,3,1));
			}else
			{
				// DEBUGMSG(("POINTS NOT FOUND??? %d %d\n",section,position));
				return false;
			}
	}
	
	
        if(section == 1)
        {
        	if(position == TOP_CAMERA)
		{
			arenaPointsInCms[0] = cvPoint2D32f(cvmGet(arenaSizeInCms,12,0),cvmGet(arenaSizeInCms,12,1));
			arenaPointsInCms[1] = cvPoint2D32f(cvmGet(arenaSizeInCms,13,0),cvmGet(arenaSizeInCms,13,1));
			arenaPointsInCms[2] = cvPoint2D32f(cvmGet(arenaSizeInCms,14,0),cvmGet(arenaSizeInCms,14,1));
			arenaPointsInCms[3] = cvPoint2D32f(cvmGet(arenaSizeInCms,15,0),cvmGet(arenaSizeInCms,15,1));
			return false;
		}
		if(position == SIDE_CAMERA)
		{
			arenaPointsInCms[0] = cvPoint2D32f(cvmGet(arenaSizeInCms,4,0),cvmGet(arenaSizeInCms,4,1));
			arenaPointsInCms[1] = cvPoint2D32f(cvmGet(arenaSizeInCms,5,0),cvmGet(arenaSizeInCms,5,1));
			arenaPointsInCms[2] = cvPoint2D32f(cvmGet(arenaSizeInCms,6,0),cvmGet(arenaSizeInCms,6,1));
			arenaPointsInCms[3] = cvPoint2D32f(cvmGet(arenaSizeInCms,7,0),cvmGet(arenaSizeInCms,7,1));
			return false;
		}
        }
        
        return true;
}
void clsVideo::startBGFGCodeBook()
{
	model = cvCreateBGCodeBookModel();
	
        //Set color thresholds to default values
        model->modMin[0] = 3;
        model->modMin[1] = model->modMin[2] = 3;
        model->modMax[0] = 10;
        model->modMax[1] = model->modMax[2] = 10;
        model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = 10;
        ImaskCodeBook = cvCreateImage( cvSize( frame->width, frame->height ), IPL_DEPTH_8U, 1 );
        ImaskCodeBookCC = cvCreateImage( cvSize( frame->width, frame->height ), IPL_DEPTH_8U, 1 );
        cvSet(ImaskCodeBook,cvScalar(255));
        
        DEBUGMSG(("BGFG model activated"));
}

void clsVideo::setRemoteCamera()
{
	remoteCamera = true;
}

void clsVideo::zoomHandler(IplImage *frame)
{
	if(zoomedOut)
	{
		INFOMSG(("ZOOMING OUT"));
		cvResetImageROI(frame);
		zoomedOut = false;
		zooming = false; 
	}
	if(zoomedIn)
	{
		
		INFOMSG(("ZOOMING IN"));
		int amountZoom = (refZoom*0.01)*frame->width;
		
		zoomsetX = clickedX-amountZoom;
		if(zoomsetX < 0)
			zoomsetX = 0;
		if(zoomsetX > frame->width)
			zoomsetX = frame->width;
		zoomwidth = zoomsetX+amountZoom;
		if(zoomwidth < 0)
			zoomwidth = 0;
		if(zoomwidth > frame->width)
			zoomwidth = frame->width;
		
		amountZoom = refZoom*frame->height;
		zoomsetY = clickedY-amountZoom;
		if(zoomsetY < 0)
			zoomsetY = 0;
		if(zoomsetY > frame->height)
			zoomsetY = frame->height;
		zoomheight = zoomsetY+amountZoom;
		if(zoomheight < 0)
			zoomheight = 0;
		if(zoomheight > frame->height)
			zoomheight = frame->height;
		
		zooming = true;
		zoomedIn = false;
	}
	
	if(zooming)
	{
		cvSetImageROI(frame, cvRect(zoomsetX,zoomsetY,zoomwidth,zoomheight));
		cvResize(frame,frame_buffer);
		cvResetImageROI(frame);
		cvCopy(frame_buffer,frame);
	}
}
//void clsVideo::zoomFrame(IplImage *frame)
//{
//	cvSetImageROI(img, cvRect(CAMREC_WIDTH,CAMREC_HEIGHT*0,CAMREC_WIDTH,CAMREC_HEIGHT));
//}

void pick_color_mouse( int event, int x, int y, int flags, void* param )
{
	switch( event ){
	case CV_EVENT_LBUTTONDOWN:
		// for(i=0;i<height;i++) for(j=0;j<width;j++) for(k=0;k<channels;k++)
		// data[i*step+j*channels+k]
		// IplImage *img = (IplImage *) param;
		// uchar *data      = (uchar *)img->imageData;
		// int step      = img->widthStep;
		// int channels  = img->nChannels;	
		// printf("clicked in %d %d :: %d %d :: %d %d %d\n",x,y,channels,step,data[y*step+x*channels+1],data[y*step+x*channels+2],data[x*step+y*channels+3]);
		break;
		
	case CV_EVENT_RBUTTONDOWN:
		INFOMSG(("RBUTTONDOWN"));
		break;
		
	case CV_EVENT_FLAG_CTRLKEY:
		INFOMSG(("FLAG_LBUTTONDBLCLK"));
		break;
	}
}

void PERSPECTIVE_MATRICES_mouseHandler(int event, int x, int y, int flags, void *param)
{
	
        switch(event) {
        case CV_EVENT_LBUTTONDOWN:
        	// printf("CV_EVENT_LBUTTONDOWN\n");	
        	clickedX = x;
		clickedY = y;
		// INFOMSG(("clicked %d %d",x,y));
        	if(PERSPECTIVE_MATRICES_nrSelectedPoints >= 4)
        	{ //ONLY DRAG POINTS!!
        		
        		if(PERSPECTIVE_MATRICES_nrSelectedPoints >= 4)
        		{ //ONLY DRAG POINTS!!
        			for(int i = 0; i <  4; i++){
        				if(x > PERSPECTIVE_MATRICES_pointsSelected[i].x + RADIUS_REC_PERSPECTIVE ||  x < PERSPECTIVE_MATRICES_pointsSelected[i].x - RADIUS_REC_PERSPECTIVE)
        				{
        					continue;
        				}
        				if(y > PERSPECTIVE_MATRICES_pointsSelected[i].y + RADIUS_REC_PERSPECTIVE ||  y < PERSPECTIVE_MATRICES_pointsSelected[i].y - RADIUS_REC_PERSPECTIVE)
        				{
        					continue;
        				}
        				dragBall = i;
        				
        			}
        		}
        		// for(int i = 0; i <  4; i++){
        			// double ax1,ax2;
        			// ax1 = (x-PERSPECTIVE_MATRICES_pointsSelected[i].x)^2;
        			// ax2 = (y-PERSPECTIVE_MATRICES_pointsSelected[i].y)^2;
        			// double t = abs(ax1+ax2);
        			// double rad = RADIUS_TO_DRAG; 
        			// if( t <= rad){
        				// dragBall = i;
        				// // printf("%d\n",dragBall);
        				// break;
        			// }
        		// }
        	}else
        	{
        		PERSPECTIVE_MATRICES_pointsSelected[PERSPECTIVE_MATRICES_nrSelectedPoints] = cvPoint(x,y);
        		PERSPECTIVE_MATRICES_nrSelectedPoints++;
        	}
        	
                break;
        case CV_EVENT_RBUTTONUP:
        	// printf("CV_EVENT_RBUTTONUP\n");
        	break;
        case CV_EVENT_RBUTTONDOWN:
                zoomedOut = true;
                break;
        case CV_EVENT_MBUTTONUP:
        	break;
        case CV_EVENT_MBUTTONDOWN:
        	clickedX = x;
		clickedY = y;
        	zoomedIn = true;
        	// printf("CV_EVENT_MBUTTONUP\n");
        	//cvSetImageROI(frame, cvRect(frame->width-50,CAMREC_HEIGHT,CAMREC_WIDTH,CAMREC_HEIGHT));
        	break;
        case CV_EVENT_LBUTTONUP:
        	// printf("CV_EVENT_LBUTTONUP\n");
        	clickedX = -1;
		clickedY = -1;
        	dragBall = -1;
                break;
        case CV_EVENT_MOUSEMOVE:
        	break;
        default:
        	dragBall = -1;
        	break;
        }
        if(dragBall > -1)
        {
                PERSPECTIVE_MATRICES_pointsSelected[dragBall] = cvPoint(x,y); 
        }
        
}

void trackbar_callback(int theSliderValue)
{
}


void perspectiveLinesMouseHandler(int event, int x, int y, int flags, void *param)
{
	
        switch(event) {
        case CV_EVENT_LBUTTONDOWN:	
        	clickedX = x;
		clickedY = y;
		
        	if(PERSPECTIVE_MATRICES_nrSelectedPoints >= 4)
        	{ //ONLY DRAG POINTS!!
        		for(int i = 0; i <  4; i++){
        			if(x > pointsPerspectiveLines[i].x + RADIUS_REC_PERSPECTIVE ||  x < pointsPerspectiveLines[i].x - RADIUS_REC_PERSPECTIVE)
        			{
        				continue;
        			}
        			if(y > pointsPerspectiveLines[i].y + RADIUS_REC_PERSPECTIVE ||  y < pointsPerspectiveLines[i].y - RADIUS_REC_PERSPECTIVE)
        			{
        				continue;
        			}
        			dragBall = i;

        		}
        	}
        	
                break;
        case CV_EVENT_RBUTTONUP:
        	// printf("CV_EVENT_RBUTTONUP\n");
        	break;
        case CV_EVENT_RBUTTONDOWN:
                zoomedOut = true;
                break;
        case CV_EVENT_MBUTTONUP:
        	break;
        case CV_EVENT_MBUTTONDOWN:
        	clickedX = x;
		clickedY = y;
        	zoomedIn = true;
        	// printf("CV_EVENT_MBUTTONUP\n");
        	//cvSetImageROI(frame, cvRect(frame->width-50,CAMREC_HEIGHT,CAMREC_WIDTH,CAMREC_HEIGHT));
        	break;
        case CV_EVENT_LBUTTONUP:
        	// printf("CV_EVENT_LBUTTONUP\n");
        	clickedX = -1;
		clickedY = -1;
        	dragBall = -1;
                break;
        case CV_EVENT_MOUSEMOVE:
        	break;
        default:
        	dragBall = -1;
        	break;
        }
        if(dragBall > -1)
        {
        	
        	if(dragBall < 2)
        	{
        		pointsPerspectiveLines[0] = cvPoint(pointsPerspectiveLines[0].x,y);
        		pointsPerspectiveLines[1] = cvPoint(pointsPerspectiveLines[1].x,y);
        	}else
        	{
        		pointsPerspectiveLines[2] = cvPoint(pointsPerspectiveLines[2].x,y);
        		pointsPerspectiveLines[3] = cvPoint(pointsPerspectiveLines[3].x,y);
        	}
                
                
        }
        
}

void pickXandY(int event, int x, int y,int flags, void* param)
{
	switch(event){
	case CV_EVENT_LBUTTONDOWN:
		clickedX = x;
		clickedY = y;
		break;
	case CV_EVENT_LBUTTONUP:
		clickedX = -1;
		clickedY = -1;
		
		break;
	
	default:
		break;
	}
}

void clsVideo::colorThreshold()
{
	//converting to HSV
	cvCvtColor(frame_smooth, frame_buffer, CV_BGR2HSV);
	
	//filtrating by color... good on this video!
	cvInRangeS(frame_buffer, cvScalar(minH, minS, minL), cvScalar(maxH, maxS, maxL), frame_threshold);
	
}
void clsVideo::framesubtraction()
{
	if(!pause)
	{
		cvSub(frame_smooth,frame_buffer2,frame_sub);
	}
	cvCvtColor( frame_sub, frame_threshold, CV_RGB2GRAY );
	cvThreshold(frame_threshold,frame_threshold,minThrs,maxThrs,CV_THRESH_BINARY);
	
	if(!pause)
	{
		cvCopy(frame_buffer2,frame_buffer);
		cvCopy(frame_smooth,frame_buffer2);
	}
}

void clsVideo::resetBGFG()
{
	nframes = 0;
}
void clsVideo::bgfgCodeBook()
{
	
	cvCvtColor( frame_smooth, frame_bufferbgfg, CV_BGR2YCrCb );//YUV For codebook method
	// cvInRangeS(frame_buffer, cvScalar(minH, minS, minL), cvScalar(maxH, maxS, maxL), frame_threshold);
	//This is where we build our background model
	try
	{
		
		if( !pause && nframes-1 < cfgnframes  )
		{
			cvBGCodeBookUpdate( model, frame_bufferbgfg );
		}
		
		
		if( nframes-1 == cfgnframes )
		{
			cvBGCodeBookClearStale( model, model->t/2 );
			pfps.resetCount();
			INFOMSG(("finished adding frames"));
		}
		
		//Find the foreground if any
		if( nframes-1 >= cfgnframes  )
		{
			
			//CV_MAT_TYPE(image->type) == CV_8UC3 && image->cols == model->size.width && image->rows == model->size.height && CV_IS_MASK_ARR(mask) && CV_ARE_SIZES_EQ(image, mask)
			
			// Find foreground by codebook method
			cvBGCodeBookDiff( model, frame_bufferbgfg, ImaskCodeBook );
			
			// This part just to visualize bounding boxes and centers if desired
			
			cvCopy(ImaskCodeBook,ImaskCodeBookCC);
			
			cvSegmentFGMask( ImaskCodeBookCC );
			
			readyTo2Dtrack = true;
			
		}
		
		nframes++;
	}catch(Exception e)
	{
		ERRMSG(("EXCEPTION:: trying to restart the BGFGCodeBook"));
		startBGFGCodeBook();
		
		nframes = 0;
		
	}
	cvCopy(ImaskCodeBook,frame_threshold);
}

void clsVideo::findParticle(IplImage *frame_countors, pts2d *point)
{
	//calculate blobs  
	storage = cvCreateMemStorage(0);
	cvFindContours( frame_countors, storage, &contour, sizeof(CvContour),
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );
	
	CvPoint pt1_rect, pt2_rect;
	
	if(contour != NULL && readyTo2Dtrack)
	{
		contourLow=cvApproxPoly(contour, sizeof(CvContour), storage,CV_POLY_APPROX_DP,1,1);
		
		//For each contour found
		CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
		
		double biggest_area = 0.1;
		
		CvRect rect;
		
		for( ; contourLow != 0; contourLow = contourLow->h_next )
		{
			//Or detect bounding rect of contour
			
			CvSeq *cesq=contourLow;
			cvContourMoments(cesq,moments);
			
			double moment10 = cvGetSpatialMoment(moments, 1, 0);
			double moment01 = cvGetSpatialMoment(moments, 0, 1);
			double area = cvGetCentralMoment(moments, 0, 0);
			
			int posX = moment10/area;
			int posY = moment01/area;
			
			if(area > biggest_area && area > minAreaToTrack )
			{
				point->x = posX;
				point->y = posY;
				point->xPx = posX;
				point->yPx = posY;
				point->timestamp = getTimestamp();
				point->fps = fps;
				
				
				rect=cvBoundingRect(contourLow, 0);
				
				pt1_rect.x = rect.x;
				pt2_rect.x = (rect.x+rect.width);
				pt1_rect.y = rect.y;
				pt2_rect.y = (rect.y+rect.height);
			}
		}
		
		delete moments;
		cvReleaseMemStorage(&storage);
		
	}
	
	particle_curr->set_new_coords(point->x,point->y);
	particle_curr->surroundRect1 = pt1_rect;
	particle_curr->surroundRect2 = pt2_rect;
}
bool clsVideo::findMotion(IplImage *frame_countors)
{
	bool iRet = false;
	//calculate blobs  
	storage = cvCreateMemStorage(0);
	cvFindContours( frame_countors, storage, &contour, sizeof(CvContour),
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );
	
	if(contour != NULL )
	{
		contourLow=cvApproxPoly(contour, sizeof(CvContour), storage,CV_POLY_APPROX_DP,1,1);
		
		//For each contour found
		CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
		
		for( ; contourLow != 0; contourLow = contourLow->h_next )
		{
			//Or detect bounding rect of contour
			
			CvSeq *cesq=contourLow;
			cvContourMoments(cesq,moments);
			
			double area = cvGetCentralMoment(moments, 0, 0);
			
			if(area > 5 )
			{
				iRet = true;
				break;
			}
		}
		
		delete moments;
		
		
	}
	cvClearMemStorage(storage);
	cvReleaseMemStorage(&storage);
	return iRet;
}
void clsVideo::findParticle2(IplImage *frame_countors, pts2d *point)
{
	//calculate blobs  
	storage = cvCreateMemStorage(0);
	cvFindContours( frame_countors, storage, &contour, sizeof(CvContour),
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );
	
	if(contour != NULL && readyTo2Dtrack)
	{
		contourLow=cvApproxPoly(contour, sizeof(CvContour), storage,CV_POLY_APPROX_DP,1,1);
		
		//For each contour found
		CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
		
		particles.clear();
		
		for( ; contourLow != 0; contourLow = contourLow->h_next )
		{
			//Or detect bounding rect of contour
			CvRect rect; 
			rect=cvBoundingRect(contourLow, 0);
			
			CvPoint pt1, pt2;
			pt1.x = rect.x;
			pt2.x = (rect.x+rect.width);
			pt1.y = rect.y;
			pt2.y = (rect.y+rect.height);
			
			cvRectangle(frame_output, pt1,pt2, CV_RGB(255,0,0), 1, 40, 0); //the rectangle is drawn on the Colored Image //DP
			
			CvSeq *cesq=contourLow;
			cvContourMoments(cesq,moments);
			
			double moment10 = cvGetSpatialMoment(moments, 1, 0);
			double moment01 = cvGetSpatialMoment(moments, 0, 1);
			double area = cvGetCentralMoment(moments, 0, 0);
			
			int posX = moment10/area;
			int posY = moment01/area;
			
			if(posX > 0 && posY > 0){
				//add to a vector of particles
				particles.push_back(new clsParticle(posX,posY,area));
			}
			
			
		}
		
		
		//find the one that is closer to the older target...
		
		#ifdef FIND_THE_CLOSEST_PARTICLE_STRATEGY
		float smallest_dist = 1000;
		int t = 0, particle_smallest_dist_id = 0;
		float posX, posY;
		
		while(particles.size() > t)
		{
			posX = particles[t]->x;
			posY = particles[t]->y;
			
			particle_curr->calc_distance(posX,posY);
			
			if(particle_curr->current_distance < smallest_dist)
			{
				particle_smallest_dist_id = t;
				smallest_dist = particle_curr->current_distance;
			}
			
			t++;
		}
		point->x = particles[particle_smallest_dist_id]->x;
		point->y = particles[particle_smallest_dist_id]->y;
		point->timestamp = getTimestamp();
		#endif
		#ifdef FIND_THE_BIGGEST_PARTICLE_STRATEGY
		double biggest_area = 0.1;
		int t = 0, particle_biggest_id = 0;
		
		while((int)particles.size() > t)
		{
			
			if(particles[t]->area > biggest_area)
			{
				particle_biggest_id = t;
				biggest_area = particles[t]->area;
			}
			
			t++;
			
			
		}
		
		if(biggest_area > 1 )
		{
			point->x = particles[particle_biggest_id]->x;
			point->y = particles[particle_biggest_id]->y;
			point->timestamp = getTimestamp();
		}
		#endif
		//update and draw tail!
		
		delete moments;
		cvReleaseMemStorage(&storage);
		
	}
	
	particle_curr->set_new_coords(point->x,point->y);
}

bool clsVideo::startMotionDetection()
{
	frame = cvQueryFrame(capture);
	if(frame == NULL)
	{
		ERRMSG(("failed to start video...\n"));
		return false;
	}
	frame_buffer = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_buffer2 = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_smooth = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels);
        frame_threshold = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, 1);
        frame_sub =  cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_output = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_to_send = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, frame->nChannels );
        frame_countors = cvCreateImage( cvSize( frame->width, frame->height ),
        	frame->depth, 1);
        
        cvNamedWindow(WINDOW_NAME);
        minThrs = 50;
        cvCreateTrackbar("min", WINDOW_NAME, &minThrs, 255, trackbar_callback);
        
        camSpecs();
        
	return true;
}
bool clsVideo::thresholdCam()
{
	frame_output = cvQueryFrame(capture);
	if(!frame_output) {
		INFOMSG(("end of file..."));
		init_video(myFilename);
		return true;
	}
	
	cvSmooth(frame_output, frame_smooth, CV_BLUR, sqr_blur+1, sqr_blur+1, 0, 0);
	
	framesubtraction();
	
	//removing loose points
	cvErode(frame_threshold, frame_threshold, NULL,1);
	
	//augmenting neighbour points
	cvDilate(frame_threshold, frame_threshold, NULL,1);
	
	cvCopy(frame_threshold,frame_countors);
	cvCopy(frame_output,frame_to_send);
	
	cvShowImage(WINDOW_NAME,frame_output);
	
	char key = (char)cvWaitKey(1);
	switch (key) {
	case 'q':
	case 'Q':
	case 27: //escape key
		return false;
	default:
		break;
	}
	return true;
}
bool clsVideo::motionDetection()
{
	if(findMotion(frame_countors))
	{
		return true;
	}
	return false;
}


