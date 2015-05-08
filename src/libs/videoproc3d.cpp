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
#include "videoproc3d.h"

#include <string>
#include <algorithm>
#include <stdio.h>


vector<CvPoint> points3Dboard;
int dragMarker = -1;
void select3DboardHandler(int event, int x, int y, int flags, void *param);

clsVideo3D::clsVideo3D()
{
	markerRectColor = CV_RGB(0, 0, 255);
	
	// pVideoAR = new clsVideoAR();
}

clsVideo3D::~clsVideo3D()
{
	
}

int clsVideo3D::startRemoteCam3D(IplImage *remoteFrame)
{
	frameCalibration = cvCreateImage( cvSize( remoteFrame->width*2, remoteFrame->height ),
        	remoteFrame->depth, remoteFrame->nChannels );
	frameSection[0] = cvCreateImage( cvSize( remoteFrame->width, remoteFrame->height ),
        	remoteFrame->depth, remoteFrame->nChannels );
	frameSection[1] = cvCreateImage( cvSize( remoteFrame->width, remoteFrame->height ),
        	remoteFrame->depth, remoteFrame->nChannels );
        
        frameCalibrationUndistort = cvCreateImage( cvSize( remoteFrame->width*2, remoteFrame->height ),
        	remoteFrame->depth, remoteFrame->nChannels );
        mapx[0] = cvCreateImage( cvGetSize(remoteFrame), IPL_DEPTH_32F, 1 );
        mapy[0] = cvCreateImage( cvGetSize(remoteFrame), IPL_DEPTH_32F, 1 );
        mapx[1] = cvCreateImage( cvGetSize(remoteFrame), IPL_DEPTH_32F, 1 );
        mapy[1] = cvCreateImage( cvGetSize(remoteFrame), IPL_DEPTH_32F, 1 );
        
        imageSize =  cvGetSize(remoteFrame);

	detectChessboardToContinue = false;
	return 0;
}

int clsVideo3D::setRemoteFrames(IplImage *img0, IplImage *img1)
{
	cvCopy(img0,frameSection[0]);
	cvCopy(img1,frameSection[1]);
	
	cvSetImageROI( frameCalibration, cvRect( 0, 0, frameSection[0]->width, frameSection[0]->height ) );
        cvCopy(frameSection[0], frameCalibration);
        cvResetImageROI(frameCalibration);
        cvSetImageROI( frameCalibration, cvRect(frameSection[1]->width,0, frameSection[1]->width, frameSection[1]->height) );
        cvCopy(frameSection[1], frameCalibration);
        cvResetImageROI(frameCalibration);
        
        // Undistort image
        if(false)
        {
        	IplImage *imge;
        	
        	cvSetImageROI( frameCalibrationUndistort, cvRect( 0, 0, frameSection[0]->width, frameSection[0]->height ) );
        	imge = cvCloneImage(frameSection[0]);
        	cvRemap( imge, frameSection[0], mapx[0], mapy[0] );
        	cvCopy(imge, frameCalibrationUndistort);
        	
        	cvResetImageROI(frameCalibrationUndistort);
        	cvSetImageROI( frameCalibrationUndistort, cvRect(frameSection[1]->width,0, frameSection[1]->width, frameSection[1]->height) );
        	imge = cvCloneImage(frameSection[1]);
        	cvRemap( imge, frameSection[1], mapx[1], mapy[1] );
        	cvCopy(imge, frameCalibrationUndistort);
        	
        	cvResetImageROI(frameCalibrationUndistort);
        }

        return 0;
}


// 0 if still calibrating
// 1 to finish calibration
int clsVideo3D::calibrateCam3D()
{
	
	// if(fast)
	// 	key = (char) cvWaitKey(10);
	// else
	// 	key = (char) cvWaitKey(0);
	
	cvShowImage(WINDOW_NAME, frameCalibration);
	
	char key = (char) cvWaitKey(FRAME_SLEEP);
	switch (key) {
	case ' ':
		detectChessboardToContinue = true;
		break;
	case 'f':
		// fast = !fast;
		break;
	case 27:
	case 'q':
		return 1;
		break;
	default:
		break;
	}
	
	if(detectChessboardToContinue)
	{
		int dchess = detectChessboard();
		if(dchess == 0)
		{
			char nameImage[128];
			sprintf(nameImage,"image%d_0.jpg",chessboardDetectedCounter-1);
			cvSaveImage(nameImage,frameSection[0]);
			sprintf(nameImage,"image%d_1.jpg",chessboardDetectedCounter-1);
			cvSaveImage(nameImage,frameSection[1]);
			
			detectChessboardToContinue = false;
		}
		if(dchess == 2)
		{
			return 1;
		}
	}
	
	return 0;
}


