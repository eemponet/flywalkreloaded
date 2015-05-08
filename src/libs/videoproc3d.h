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
// #include "videoprocAR.h"

#define RADIUS_MARKER 10

class clsVideo3D : public clsVideo {

public: 
	clsVideo3D();
	~clsVideo3D();
	
	
	int start3Dcalibration(int position);
	int startRemoteCam3D(IplImage *remoteFrame);
	int setRemoteFrames(IplImage *img0, IplImage *img1);
	int calibrateCam3D();
	
	int finishCalibrateCam3D();
	int rectifyImages();
	int select3Dboard();
	int autoDetectMarkers();
	
	int calibrateImages3D();
	
private:
	int detectChessboard();
	int drawChessboard();
	int save3Dcalibration(CvMat *M1,CvMat *D1,CvMat *R1, CvMat *M2,CvMat *D2,CvMat *R2);
	
	void triangulationHartleys(CvPoint2D32f point1, CvPoint2D32f point2,CvMat *P1,CvMat *P2,CvPoint3D32f *point3d);
	void triangulationBouguets(CvPoint2D32f point1, CvPoint2D32f point2,CvMat *Q,CvPoint3D32f *point3d);
	void calculateChessboard3DError(vector<CvPoint2D32f> temp1,vector<CvPoint2D32f> temp2, CvMat *P1, CvMat *P2, CvMat *Q);
	
	void drawAllMarkers(IplImage *frame, vector<CvPoint> points );
	
	int camerasPosition;
	
	IplImage *frameCalibration;
	IplImage *frameSection[2];
	IplImage *frameCalibrationUndistort;
	
	IplImage *mx[2],*my[2];
	IplImage *mapx[2],*mapy[2];
	
	bool detectChessboardToContinue;
	//stereo calibration:
	vector<CvPoint2D32f> temp;
	vector<CvPoint2D32f> pointsDetected[2];
	bool isVerticalStereo;
	int useUncalibrated;
	CvSize imageSize;
	CvMat* mx1, *my1, *mx2, *my2;
	CvStereoBMState *BMState;
	IplImage *framesRectified;
	//drag points
	CvPoint2D32f* cornersSection[2];
	
	int chessboardDetectedCounter;
	
	CvScalar markerRectColor;
	
	bool initRectify;
	// clsVideoAR *pVideoAR;
	

};
