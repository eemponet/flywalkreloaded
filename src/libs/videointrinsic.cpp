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
#include "videointrinsic.h"

clsVideoIntrinsic::clsVideoIntrinsic()
{
}
clsVideoIntrinsic::~clsVideoIntrinsic()
{
}

bool clsVideoIntrinsic::startCalibrateIntrisicParameters()
{
	INFOMSG(("Starting to calibrate the intrisic parameteres"));
	INFOMSG(("You can now add chessboard samples using ' ' and to finish and save use 'w'\n"));
	cvNamedWindow(WINDOW_NAME);
	
	persChessboardPoints = cvCreateMat( CHESSBOARD_N*SAMPLES_N, 2, CV_32FC1 );
	perCurrentCorners = new CvPoint2D32f[ CHESSBOARD_N ];
	//persChessboardPoints.resize(10*7);
	
	CvSize imageSize = cvGetSize( frame );
	
	persFrameGray = cvCreateImage(imageSize, IPL_DEPTH_8U, 1);
	
	persAddNewSample = false;
	
	perSampleNr = 0;
	
	return true;
}

bool clsVideoIntrinsic::calibrateIntrisicParameters()
{
	if(!remoteCamera)
	{
		frame = cvQueryFrame(capture);
	}
	cvShowImage(WINDOW_NAME, frame);
	// DO STUFFF
	
	if(persAddNewSample)
	{
		
		cvCvtColor(frame, persFrameGray, CV_BGR2GRAY);
		
		CvPoint2D32f* corners;
		corners = new CvPoint2D32f[ CHESSBOARD_N ];
		
		int step = 0;
		int cornersDetected = 0;
		int result = cvFindChessboardCorners(frame, cvSize(CHESSBOARD_Y,CHESSBOARD_X),
			corners, &cornersDetected,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS
			);
		
		if(result && cornersDetected == CHESSBOARD_N){
			
			cvFindCornerSubPix(persFrameGray, corners, cornersDetected,
				cvSize(10, 10), cvSize(-1,-1),
				cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03) // FROM THE BOOK
				// cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ) // FROM OLD CODE
				);
			step = perSampleNr*CHESSBOARD_N;
			
			for( int i=step, j=0; j < CHESSBOARD_N; ++i, ++j ){
				CV_MAT_ELEM( *persChessboardPoints, float, i, 0 ) = corners[j].x;
				CV_MAT_ELEM( *persChessboardPoints, float, i, 1 ) = corners[j].y;
        		}
        		perSampleNr++;
        		INFOMSG(("FOUND."));
        	}else{
        		INFOMSG(("Fail to add new frame, chessboard not found"));
        	}
        	
        	persAddNewSample = false;
        }
        
        
	//FINISHING STUFF
	char key = (char)cvWaitKey(FRAME_SLEEP); //delay N millis, usually long enough to display and capture input
	switch (key) {
	case ' ':
	case 'a':
		persAddNewSample = true;
		if(perSampleNr >= SAMPLES_N)
		{
			return true;
		}
		break;
	case 'q':
	case 'Q':
	case 27: //escape key
		return true;
	default:
		break;
	}
	
	// persAddNewSample = true;
	
	return false;
}

bool clsVideoIntrinsic::finishCalibrateIntrisicParameters(int cameraSection,int cameraPosition)
{
	if(perSampleNr < SAMPLES_N)
	{
		ERRMSG(("Failed to calibrate you need to add frames with the chessboard to calibrate"));
		return false;
	}
	CvMat *objectPoints = cvCreateMat( perSampleNr*CHESSBOARD_N , 3, CV_32FC1 );
        CvMat *imagePoints = cvCreateMat( perSampleNr*CHESSBOARD_N , 3, CV_32FC1 );
        CvMat *pointCounts = cvCreateMat( perSampleNr , 1, CV_32SC1 );
        
        CvMat *intrinsicParams        = cvCreateMat(3, 3, CV_32FC1);
        CvMat *distortionCoefficients = cvCreateMat(5, 1, CV_32FC1);
        
        CvSize imageSize = cvGetSize( frame );
        
        int j = 0;
        
        for(int i=0; i < perSampleNr; i++){
                for(int x=0; x < CHESSBOARD_X; x++){
                        for(int y=0; y < CHESSBOARD_Y; y++){
                                CV_MAT_ELEM(*objectPoints, float, j, 0) = x*CHESSBOARD_SQ_SIZE;
                                CV_MAT_ELEM(*objectPoints, float, j, 1) = (j%CHESSBOARD_Y)*CHESSBOARD_SQ_SIZE;
                                CV_MAT_ELEM(*objectPoints, float, j, 2) = 0.0f;
                                
                                
                                CV_MAT_ELEM(*imagePoints, float, j, 0) = CV_MAT_ELEM( *persChessboardPoints, float, j, 0 );
                                CV_MAT_ELEM(*imagePoints, float, j, 1) = CV_MAT_ELEM( *persChessboardPoints, float, j, 0 );
                                CV_MAT_ELEM(*imagePoints, float, j, 2) = 0.0f;
                                
                                j++;
                        }
                }
                CV_MAT_ELEM( *pointCounts, int, i, 0 ) = CHESSBOARD_N;
        }
        
        cvCalibrateCamera2(objectPoints, imagePoints, pointCounts, imageSize, intrinsicParams, distortionCoefficients);
        
        PrintMat(intrinsicParams);
        PrintMat(distortionCoefficients);
        
        //saving to files!!!
        saveIntrinsic(intrinsicParams,cameraSection,cameraPosition);
        saveDistortionCoeffs(distortionCoefficients,cameraSection,cameraPosition);
        
	cvReleaseImage(&persFrameGray);
	
	return true;	
}
bool clsVideoIntrinsic::startUndistortedImage(int section,int position)
{
	// EXAMPLE OF LOADING THESE MATRICES BACK IN:
	CvMat *intrinsic = cvCreateMat(3, 3, CV_32FC1);
	CvMat *distortion = cvCreateMat(5, 1, CV_32FC1);
	// Build the undistort map that we will use for all
	// subsequent frames.
	//
	loadIntrinsic(intrinsic,section,position);
	loadDistortionCoeffs(distortion,section,position);
	
	INFOMSG(("Loading..."));
	PrintMat(intrinsic);
        PrintMat(distortion);
        
	mapx = cvCreateImage( cvGetSize(frame), IPL_DEPTH_32F, 1 );
	mapy = cvCreateImage( cvGetSize(frame), IPL_DEPTH_32F, 1 );
	cvInitUndistortMap(
		intrinsic,
		distortion,
		mapx,
		mapy
		);

}
bool clsVideoIntrinsic::showUndistortImage()
{
	IplImage *t = cvCloneImage(frame);
	cvShowImage("Calibration", frame ); // Show raw image
	cvRemap( t, frame, mapx, mapy );
	// Undistort image
	cvReleaseImage(&t);
	cvShowImage("Undistort", frame);
	// Show corrected image
	
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