int clsVideo3D::calibrateImages3D()
{
	
	// cvShowImage("img0", frameSection[0]);
	// cvShowImage("img1", frameSection[1]);
	// select3Dboard();
	
	vector<CvPoint2D32f> temp(CHESSBOARD_N);
	vector<CvPoint2D32f> pointsDetected[2];
	vector<string> imageNames[2];
	
	int count = 0, result=0;
	IplImage *framePainted[2];
	
	char buf[1024];
	char key;
	FILE* f = fopen("list.txt", "rt");
	if( !f )
	{
		ERRMSG(( "can not open file"));
		return 0;
	}
	
	bool continuar = true;
	bool chessboardFound[2];
	
	bool fast = true;
	
	do
	{
		
		for(int j=0;j<2;j++)
		{
			chessboardFound[j] = false;
			if( !fgets( buf, sizeof(buf)-3, f ))
			{
				continuar = false;
				break;
			}
			size_t len = strlen(buf);
			while( len > 0 && isspace(buf[len-1]))
				buf[--len] = '\0';
			if( buf[0] == '#')
			{
				continuar = false;
				break;
			}
			
			frameSection[j] = cvLoadImage( buf, CV_LOAD_IMAGE_COLOR );
			if( !frameSection[j] )
			{
				continuar = false;
				break;
			}
			
			imageNames[j].push_back(buf);
			
			INFOMSG(("Load image %s on section %d [sampleid=%d] - %dx%d c=%d",buf,j,chessboardDetectedCounter,frameSection[j]->width,frameSection[j]->height,frameSection[j]->nChannels));
			IplImage *img = cvCloneImage(frameSection[j]);
			result = cvFindChessboardCorners( img, cvSize(CHESSBOARD_X, CHESSBOARD_Y),
				&temp[0], &count,
				CV_CALIB_CB_ADAPTIVE_THRESH |
				CV_CALIB_CB_NORMALIZE_IMAGE);
			
			cvReleaseImage(&img);
			
			if(result)
			{
				chessboardFound[j] = true;
				
				INFOMSG(("Chessboard found"));
				INFOMSG(("temp size: %d",temp.size()));
				
				framePainted[j] = cvCloneImage( frameSection[j]);
				cvDrawChessboardCorners( framePainted[j], cvSize(CHESSBOARD_X, CHESSBOARD_Y), &temp[0],
					count, result );
				cvShowImage( "corners", framePainted[j] );
				cvReleaseImage( &framePainted[j] );
				
				IplImage *grayImage = cvCreateImage( cvGetSize( frameSection[j] ), 8, 1 );
				cvCvtColor( frameSection[j], grayImage, CV_BGR2GRAY );
				
				cvFindCornerSubPix(grayImage, &temp[0], count,
					cvSize(CHESSBOARD_X, CHESSBOARD_Y), cvSize(-1,-1),
					cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 )
					);
				
				cvReleaseImage(&grayImage);
				
				vector<CvPoint2D32f>& pts = pointsDetected[j];
				int totalPoints = pts.size();
				pts.resize(totalPoints + CHESSBOARD_N, cvPoint2D32f(0,0));
				copy( temp.begin(), temp.end(), pts.begin() + totalPoints );
				
			}else
			{
				INFOMSG(("Chessboard NOT found"));
				return 0;
			}
			
			cvReleaseImage(&frameSection[j]);
			
			if(fast)
				key = (char) cvWaitKey(10);
			else
				key = (char) cvWaitKey(0);

			
			switch (key) {
			case ' ':
				break;
			case 'f':
				fast = !fast;
				break;
			case 27:
			case 'q':
				return 1;
				break;
			default:
				break;
			}
			
		}
		if(chessboardFound[0] && chessboardFound[1])
			chessboardDetectedCounter++;
		
	}while(continuar);
	
	
	//CV STEREO CALIBRATION
	INFOMSG(("Loaded %d samples... pontos: %d %d",chessboardDetectedCounter,pointsDetected[0].size(),pointsDetected[1].size()));
	
	fclose(f);
	
	camerasPosition = 1;
	INFOMSG(("imageNames[0] sizeof =%d ",imageNames[0].size()));
	frameSection[0] = cvLoadImage( (const char*)imageNames[0][0].c_str(), CV_LOAD_IMAGE_COLOR );
	frameSection[1] = cvLoadImage( (const char*)imageNames[1][0].c_str(), CV_LOAD_IMAGE_COLOR );
	
	vector<CvPoint3D32f> objectPoints;
	vector<int> npoints;
	double M1[3][3], M2[3][3], D1[5], D2[5];
	double R[3][3], T[3], E[3][3], F[3][3];
	double Q[3][3];
	CvMat _M1 = cvMat(3, 3, CV_64F, M1 );
	CvMat _M2 = cvMat(3, 3, CV_64F, M2 );
	CvMat _D1 = cvMat(1, 5, CV_64F, D1 );
	CvMat _D2 = cvMat(1, 5, CV_64F, D2 );
	CvMat _R = cvMat(3, 3, CV_64F, R );
	CvMat _T = cvMat(3, 1, CV_64F, T );
	CvMat _E = cvMat(3, 3, CV_64F, E );
	CvMat _F = cvMat(3, 3, CV_64F, F );
	CvMat _Q = cvMat(4, 4, CV_64F, Q );
	
	CvSize imageSize = {0,0};
	int i,j;
	
	objectPoints.resize(chessboardDetectedCounter*CHESSBOARD_N);
	for( i = 0; i < CHESSBOARD_X; i++ )
		for( j = 0; j < CHESSBOARD_X; j++ )
			objectPoints[i*CHESSBOARD_X + j] = cvPoint3D32f(i*CHESSBOARD_SQ_SIZE, j*CHESSBOARD_SQ_SIZE, 0);
	for( i = 1; i < chessboardDetectedCounter; i++ )
		copy( objectPoints.begin(), objectPoints.begin() + CHESSBOARD_N,objectPoints.begin() + i*CHESSBOARD_N );
	
	npoints.resize(chessboardDetectedCounter,CHESSBOARD_N);
	imageSize = cvGetSize(frameSection[0] );
	// finishCalibrateCam3D();
	int N = chessboardDetectedCounter*CHESSBOARD_N;
	CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
	CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &pointsDetected[0][0] );
	CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &pointsDetected[1][0] );
	CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
	
	cvSetIdentity(&_M1);
	cvSetIdentity(&_M2);
	cvZero(&_D1);
	cvZero(&_D2);
	cvStereoCalibrate( &_objectPoints, &_imagePoints1,
		&_imagePoints2, &_npoints,
		&_M1, &_D1, &_M2, &_D2,
		imageSize, &_R, &_T, &_E, &_F,
		cvTermCriteria(CV_TERMCRIT_ITER+
			CV_TERMCRIT_EPS, 100, 1e-5),
		CV_CALIB_FIX_ASPECT_RATIO +
		CV_CALIB_ZERO_TANGENT_DIST +
		CV_CALIB_SAME_FOCAL_LENGTH );
	
	
	// CALIBRATION QUALITY CHECK
	// because the output fundamental matrix implicitly
	// includes all the output information,
	// we can check the quality of calibration using the
	// epipolar geometry constraint: m2^t*F*m1=0
	vector<CvPoint3D32f> lines[2];
	pointsDetected[0].resize(CHESSBOARD_N);
	pointsDetected[1].resize(CHESSBOARD_N);
	_imagePoints1 = cvMat(1, N, CV_32FC2, &pointsDetected[0][0] );
	_imagePoints2 = cvMat(1, N, CV_32FC2, &pointsDetected[1][0] );
	lines[0].resize(N);
	lines[1].resize(N);
	CvMat _L1 = cvMat(1, N, CV_32FC3, &lines[0][0]);
	CvMat _L2 = cvMat(1, N, CV_32FC3, &lines[1][0]);
	//Always work in undistorted space
	cvUndistortPoints( &_imagePoints1, &_imagePoints1,
		&_M1, &_D1, 0, &_M1 );
	cvUndistortPoints( &_imagePoints2, &_imagePoints2,
		&_M2, &_D2, 0, &_M2 );
	cvComputeCorrespondEpilines( &_imagePoints1, 1, &_F, &_L1 );
	cvComputeCorrespondEpilines( &_imagePoints2, 2, &_F, &_L2 );
	double avgErr = 0;
	for( i = 0; i < N; i++ )
	{
		double err = fabs(pointsDetected[0][i].x*lines[1][i].x +
			pointsDetected[0][i].y*lines[1][i].y + lines[1][i].z)
		+ fabs(pointsDetected[1][i].x*lines[0][i].x +
			pointsDetected[1][i].y*lines[0][i].y + lines[0][i].z);
		avgErr += err;
	}
	printf( "avg err = %g\n", avgErr/(chessboardDetectedCounter*CHESSBOARD_N) );
	
	
	
	
	
	
	
	//COMPUTE AND DISPLAY RECTIFICATION
	bool showUndistorted= true;
	int useUncalibrated = 2;
	bool isVerticalStereo;
	
	if( showUndistorted )
	{
		CvMat* mx1 = cvCreateMat( imageSize.height,
			imageSize.width, CV_32F );
		CvMat* my1 = cvCreateMat( imageSize.height,
			imageSize.width, CV_32F );
		CvMat* mx2 = cvCreateMat( imageSize.height,
			imageSize.width, CV_32F );
		CvMat* my2 = cvCreateMat( imageSize.height,
			imageSize.width, CV_32F );
		CvMat* img1r = cvCreateMat( imageSize.height,
			imageSize.width, CV_8U );
		CvMat* img2r = cvCreateMat( imageSize.height,
			imageSize.width, CV_8U );
		CvMat* disp = cvCreateMat( imageSize.height,
			imageSize.width, CV_16S );
		CvMat* vdisp = cvCreateMat( imageSize.height,
			imageSize.width, CV_8U );
		CvMat* pair;
		double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
		CvMat _R1 = cvMat(3, 3, CV_64F, R1);
		CvMat _R2 = cvMat(3, 3, CV_64F, R2);
		CvMat _disp = cvMat( imageSize.height,
			imageSize.width, CV_16S, disp);
		CvMat _P1 = cvMat(3, 4, CV_64F, P1);
		CvMat _P2 = cvMat(3, 4, CV_64F, P2);
		// IF BY CALIBRATED (BOUGUET'S METHOD)
		// if( useUncalibrated == 0 )
		// {
		cvZero(&_P1);
		cvZero(&_P2);
		
		cvStereoRectify( &_M1, &_M2, &_D1, &_D2, imageSize,
			&_R, &_T,
			&_R1, &_R2, &_P1, &_P2, &_Q,
			0/*CV_CALIB_ZERO_DISPARITY*/ );
		INFOMSG(("Q"));
		PrintMat(&_Q);
		
		INFOMSG(("P1::"));
		PrintMat(&_P1);
		
		INFOMSG(("P2"));
		PrintMat(&_P2);
		
		INFOMSG(("R"));
		PrintMat(&_R);
		
		INFOMSG(("T"));
		PrintMat(&_T);
		
		isVerticalStereo = fabs(P2[1][3]) > fabs(P2[0][3]);
		if( useUncalibrated == 0 )
		{
			//Precompute maps for cvRemap()
			cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_P1,mx1,my1);
			cvInitUndistortRectifyMap(&_M2,&_D2,&_R2,&_P2,mx2,my2);
		}
		//OR ELSE HARTLEY'S METHOD
		else if( useUncalibrated == 1 || useUncalibrated == 2 )
			// use intrinsic parameters of each camera, but
		// compute the rectification transformation directly
		// from the fundamental matrix
		{
			double H1[3][3], H2[3][3], iM[3][3];
			CvMat _H1 = cvMat(3, 3, CV_64F, H1);
			CvMat _H2 = cvMat(3, 3, CV_64F, H2);
			CvMat _iM = cvMat(3, 3, CV_64F, iM);
			//Just to show you could have independently used F
			if( useUncalibrated == 2 )
			cvFindFundamentalMat( &_imagePoints1,
				&_imagePoints2, &_F);
			cvStereoRectifyUncalibrated( &_imagePoints1,
				&_imagePoints2, &_F,
				imageSize,
				&_H1, &_H2, 3);
			cvInvert(&_M1, &_iM);
			cvMatMul(&_H1, &_M1, &_R1);
			cvMatMul(&_iM, &_R1, &_R1);
			cvInvert(&_M2, &_iM);
			cvMatMul(&_H2, &_M2, &_R2);
			cvMatMul(&_iM, &_R2, &_R2);
			//Precompute map for cvRemap()
			
			cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_M1,mx1,my1);
			cvInitUndistortRectifyMap(&_M2,&_D1,&_R2,&_M2,mx2,my2);
		}
		else
			assert(0);
		
		INFOMSG(("image undistorted..."));
		cvNamedWindow( "rectified", 1 );
		// RECTIFY THE IMAGES AND FIND DISPARITY MAPS
		if( !isVerticalStereo )
		pair = cvCreateMat( imageSize.height, imageSize.width*2,
			CV_8UC3 );
		else
		pair = cvCreateMat( imageSize.height*2, imageSize.width,
			CV_8UC3 );
		//Setup for finding stereo correspondences
		BMState = cvCreateStereoBMState();
		assert(BMState != 0);
		BMState->preFilterSize=41;
		BMState->preFilterCap=31;
		BMState->SADWindowSize=41;
		BMState->minDisparity=-64;
		BMState->numberOfDisparities=128;
		BMState->textureThreshold=10;
		BMState->uniquenessRatio=15;
		
		BMState->preFilterSize=9;
		BMState->preFilterCap=32;
		BMState->SADWindowSize=9;
		BMState->minDisparity=0;
		BMState->numberOfDisparities=32;
		BMState->textureThreshold=0;
		BMState->uniquenessRatio=0;
		
		initRectify = true;
		int k = 0;
		if(initRectify)
		{
			cvNamedWindow("disparity");
			cvCreateTrackbar("preFilterSize", "disparity", &BMState->preFilterSize, 250, trackbar_callback);
			cvCreateTrackbar("preFilterCap        ", "disparity", &BMState->preFilterCap        , 63, trackbar_callback);
			cvCreateTrackbar("SADWindowSize       ", "disparity", &BMState->SADWindowSize       , 255, trackbar_callback);
			cvCreateTrackbar("minDisparity        ", "disparity", &BMState->minDisparity        , 255, trackbar_callback);
			// cvCreateTrackbar("numberOfDisparities ", "disparity", &BMState->numberOfDisparities , 255, trackbar_callback);
			cvCreateTrackbar("textureThreshold    ", "disparity", &BMState->textureThreshold    , 255, trackbar_callback);
			cvCreateTrackbar("uniquenessRatio     ", "disparity", &BMState->uniquenessRatio     , 255, trackbar_callback);
			initRectify = false;
		}
		
		for( i = 0; i <  chessboardDetectedCounter ; i++ )
		// for( i = 0; ;)
		{
			IplImage* img1=cvLoadImage(imageNames[0][i].c_str(),0);
			IplImage* img2=cvLoadImage(imageNames[1][i].c_str(),0);
			if( img1 && img2 )
			{
				CvMat part;
				cvRemap( img1, img1r, mx1, my1 );
				cvRemap( img2, img2r, mx2, my2 );
				if( !isVerticalStereo || useUncalibrated != 0 )
				{
					// When the stereo camera is oriented vertically,
					// useUncalibrated==0 does not transpose the
					// image, so the epipolar lines in the rectified
					// images are vertical. Stereo correspondence
					// function does not support such a case.
					cvFindStereoCorrespondenceBM( img1r, img2r, disp,
						BMState);
					cvNormalize( disp, vdisp, 0, 256, CV_MINMAX );
					cvShowImage( "disparity", vdisp );
					
					// void cvFindStereoCorrespondenceGC(const CvArr* left, const CvArr* right, CvArr* dispLeft, CvArr* dispRight, CvStereoGCState* state, int useDisparityGuess = CV_DEFAULT(0))¶
					// double minVal; 
					// double maxVal; 
					// Point minLoc; 
					// Point maxLoc;
					
					// cvMinMaxLoc( disp, &minVal, &maxVal );
					
					// min(disp, &min, &max);
					// INFOMSG(("curiosity: %.2f %.2f",minVal,maxVal));
				}
				if( !isVerticalStereo )
				{
					cvGetCols( pair, &part, 0, imageSize.width );
					cvCvtColor( img1r, &part, CV_GRAY2BGR );
					cvGetCols( pair, &part, imageSize.width,
						imageSize.width*2 );
					cvCvtColor( img2r, &part, CV_GRAY2BGR );
					for( j = 0; j < imageSize.height; j += 16 )
					cvLine( pair, cvPoint(0,j),
						cvPoint(imageSize.width*2,j),
						CV_RGB(0,255,0));
				}
				else
				{
					cvGetRows( pair, &part, 0, imageSize.height );
					cvCvtColor( img1r, &part, CV_GRAY2BGR );
					cvGetRows( pair, &part, imageSize.height,
						imageSize.height*2 );
					cvCvtColor( img2r, &part, CV_GRAY2BGR );
					for( j = 0; j < imageSize.width; j += 16 )
					cvLine( pair, cvPoint(j,0),
						cvPoint(j,imageSize.height*2),
						CV_RGB(0,255,0));
				}
				cvShowImage( "rectified", pair );
				if( cvWaitKey(FRAME_SLEEP) == 27 )
				{
					break;
				}
				if(BMState->preFilterSize % 2 == 0)
				{
					BMState->preFilterSize++;
				}
				if(BMState->preFilterSize < 5)
				{
					BMState->preFilterSize  = 5;
				}
				if(BMState->preFilterCap == 0)
					BMState->preFilterCap++;
				if(BMState->SADWindowSize % 2 == 0)
				{
					BMState->SADWindowSize++;
				}
				if(BMState->SADWindowSize < 5)
				{
					BMState->SADWindowSize  = 5;
				}
				
			}
			
			//calculate error:
			// _Q
			vector<CvPoint2D32f> temp1(CHESSBOARD_N);
			vector<CvPoint2D32f> temp2(CHESSBOARD_N);
			
			cvFindChessboardCorners( img1, cvSize(CHESSBOARD_X, CHESSBOARD_Y),
				&temp1[0], &count,
				CV_CALIB_CB_ADAPTIVE_THRESH |
				CV_CALIB_CB_NORMALIZE_IMAGE);
			cvFindChessboardCorners( img2, cvSize(CHESSBOARD_X, CHESSBOARD_Y),
				&temp2[0], &count,
				CV_CALIB_CB_ADAPTIVE_THRESH |
				CV_CALIB_CB_NORMALIZE_IMAGE);
			
			calculateChessboard3DError(temp1, temp2, &_P1,&_P2, &_Q);
			cvReleaseImage( &img1 );
			cvReleaseImage( &img2 );
			
			// cvWaitKey(0);
		}
		cvReleaseStereoBMState(&BMState);
		cvReleaseMat( &mx1 );
		cvReleaseMat( &my1 );
		cvReleaseMat( &mx2 );
		cvReleaseMat( &my2 );
		cvReleaseMat( &img1r );
		cvReleaseMat( &img2r );
		cvReleaseMat( &disp );
		
		
	}
	
	return 0;
}

void clsVideo3D::calculateChessboard3DError(vector<CvPoint2D32f> temp1,vector<CvPoint2D32f> temp2, CvMat *P1, CvMat *P2, CvMat *Q)
{
	CvPoint3D32f *tmppoint = new CvPoint3D32f();
	vector<CvPoint3D32f> points3dBouguets;
	vector<CvPoint3D32f> points3dHartleys;
	
	for(int i=0;i<CHESSBOARD_N;i++)
	{
		triangulationHartleys(temp1[i],temp2[i],P1, P2, tmppoint);
		points3dHartleys.push_back(*tmppoint);
		triangulationBouguets(temp1[i],temp2[i],Q,tmppoint);
		points3dBouguets.push_back(*tmppoint);
		// INFOMSG(("RESULT: %.2f %.2f %.2f",point3dBouguets->x,point3dBouguets->y,point3dBouguets->z));
		// INFOMSG(("RESULT HAT: %.2f %.2f %.2f",point3dHartleys->x,point3dHartleys->y,point3dHartleys->z));
	}
	
	
	double xd, yd, zd, distance, errMean=0,err, errMax, errMin = 900, mean = 0, meanMin = 900, meanMax;
	int meanN = 0;
	
	for(int i = 0;i < points3dHartleys.size()-1;i++)
	{
		// printf("%.2f;%.2f;%.2f\n",points3dHartleys[i].x,points3dHartleys[i].y,points3dHartleys[i].z);
		// points3dHartleys[i].x,points3dHartleys[i].y,points3dHartleys[i].z
		xd = points3dHartleys[i+1].x - points3dHartleys[i].x;
		yd = points3dHartleys[i+1].y - points3dHartleys[i].y;
		zd = points3dHartleys[i+1].z - points3dHartleys[i].z;
		distance = sqrt(xd*xd + yd*yd + zd*zd);
		
		if((i+1)%CHESSBOARD_X == 0){
			continue;
		}
		
		err = CHESSBOARD_SQ_SIZE - distance;
		errMean = errMean+err;
		mean = mean+distance;
		                                   
		meanN++;
		
		if(err > errMax){
		 	errMax = err;
		}
		if(err < errMin){
		 	errMin = err;
		}
		if(distance > meanMax){
		 	meanMax = distance;
		}
		if(distance < meanMin){
		 	meanMin = distance;
		}
		// printf("%.4f\n",distance);
	}
	errMean = errMean/meanN;
	mean = mean / meanN;
	// INFOMSG(("errMean: %.2f +: %.2f -: %.2f",errMean,errMax,errMin));
	// INFOMSG(("Mean: %.2f +: %.2f -: %.2f",mean,meanMax,meanMin));
	// INFOMSG(("HARTLEYS:: Mean: +:%.2f  %.2f -:%.2f ::: errMean: +:%.2f %.2f -:%.2f",meanMax-mean,mean,mean-meanMin,errMax,errMean,errMin));
	
	errMean=0;
	errMin = 900;
	errMax = 0;
	meanN = 0;
	mean = 0;
	meanMin = 900;
	meanMax = 0;
	
	
	for(int i = 0;i < points3dBouguets.size()-1;i++)
	{
		if((i+1)%CHESSBOARD_X == 0){
			continue;
		}
		
		// printf("%.2f;%.2f;%.2f\n",points3dBouguets[i].x,points3dBouguets[i].y,points3dHartleys[i].z);
		xd = points3dBouguets[i+1].x - points3dBouguets[i].x;
		yd = points3dBouguets[i+1].y - points3dBouguets[i].y;
		zd = points3dBouguets[i+1].z - points3dBouguets[i].z;
		distance = sqrt(xd*xd + yd*yd + zd*zd);
		
		err = CHESSBOARD_SQ_SIZE - distance;
		errMean = errMean+err;
		mean = mean+distance;
		meanN++;
		
		if(err > errMax){
		 	errMax = err;
		}
		if(err < errMin){
		 	errMin = err;
		}
		if(distance > meanMax){
		 	meanMax = distance;
		}
		if(distance < meanMin){
		 	meanMin = distance;
		}
		// printf("%.4f\n",distance);
		// printf("%.4f\n",distance);
	}
	errMean = errMean/meanN;
	mean = mean / meanN;
	// INFOMSG(("BOUGUETS:: Mean: +:%.2f  %.2f -:%.2f ::: errMean: +:%.2f %.2f -:%.2f",meanMax-mean,mean,mean-meanMin,errMax,errMean,errMin));
	
}
int clsVideo3D::finishCalibrateCam3D()
{
	int iRet = 0;
	//CV STEREO CALIBRATION
	INFOMSG(("Loaded %d samples... pontos: %d %d",chessboardDetectedCounter,pointsDetected[0].size(),pointsDetected[1].size()));
	
	camerasPosition = 1;
	
	vector<CvPoint3D32f> objectPoints;
	vector<int> npoints;
	double M1[3][3], M2[3][3], D1[5], D2[5];
	double R[3][3], T[3], E[3][3], F[3][3];
	double Q[4][4];
	CvMat _M1 = cvMat(3, 3, CV_64F, M1 );
	CvMat _M2 = cvMat(3, 3, CV_64F, M2 );
	CvMat _D1 = cvMat(1, 5, CV_64F, D1 );
	CvMat _D2 = cvMat(1, 5, CV_64F, D2 );
	CvMat _R = cvMat(3, 3, CV_64F, R );
	CvMat _T = cvMat(3, 1, CV_64F, T );
	CvMat _E = cvMat(3, 3, CV_64F, E );
	CvMat _F = cvMat(3, 3, CV_64F, F );
	CvMat _Q = cvMat(4, 4, CV_64F, Q );
	
	CvSize imageSize = {0,0};
	int i,j;
	
	objectPoints.resize(chessboardDetectedCounter*CHESSBOARD_N);
	for( i = 0; i < CHESSBOARD_X; i++ )
		for( j = 0; j < CHESSBOARD_X; j++ )
			objectPoints[i*CHESSBOARD_X + j] = cvPoint3D32f(i*CHESSBOARD_SQ_SIZE, j*CHESSBOARD_SQ_SIZE, 0);
	for( i = 1; i < chessboardDetectedCounter; i++ )
		copy( objectPoints.begin(), objectPoints.begin() + CHESSBOARD_N,objectPoints.begin() + i*CHESSBOARD_N );
	
	npoints.resize(chessboardDetectedCounter,CHESSBOARD_N);
	imageSize = cvGetSize(frameSection[0] );
	// finishCalibrateCam3D();
	int N = chessboardDetectedCounter*CHESSBOARD_N;
	CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
	CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &pointsDetected[0][0] );
	CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &pointsDetected[1][0] );
	CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
	
	cvSetIdentity(&_M1);
	cvSetIdentity(&_M2);
	cvZero(&_D1);
	cvZero(&_D2);
	cvStereoCalibrate( &_objectPoints, &_imagePoints1,
		&_imagePoints2, &_npoints,
		&_M1, &_D1, &_M2, &_D2,
		imageSize, &_R, &_T, &_E, &_F,
		cvTermCriteria(CV_TERMCRIT_ITER+
			CV_TERMCRIT_EPS, 100, 1e-5),
		CV_CALIB_FIX_ASPECT_RATIO +
		CV_CALIB_ZERO_TANGENT_DIST +
		CV_CALIB_SAME_FOCAL_LENGTH );
	
	
	// CALIBRATION QUALITY CHECK
	// because the output fundamental matrix implicitly
	// includes all the output information,
	// we can check the quality of calibration using the
	// epipolar geometry constraint: m2^t*F*m1=0
	vector<CvPoint3D32f> lines[2];
	pointsDetected[0].resize(CHESSBOARD_N);
	pointsDetected[1].resize(CHESSBOARD_N);
	_imagePoints1 = cvMat(1, N, CV_32FC2, &pointsDetected[0][0] );
	_imagePoints2 = cvMat(1, N, CV_32FC2, &pointsDetected[1][0] );
	lines[0].resize(N);
	lines[1].resize(N);
	CvMat _L1 = cvMat(1, N, CV_32FC3, &lines[0][0]);
	CvMat _L2 = cvMat(1, N, CV_32FC3, &lines[1][0]);
	//Always work in undistorted space
	cvUndistortPoints( &_imagePoints1, &_imagePoints1,
		&_M1, &_D1, 0, &_M1 );
	cvUndistortPoints( &_imagePoints2, &_imagePoints2,
		&_M2, &_D2, 0, &_M2 );
	cvComputeCorrespondEpilines( &_imagePoints1, 1, &_F, &_L1 );
	cvComputeCorrespondEpilines( &_imagePoints2, 2, &_F, &_L2 );
	double avgErr = 0;
	for( i = 0; i < N; i++ )
	{
		double err = fabs(pointsDetected[0][i].x*lines[1][i].x +
			pointsDetected[0][i].y*lines[1][i].y + lines[1][i].z)
		+ fabs(pointsDetected[1][i].x*lines[0][i].x +
			pointsDetected[1][i].y*lines[0][i].y + lines[0][i].z);
		avgErr += err;
	}
	printf( "avg err = %g\n", avgErr/(chessboardDetectedCounter*CHESSBOARD_N) );
	
	//COMPUTE AND DISPLAY RECTIFICATION
	bool showUndistorted= true;
	useUncalibrated = 1;
	
	if( showUndistorted )
	{
		
		double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
		CvMat _R1 = cvMat(3, 3, CV_64F, R1);
		CvMat _R2 = cvMat(3, 3, CV_64F, R2);
		// IF BY CALIBRATED (BOUGUET'S METHOD)
		if( useUncalibrated == 0 )
		{
			CvMat _P1 = cvMat(3, 4, CV_64F, P1);
			CvMat _P2 = cvMat(3, 4, CV_64F, P2);
			cvStereoRectify( &_M1, &_M2, &_D1, &_D2, imageSize,
				&_R, &_T,
				&_R1, &_R2, &_P1, &_P2, &_Q,
				0/*CV_CALIB_ZERO_DISPARITY*/ );
			isVerticalStereo = fabs(P2[1][3]) > fabs(P2[0][3]);
			//Precompute maps for cvRemap()
			cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_P1,mx1,my1);
			cvInitUndistortRectifyMap(&_M2,&_D2,&_R2,&_P2,mx2,my2);
		}
		//OR ELSE HARTLEY'S METHOD
		else if( useUncalibrated == 1 || useUncalibrated == 2 )
			// use intrinsic parameters of each camera, but
		// compute the rectification transformation directly
		// from the fundamental matrix
		{
			double H1[3][3], H2[3][3], iM[3][3];
			CvMat _H1 = cvMat(3, 3, CV_64F, H1);
			CvMat _H2 = cvMat(3, 3, CV_64F, H2);
			CvMat _iM = cvMat(3, 3, CV_64F, iM);
			//Just to show you could have independently used F
			if( useUncalibrated == 2 )
			cvFindFundamentalMat( &_imagePoints1,
				&_imagePoints2, &_F);
			cvStereoRectifyUncalibrated( &_imagePoints1,
				&_imagePoints2, &_F,
				imageSize,
				&_H1, &_H2, 3);
			cvInvert(&_M1, &_iM);
			cvMatMul(&_H1, &_M1, &_R1);
			cvMatMul(&_iM, &_R1, &_R1);
			cvInvert(&_M2, &_iM);
			cvMatMul(&_H2, &_M2, &_R2);
			cvMatMul(&_iM, &_R2, &_R2);
			//Precompute map for cvRemap()
			
			cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_M1,mx1,my1);
			cvInitUndistortRectifyMap(&_M2,&_D1,&_R2,&_M2,mx2,my2);
		}
		else
			assert(0);
		
		// INFOMSG(("image undistorted..."));
		// cvNamedWindow( "rectified", 1 );
		// RECTIFY THE IMAGES AND FIND DISPARITY MAPS
		// if( !isVerticalStereo )
		// pair = cvCreateMat( imageSize.height, imageSize.width*2,
		// 	CV_8UC3 );
		// else
		// pair = cvCreateMat( imageSize.height*2, imageSize.width,
		// 	CV_8UC3 );
		//Setup for finding stereo correspondences
		BMState = cvCreateStereoBMState();
		assert(BMState != 0);
		BMState->preFilterSize=41;
		BMState->preFilterCap=31;
		BMState->SADWindowSize=41;
		BMState->minDisparity=-64;
		BMState->numberOfDisparities=128;
		BMState->textureThreshold=10;
		BMState->uniquenessRatio=15;
		// for( i = 0; i < chessboardDetectedCounter; i++ )
		// {
			
		// }
		INFOMSG(("TODO:: cvReleaseStereoBMState(&BMState);"));
		
	}
	
	
	// CvMat *rotationMatrix,*translationMatrix,*essentialMatrix,*fundamentalMatrix;
	
	// CvMat *objectPoints;
	// CvMat *pointCounts;
	// CvMat *intrinsicParams[2];
	// CvMat *distortionCoeffs[2];
	
	// if(chessboardDetectedCounter < 2)
	// {
	// 	return 0;
	// }
		
	// pointCounts = cvCreateMat( chessboardDetectedCounter , 1, CV_32SC1 );
	// objectPoints = cvCreateMat( chessboardDetectedCounter*CHESSBOARD_N , 3, CV_32FC1 );
	// for(int i=0; i < chessboardDetectedCounter; i++)
	// {
	// 	for(int x=0; x < CHESSBOARD_X; x++)
	// 	{
	// 		for(int y=0; y < CHESSBOARD_Y; y++){
	// 			CV_MAT_ELEM(*objectPoints, float, i, 0) = x*CHESSBOARD_SQ_SIZE;
	// 			CV_MAT_ELEM(*objectPoints, float, i, 1) = (i%CHESSBOARD_Y)*CHESSBOARD_SQ_SIZE;
	// 			CV_MAT_ELEM(*objectPoints, float, i, 2) = 0.0f;
	// 		}
	// 	}
	// 	CV_MAT_ELEM( *pointCounts, int, i, 0 ) = CHESSBOARD_N;
        // }
        
        
        // intrinsicParams[0] = cvCreateMat(3, 3, CV_32FC1);
        // intrinsicParams[1] = cvCreateMat(3, 3, CV_32FC1);
        
        // distortionCoeffs[0] = cvCreateMat(5, 1, CV_32FC1);
        // distortionCoeffs[1] = cvCreateMat(5, 1, CV_32FC1);
        
        // loadIntrinsic(intrinsicParams[0],0,camerasPosition);
        // loadDistortionCoeffs(distortionCoeffs[0],0,camerasPosition);
        
        // loadIntrinsic(intrinsicParams[1],1,camerasPosition);
	// loadDistortionCoeffs(distortionCoeffs[1],1,camerasPosition);
	
	// rotationMatrix            = cvCreateMat(3, 3, CV_64F);
        // translationMatrix         = cvCreateMat(3, 1, CV_64F);
        // essentialMatrix           = cvCreateMat(3, 3, CV_64F);
        // fundamentalMatrix         = cvCreateMat(3, 3, CV_64F);
        // float err;
	// err = cvStereoCalibrate( objectPoints, pointsSection[0], pointsSection[1], pointCounts,
        //                     intrinsicParams[0], distortionCoeffs[0], intrinsicParams[1], distortionCoeffs[1],
        //                     cvGetSize(frameSection[0]), rotationMatrix, translationMatrix, essentialMatrix, fundamentalMatrix,
        //                     cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5),
        //                     CV_CALIB_USE_INTRINSIC_GUESS + CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_SAME_FOCAL_LENGTH
        //          );
        // printf(" [ reprojection error = %.5f ]\n", sqrt(err/(chessboardDetectedCounter*2)));
        
        // printf("Rotation matrix:\n ");
        // PrintMat(rotationMatrix);
        
        // printf("Translation matrix:\n ");
        // PrintMat(translationMatrix);
        
        // printf("Essential matrix:\n ");
        // PrintMat(essentialMatrix);
        
        // printf("Fundamental matrix:\n ");
        // PrintMat(fundamentalMatrix);
        
        // printf("CAM 0 - intrinsic:\n ");
        // PrintMat(intrinsicParams[0]);
        
        // printf("CAM 0 - dist coeffs:\n ");
        // PrintMat(distortionCoeffs[0]);
        
        // printf("CAM 1 - intrinsic:\n ");
        // PrintMat(intrinsicParams[1]);
        
        // printf("CAM 1 - dist coeffs:\n ");
        // PrintMat(distortionCoeffs[1]);
        
        // CvMat *rectificationTransform[2];
        // CvMat *projectionMatrix[2];
        // CvMat *disparityTodepth;
        // CvSize rectifiedImageSize;
        
        // rectificationTransform[0] = cvCreateMat(3, 3, CV_64F);
        // rectificationTransform[1] = cvCreateMat(3, 3, CV_64F);
        
        // projectionMatrix[0]       = cvCreateMat(3, 4, CV_64F);
        // projectionMatrix[1]       = cvCreateMat(3, 4, CV_64F);
        
        // disparityTodepth          = cvCreateMat(4, 4, CV_64F);
        
        // cvStereoRectify(intrinsicParams[0], intrinsicParams[1], distortionCoeffs[0], distortionCoeffs[1], cvGetSize(frameSection[0]), rotationMatrix, translationMatrix,
        //                 rectificationTransform[0], rectificationTransform[1], projectionMatrix[0], projectionMatrix[1], disparityTodepth, NULL, 0.0f, rectifiedImageSize);
        
        // CvMat *newIntrisicParams[2];
        // newIntrisicParams[0] = cvCreateMat(3, 3, CV_32FC1);
        // newIntrisicParams[1]        = cvCreateMat(3, 3, CV_32FC1);
        // cvCopy(intrinsicParams[0] ,newIntrisicParams[0] );
        // cvCopy(intrinsicParams[1] ,newIntrisicParams[1] );
        
        // CvSize imageSize =  cvGetSize(frameSection[0]);
        
        // mx[0]        = cvCreateMat(imageSize.height, imageSize.width , CV_32FC1);
        // my[0]        = cvCreateMat(imageSize.height, imageSize.width , CV_32FC1);
        
        // mx[1]        = cvCreateMat(imageSize.height, imageSize.width , CV_32FC1);
        // my[1]        = cvCreateMat(imageSize.height, imageSize.width , CV_32FC1);
        
        // cvInitUndistortRectifyMap(intrinsicParams[0],distortionCoeffs[0],rectificationTransform[0],newIntrisicParams[0],mx[0],my[0]);
        // cvInitUndistortRectifyMap(intrinsicParams[1],distortionCoeffs[1],rectificationTransform[1],newIntrisicParams[1],mx[1],my[1]);
        
        
        
        
        return iRet;
}


int clsVideo3D::start3Dcalibration(int position)
{
	temp.resize(CHESSBOARD_N);
	chessboardDetectedCounter = 0;
	
	mx1 = cvCreateMat( imageSize.height,
		imageSize.width, CV_32F );
	my1 = cvCreateMat( imageSize.height,
		imageSize.width, CV_32F );
	mx2 = cvCreateMat( imageSize.height,
		imageSize.width, CV_32F );
	my2 = cvCreateMat( imageSize.height,
		imageSize.width, CV_32F );
	
	initRectify = true;
        // chessboardDetectedCounter = 0;
        
        // camerasPosition = position;
        
        // //Only for 
	// points3Dboard.resize(16);
	// int frameWidth = frameSection[0]->width;
	// int frame2ndCameraGap = 0;
	// for(int i = 0,inc = 50; i < points3Dboard.size(); inc+=100)
	// {
	// 	if(i == 8) //changing to points of 2nd camera!!
	// 	{
	// 		inc = 50;
	// 		frame2ndCameraGap = frameSection[0]->width;
	// 	}
	// 	points3Dboard[i] = cvPoint(frame2ndCameraGap+0+50,0+inc);
	// 	i++;
	// 	points3Dboard[i] = cvPoint(frame2ndCameraGap+frameWidth-50,inc);
	// 	i++;
	// }
	// cvNamedWindow(WINDOW_NAME);
	// cvSetMouseCallback( WINDOW_NAME, select3DboardHandler, NULL );
	
	// //Undistorting images??
	// CvMat *intrinsicParams[2];
	// CvMat *distortionCoeffs[2];
	
        
        
        // intrinsicParams[0] = cvCreateMat(3, 3, CV_32FC1);
        
        // distortionCoeffs[0] = cvCreateMat(5, 1, CV_32FC1);
        
        // loadIntrinsic(intrinsicParams[0],0,camerasPosition);
        // loadDistortionCoeffs(distortionCoeffs[0],0,camerasPosition);
        
        // cvInitUndistortMap(intrinsicParams[0],distortionCoeffs[0],mapx[0],mapy[0]);
        
        // //CAMERA 2
        
        
        // intrinsicParams[1] = cvCreateMat(3, 3, CV_32FC1);
        // distortionCoeffs[1] = cvCreateMat(5, 1, CV_32FC1);
        
        // loadIntrinsic(intrinsicParams[1],1,camerasPosition);
	// loadDistortionCoeffs(distortionCoeffs[1],1,camerasPosition);
	
        // cvInitUndistortMap(intrinsicParams[1],distortionCoeffs[1],mapx[1],mapy[1]);        
        
	return 0;
}


// 0 if found on both pics
// 1 on not found
// 2 samples done
int clsVideo3D::detectChessboard()
{
	
	if(chessboardDetectedCounter >= SAMPLES_3DCALIBRATION_N)
	{
		INFOMSG(("Done with samples..."));
		return 2;
	}
	int result = 0, count =0;
	bool chessboardFound[2];
	IplImage *framePainted[2];
	for(int j=0;j<2;j++)
	{
		chessboardFound[j] = false;
		
		IplImage *img = cvCloneImage(frameSection[j]);
		result = cvFindChessboardCorners( img, cvSize(CHESSBOARD_X, CHESSBOARD_Y),
			&temp[0], &count,
			CV_CALIB_CB_ADAPTIVE_THRESH |
			CV_CALIB_CB_NORMALIZE_IMAGE);
		cvReleaseImage(&img);
		
		if(result)
		{
			chessboardFound[j] = true;
			
			INFOMSG(("Chessboard found"));
			INFOMSG(("temp size: %d",temp.size()));
			
			framePainted[j] = cvCloneImage( frameSection[j]);
			cvDrawChessboardCorners( framePainted[j], cvSize(CHESSBOARD_X, CHESSBOARD_Y), &temp[0],
				count, result );
			
			
			vector<CvPoint2D32f>& pts = pointsDetected[j];
			int totalPoints = pts.size();
			pts.resize(totalPoints + CHESSBOARD_N, cvPoint2D32f(0,0));
			copy( temp.begin(), temp.end(), pts.begin() + totalPoints );
			
		}
	}
	if( !(chessboardFound[0] && chessboardFound[1]) )
	{
		INFOMSG(("Chessboard NOT found"));
		return 1;
	}
	chessboardDetectedCounter++;
	
	IplImage *framesPainted;
	framesPainted = cvCreateImage( cvSize( framePainted[0]->width*2, framePainted[0]->height ),
        	framePainted[0]->depth, framePainted[0]->nChannels );
        
        cvSetImageROI( framesPainted, cvRect( 0, 0, framePainted[0]->width, framePainted[0]->height ) );
        cvCopy(framePainted[0], framesPainted);
        cvResetImageROI(framesPainted);
        cvSetImageROI( framesPainted, cvRect(framePainted[1]->width,0, framePainted[1]->width, framePainted[1]->height) );
        cvCopy(framePainted[1], framesPainted);
        cvResetImageROI(framesPainted);
        
        // cvShowImage( "corners", framesPainted );
        cvReleaseImage( &framePainted[0] );
        cvReleaseImage( &framePainted[1] );
        cvReleaseImage( &framesPainted);
        
	
	
	return 0;
	// int step;
	
	// int cornersDetected0;
	// int resultFindChessboard0;
	
	// int cornersDetected1;
	// int resultFindChessboard1;
	
	// resultFindChessboard0 = 0;
	// cornersDetected0 = 0;
	
	// cornersSection[0] = new CvPoint2D32f[CHESSBOARD_N];
	// cvCvtColor(frameSection[0], frameGraySection[0], CV_BGR2GRAY);
	
	// resultFindChessboard0 = cvFindChessboardCorners(frameGraySection[0], cvSize(CHESSBOARD_Y,CHESSBOARD_X),
	// 		cornersSection[0], &cornersDetected0,
	// 		CV_CALIB_CB_ADAPTIVE_THRESH | /*CV_CALIB_CB_FILTER_QUADS | */ CV_CALIB_CB_NORMALIZE_IMAGE
	// 		);
	// if(resultFindChessboard0 == 0 || cornersDetected0 != CHESSBOARD_N)
	// {
	// 	INFOMSG(("Not found in section 0"));
	// 	return 0;
	// }
	
	// // NOW WE DETECT ON SECTION 1 the chessboard...
	// resultFindChessboard1 = 0;
	// cornersDetected1 = 0;
	
	// cornersSection[1] = new CvPoint2D32f[CHESSBOARD_N];
	// cvCvtColor(frameSection[1], frameGraySection[1], CV_BGR2GRAY);
	
	// resultFindChessboard1 = cvFindChessboardCorners(frameGraySection[1], cvSize(CHESSBOARD_X,CHESSBOARD_Y),
	// 		cornersSection[1], &cornersDetected1,
	// 		CV_CALIB_CB_ADAPTIVE_THRESH | /*CV_CALIB_CB_FILTER_QUADS | */ CV_CALIB_CB_NORMALIZE_IMAGE
	// 		);
	
	// if(resultFindChessboard1 == 0 || cornersDetected1 != CHESSBOARD_N)
	// {
	// 	INFOMSG(("Not found in section 1"));
	// 	return 0;
	// }
	
	
	
	// //Now we calculate subpixel... for section 0
	// cvFindCornerSubPix(frameGraySection[0], cornersSection[0], cornersDetected0,
	// 	cvSize(10, 10), cvSize(-1,-1),
	// 	cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,20,0.03) // FROM THE BOOK
	// 	// cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ) // FROM OLD CODE
	// 	);
	// step = chessboardDetectedCounter*CHESSBOARD_N;
	
	// for( int i=step, j=0; j < CHESSBOARD_N; ++i, ++j ){
	// 	CV_MAT_ELEM( *pointsSection[0], float, i, 0 ) = cornersSection[0][j].x;
	// 	CV_MAT_ELEM( *pointsSection[0], float, i, 1 ) = cornersSection[0][j].y;
	// }
	
	// //Now we calculate subpixel... for section 1
	// cvFindCornerSubPix(frameGraySection[1], cornersSection[1], cornersDetected1,
	// 	cvSize(10, 10), cvSize(-1,-1),
	// 	cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,20,0.03) // FROM THE BOOK
	// 	// cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ) // FROM OLD CODE
	// 	);
	// step = chessboardDetectedCounter*CHESSBOARD_N;
	
	// for( int i=step, j=0; j < CHESSBOARD_N; ++i, ++j ){
	// 	CV_MAT_ELEM( *pointsSection[1], float, i, 0 ) = cornersSection[1][j].x;
	// 	CV_MAT_ELEM( *pointsSection[1], float, i, 1 ) = cornersSection[1][j].y;
	// }
	
	// chessboardDetectedCounter++;
	
	// INFOMSG(("FOUND."));
	
	// char imagename[128];
	// sprintf(imagename,"image%d_%d.jpg",chessboardDetectedCounter-1,0);
	// cvSaveImage(imagename, frameSection[0] );
	// sprintf(imagename,"image%d_%d.jpg",chessboardDetectedCounter-1,1);
	// cvSaveImage(imagename, frameSection[1] );
	
	// return 0;
}

int clsVideo3D::rectifyImages()
{
	
	if(initRectify)
	{
		cvNamedWindow("disparity");
		cvCreateTrackbar("preFilterSize", "disparity", &BMState->preFilterSize, 41, trackbar_callback);
		initRectify = false;
	}
	IplImage* img1 = cvCreateImage( imageSize,
        	frameSection[0]->depth, 1);
        IplImage* img2 = cvCreateImage( imageSize,
        	frameSection[1]->depth, 1 );
        
        cvCvtColor(frameSection[0], img1, CV_BGR2GRAY);
        cvCvtColor(frameSection[1], img2, CV_BGR2GRAY);
        
        CvMat* img1r = cvCreateMat( imageSize.height,
        	imageSize.width, CV_8U );
        CvMat* img2r = cvCreateMat( imageSize.height,
        	imageSize.width, CV_8U );
        
        CvMat* disp = cvCreateMat( imageSize.height,
        	imageSize.width, CV_16S );
        CvMat* vdisp = cvCreateMat( imageSize.height,
        	imageSize.width, CV_8U );
        CvMat* pair;
        
        if( !isVerticalStereo )
        pair = cvCreateMat( imageSize.height, imageSize.width*2,
        	CV_8UC3 );
        else
        pair = cvCreateMat( imageSize.height*2, imageSize.width,
        	CV_8UC3 );
        
        int j;                              
        
        if( img1 && img2 )
        {
        	CvMat part;
        	
        	cvRemap( img1, img1r, mx1, my1 );
        	cvRemap( img2, img2r, mx2, my2 );
        	
        	if( !isVerticalStereo || useUncalibrated != 0 )
        	{
        		// When the stereo camera is oriented vertically,
        		// useUncalibrated==0 does not transpose the
        		// image, so the epipolar lines in the rectified
        		// images are vertical. Stereo correspondence
        		// function does not support such a case.
        		
        		// BMState->preFilterSize=41;
        		// BMState->preFilterCap=31;
        		// BMState->SADWindowSize=41;
        		// BMState->minDisparity=-64;
        		// BMState->numberOfDisparities=128;
        		// BMState->textureThreshold=10;
        		// BMState->uniquenessRatio=15;
        		
        		cvFindStereoCorrespondenceBM( img1r, img2r, disp,
        			BMState);
        		cvNormalize( disp, vdisp, 0, 256, CV_MINMAX );
        		cvNamedWindow( "disparity" );
        		cvShowImage( "disparity", vdisp );
        	}
		
        	if( !isVerticalStereo )
        	{
        		cvGetCols( pair, &part, 0, imageSize.width );
        		cvCvtColor( img1r, &part, CV_GRAY2BGR );
        		cvGetCols( pair, &part, imageSize.width,
        			imageSize.width*2 );
        		cvCvtColor( img2r, &part, CV_GRAY2BGR );
        		for( j = 0; j < imageSize.height; j += 16 )
        		cvLine( pair, cvPoint(0,j),
        			cvPoint(imageSize.width*2,j),
        			CV_RGB(0,255,0));
        	}
        	else
        	{
        		cvGetRows( pair, &part, 0, imageSize.height );
        		cvCvtColor( img1r, &part, CV_GRAY2BGR );
        		cvGetRows( pair, &part, imageSize.height,
        			imageSize.height*2 );
        		cvCvtColor( img2r, &part, CV_GRAY2BGR );
        		for( j = 0; j < imageSize.width; j += 16 )
        		cvLine( pair, cvPoint(j,0),
        			cvPoint(j,imageSize.height*2),
        			CV_RGB(0,255,0));
        	}
        	// cvCopy(pair,framesRectified);
        	
        }
        cvReleaseImage( &img1 );
        cvReleaseImage( &img2 );
        cvReleaseMat( &img1r );
        cvReleaseMat( &img2r );
        cvReleaseMat( &disp );
        
        //NEED TO RELEASE THIS AFTER ALL : 
        // cvReleaseMat( &mx1 );
        // 	cvReleaseMat( &my1 );
        // 	cvReleaseMat( &mx2 );
        // 	cvReleaseMat( &my2 );
        // return 1;
        // IplImage *t = cvCloneImage(frameSection[0]);
        // cvRemap( t, frameSection[0] , mx[0], my[0] );
        cvShowImage( "rectified", pair );
        
        char key = (char)cvWaitKey(FRAME_SLEEP);
        switch (key) {
        case 'q':
        case 'Q':
        case 27: //escape key
        	return false;
        default:
        	break;
        }
	
	
        // cvShowImage("yoo rectified", t);
        // t = cvCloneImage( image[1]); 
        // cvRemap( t, image[1] , mx[1], my[1] );
	return true;
}

int clsVideo3D::select3Dboard()
{
	
	vector<CvPoint> points3Dmarkers[2];
	points3Dmarkers[0].resize(8);
	points3Dmarkers[1].resize(8);
	
	for(int i=0;i<points3Dmarkers[0].size();i++)
	{
		points3Dmarkers[0][i] = points3Dboard[i];
	}
	
	for(int i=points3Dmarkers[0].size();i<points3Dmarkers[0].size()+points3Dmarkers[1].size();i++)
	{
		points3Dmarkers[1][i-points3Dmarkers[0].size()] = points3Dboard[i];
	}
	
	markerRectColor = CV_RGB(0, 0, 255);
	drawAllMarkers(frameCalibration, points3Dmarkers[0]);
	
	markerRectColor = CV_RGB(0, 255, 0);
	drawAllMarkers(frameCalibration, points3Dmarkers[1]);

}

void clsVideo3D::drawAllMarkers(IplImage *frame, vector<CvPoint> points)
{
	
	char tmp_txt[10];
        for(int i = 0; i < points.size(); i++){
                // cvCircle(frame,points[i],RADIUS_TO_DRAG,CV_RGB(0, 0, 255),2);
                cvCircle(frame,points[i],1, CV_RGB(0, 255, 0),1);
                cvRectangle(frame,cvPoint(points[i].x-RADIUS_MARKER,points[i].y-RADIUS_MARKER),cvPoint(points[i].x+RADIUS_MARKER,points[i].y+RADIUS_MARKER),markerRectColor,2);
                snprintf(tmp_txt,9,"%d",i+1);
                cvPutText(frame,tmp_txt,points[i],&fpsFont,CV_RGB(255, 0, 0));
        }
        
        // cvLine(frame,points[0],points[1],CV_RGB(0,255,0));
        
        // cvLine(frame,points[1],points[3],CV_RGB(0,255,0));
        
        // cvLine(frame,points[2],points[3],CV_RGB(0,255,0));
        // cvLine(frame,points[2],points[0],CV_RGB(0,255,0));
        
}


int clsVideo3D::drawChessboard()
{
	if(chessboardDetectedCounter < 1)
	{
		return 0;
	}
	CvPoint point0;
	CvPoint point1;
	
	for( int j=0; j < CHESSBOARD_N; ++j ){
		point0.x = cornersSection[0][j].x;
		point0.y = cornersSection[0][j].y;
		
		point1.x = cornersSection[1][j].x +frameSection[0]->width;
		point1.y = cornersSection[1][j].y;
		
		cvLine(frameCalibration,point0,point1,CV_RGB(0,255,0));
	}
	
        return 0;
}

void select3DboardHandler(int event, int x, int y, int flags, void *param)
{
	
	
        switch(event) {
        case CV_EVENT_LBUTTONDOWN:
		
		for(int i = 0; i <  points3Dboard.size(); i++){
			if(x > points3Dboard[i].x + RADIUS_MARKER ||  x < points3Dboard[i].x - RADIUS_MARKER)
			{
				continue;
			}
			if(y > points3Dboard[i].y + RADIUS_MARKER ||  y < points3Dboard[i].y - RADIUS_MARKER)
			{
				continue;
			}
			// INFOMSG(("
			dragMarker = i;
			
		}
        	
                break;
        case CV_EVENT_RBUTTONUP:
        	break;
        case CV_EVENT_RBUTTONDOWN:
                break;
        case CV_EVENT_MBUTTONUP:
        	break;
        case CV_EVENT_MBUTTONDOWN:
        	break;
        case CV_EVENT_LBUTTONUP:
        	// printf("CV_EVENT_LBUTTONUP\n");
        	dragMarker = -1;
                break;
        case CV_EVENT_MOUSEMOVE:
        	break;
        default:
        	dragMarker = -1;
        	break;
        }
        if(dragMarker > -1)
        {
        	points3Dboard[dragMarker] = cvPoint(x,y);
        }
        
}


int clsVideo3D::autoDetectMarkers()
{
	
	// pVideoAR->detectMarker(frameCalibration);
	
	char key = (char)cvWaitKey(FRAME_SLEEP); //delay N millis, usually long enough to display and capture input
	switch (key) {
	case ' ':
		if(detectChessboard())
		{
			return 1;
		}
		break;
	case 's':
	case 'S':
		save = true;
		return 1;
	case 'q':
	case 'Q':
	case 27: //escape key
		return 1;
	default:
		break;
	}
	
	//THIS IS FUN::
	// detectChessboard();
	// drawChessboard();
	
	fps = pfps.updateFPS();
	sprintf(tmpmsg,"fps: %.2f",fps);
	cvPutText(frameCalibration,tmpmsg,cvPoint(10,20),&fpsFont,cvScalar(255,0,0));
	
	cvShowImage(WINDOW_NAME, frameCalibration);
	
	return 0;
}

int clsVideo3D::save3Dcalibration(CvMat *M1,CvMat *D1,CvMat *R1, CvMat *M2,CvMat *D2,CvMat *R2)
{
	
	// cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_M1,mx1,my1);
	// cvInitUndistortRectifyMap(&_M2,&_D1,&_R2,&_M2,mx2,my2);
	
	return 0;
}

void clsVideo3D::triangulationHartleys(CvPoint2D32f point1, CvPoint2D32f point2,CvMat *P1,CvMat *P2,CvPoint3D32f *point3d){
        CvMat* A=cvCreateMat(4,4,CV_64FC1);
        CvMat* S=cvCreateMat(4,4,CV_64FC1);
        CvMat* U=cvCreateMat(4,4,CV_64FC1);
        CvMat* V=cvCreateMat(4,4,CV_64FC1);
        
        
        double d, x1, x2, y1, y2;
        
        x1 = point1.x;
        y1 = point1.y;
        x2 = point2.x;
        y2 = point2.y;
        
        d = x1 - x2;
        
        int rowA, rowB;
        //construct the A matrix in hartley/zisserman
        for (int j = 0; j < 4; j++) {
                
                rowA = 0; rowB = 2;
                cvmSet(A,0,j,x1*cvmGet(P1,rowB,j) - cvmGet(P1,rowA,j));
                cvmSet(A,2,j,x2*cvmGet(P2,rowB,j) - cvmGet(P2,rowA,j));
                
                rowA = 1; rowB = 2;
                cvmSet(A,1,j,y1*cvmGet(P1,rowB,j) - cvmGet(P1,rowA,j));
                cvmSet(A,3,j,y2*cvmGet(P2,rowB,j) - cvmGet(P2,rowA,j));
        }
        
        //perform SVD
        cvSVD(A, S, U, V );
        
        //normalize the last column of the V matrix
        double normalizing_number = cvmGet(V,3,3);
        
        if(normalizing_number != 0){
                cvmSet(V,0,3,cvmGet(V,0,3)/cvmGet(V,3,3));
                cvmSet(V,1,3,cvmGet(V,1,3)/cvmGet(V,3,3));
                cvmSet(V,2,3,cvmGet(V,2,3)/cvmGet(V,3,3));
                cvmSet(V,3,3,cvmGet(V,3,3)/cvmGet(V,3,3));
        }
        
        //print the 3d point solution. Note that this solution is 4x1! this
        //is because the points are in homogeneous format ... i.e. (X,Y,Z,1)
        
        point3d->x = cvmGet(V,0,3);
        point3d->y = cvmGet(V,1,3);
        point3d->z = cvmGet(V,2,3);
        
}

void clsVideo3D::triangulationBouguets(CvPoint2D32f point1, CvPoint2D32f point2,CvMat *Q,CvPoint3D32f *point3d){
        double d, X, Y, Z, W, x1, x2, y1, y2;
        
        x1 = point1.x;
        y1 = point1.y;
        x2 = point2.x;
        y2 = point2.y;
        
        d = x1 - x2;
        // X = x1 * cvmGet(Q,0, 0) + cvmGet(Q,0, 3);
        // Y = y1 * cvmGet(Q,1, 1) + cvmGet(Q,1, 3);
        X = x1 * cvmGet(Q,0, 0);
        Y = y1 * cvmGet(Q,1, 1);
        Z = cvmGet(Q,2, 3);
        W = d * cvmGet(Q,3, 2) + cvmGet(Q,3, 3);
        
        // point3d->x = X / W ;
        // point3d->y = Y / W ;
        // point3d->z = Z / W ;
        point3d->x = X;
        point3d->y = Y;
        point3d->z = Z;
        // result->w = W;
}
