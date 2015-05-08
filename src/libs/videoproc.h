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
#ifndef VIDEO_PROC_H
#define VIDEO_PROC_H

#include "utils.h"
#include "def.h"
#include "fps.h"
#include "timers.h"

/** TEST MODE **/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "videorecord.h"
/** END OF TEST MODE **/

#include <vector>
#include <highgui.h>
#include <cvaux.h>

using namespace cv;

#define WINDOW_NAME "video | q or esc to quit"
#define WINDOW_NAME_DEBUG "other window"
#define WINDOW_NAME_DEBUG2 "other window2"

void trackbar_callback(int theSliderValue);

struct PERSPECTIVE_MATRICES_arenaSizeInCms
{
	float arena_s1_top_height;
	float arena_s1_top_width;
	float arena_s1_side_height;
	float arena_s1_side_width;
	
	float arena_s2_top_height;
	float arena_s2_top_width;
	float arena_s2_side_height;
	float arena_s2_side_width;
};


static int particle_itr_id = 1;

class clsParticle
{
public:
	clsParticle()
	{
		maxSizeOfTrail=MAX_SIZE_OF_TRAIL;
		x = -1;
		y = -1;
		xInCm = -1;
		yInCm = -1;
		updated = false;
		draw = true;
	}
	clsParticle(int x,int y)
	{
		maxSizeOfTrail=MAX_SIZE_OF_TRAIL;
		id = particle_itr_id++;
		this->x = x;
		this->y = y;
		this->area = 0;
		
		xInCm = -1;
		yInCm = -1;
		
		color = CV_RGB(0,255,0);
		points.push_back(cvPoint(x,y));
		
		surroundRect1 = cvPoint(0,0);
		surroundRect2 = cvPoint(0,0);
		draw = true;
	}
	clsParticle(int x,int y,double area)
	{
		maxSizeOfTrail=MAX_SIZE_OF_TRAIL;
		this->area = area;
		
		id = particle_itr_id++;
		this->x = x;
		this->y = y;
		
		xInCm = -1;
		yInCm = -1;
		
		color = CV_RGB(0,255,0);
		points.push_back(cvPoint(x,y));	
		draw = true;
		//TODO: THIS SHOULD USE THE SAME FUNCTION AS ABOVE!!!!
	}
	~clsParticle()
	{
	}
	bool is_inside(CvSeq *cesq)
	{
		CvPoint2D32f tmp = cvPoint2D32f(x,y );
		double t = cvPointPolygonTest(cesq,tmp,0);
		return t>0;
	}
	
	double calc_distance(int newx,int newy)
	{

		current_distance = fabs(sqrt( (newx-x)*(newx-x) + (newy-y)*(newy-y)));
		return current_distance;
	}
	
	void disupdate()
	{
		updated = false;
	}
	
	void set_new_coords(int x, int y)
	{
		this->x = x;
		this->y = y;
		draw = false;
		points.push_back(cvPoint(x,y));
		
		while((int)points.size() > maxSizeOfTrail)
		{
			points.erase(points.begin());
		}
		
		updated = true;
	}
	
	void set_coords_in_cm(float x, float y)
	{
		this->xInCm = x;
		this->yInCm = y;
		
	}
	
	void draw_trail(IplImage *frame)
	{
		int t = 0;
		while((int)points.size() > t)
		{
			cvRectangle(frame, cvPoint(points[t].x,points[t].y),cvPoint(points[t].x+2,points[t].y+2), color, 1, 40, 0);
			t++;
		}
		
		t = 1;
		while((int)points.size() > t)
		{
			cvLine(frame, cvPoint(points[t].x,points[t].y),cvPoint(points[t-1].x+1,points[t-1].y+1), color, 1, 40, 0);
			t++;
		}
		
		// if(!draw){
			
			cvRectangle(frame, surroundRect1,surroundRect2, CV_RGB(255,0,0), 1, 40, 0); //the rectangle is drawn on the Colored Image //DP
			// draw = true;
		// }
		
		
	}
	int id;
	int x; //in pixels
	int y; //in pixels
	
	float xInCm; //in pixels
	float yInCm; //in pixels
	
	CvScalar color;
	vector <CvPoint> points;
	double current_distance;
	double area;
	int updated;
	int maxSizeOfTrail;
	CvMemStorage* storage; //= cvCreateMemStorage(0);
	
	CvPoint surroundRect1, surroundRect2;
	
	bool draw;
};

class clsVideo
{
public:
	clsVideo();
	~clsVideo();
	
	bool init_video(int cameraId);
	bool init_video(char *filename);
	bool init_video();
	
	bool showVideo();
	
	bool oneParticleTracking(pts2d *points);
	bool start_2dpts(options2dTrack *currOptions);
	bool checkFPS();
	
	bool clickTracking(pts2d *points);
	
	void closest_particle();
	
	bool startCalibratePerspectiveMatrices(int cameraSection, int cameraPosition);
	bool calibratePerspectiveMatrices();
	bool finishCalibratePerspectiveMatrices(int cameraSection, int cameraPosition);
	
	bool startCalibratePerspectiveLines();
	bool calibratePerspectiveLines();
	bool finishCalibratePerspectiveLines(int cameraSection,int cameraPosition );
	bool loadPerspectiveLines(CvMat *matrix,int sec,int pos);
	
	void PrintMat(CvMat *A);
	void drawMarkers(IplImage *frame, vector<CvPoint> points);
	
	bool loadIntrinsic(CvMat *matrix,int cameraSection, int cameraPosition);
	bool loadDistortionCoeffs(CvMat *matrix,int cameraSection, int cameraPosition);
	bool loadPerspectiveMatrices(CvMat *matrix,int cameraSection, int cameraPosition);
	
	bool saveIntrinsic(CvMat *matrix,int cameraSection, int cameraPosition);
	bool saveDistortionCoeffs(CvMat *matrix,int cameraSection, int cameraPosition);
	bool savePerspectiveMatrices(CvMat *matrix,int cameraSection, int cameraPosition);
	bool loadArenaInCms(int section,int position,CvPoint2D32f *arenaPointsInCms);
	
	bool start2DCorrect( );
	bool pixelsToArenaCms(pts2d *point,pts3D lastPoint);
	
	void calculatePerspective(pts2d p, CvMat *perspectiveMatrix,CvPoint2D32f *pDst,int perspectiveLine);
	bool isPointInsideSection(pts2d point);
	
	void startBGFGCodeBook();
	
	void setRemoteCamera();
	void setCurrentFrame(IplImage *remoteframe);
	bool startRemoteCamera(IplImage *remoteFrame);
	
	bool gui;
	
	CvBGCodeBookModel* model;
	int nframes;
	IplImage *ImaskCodeBook,*ImaskCodeBookCC;
	
	int lockedSendOutImage;
	IplImage *frame_to_send;
	IplImage *frame_to_send_tracing;
	IplImage *frame_to_send_calib;
	
	bool startMotionDetection();
	bool thresholdCam();
	bool motionDetection();
	
	bool overlappingSections(pts2d point,pts3D last3Dpoint);
	int cfgnframes;
	int minAreaToTrack;
	/**
	TEST
	**/
	bool allocOneParticleTrackingTest();
	bool oneParticleTrackingTest();

	clsParticle *particlebgfg;
	clsParticle *particlesubtraction;
	
	pts2d point;
	
	void resetBGFG();
	
	IplImage *frame_output;
	/** end of test stuff **/
	

	
protected: 
	bool save;
	clsFPS pfps;
	float fps;
	CvFont fpsFont;
	char tmpmsg[128];
	
	CvCapture *capture;
	IplImage *frame;
	
	bool remoteCamera;
	
private:
	
	IplImage *frame_smooth;
	IplImage *frame_debug;
	IplImage *frame_sub;
	IplImage *frame_countors;
	
	IplImage *frame_buffer;
	IplImage *frame_buffer2;
	IplImage *frame_threshold;
	IplImage *frame_bufferbgfg;
	CvMemStorage* storage;
	CvSeq* contour;
	CvSeq* contourLow;
	
	int cameraIdx;
	
	bool pause;
	int minH,maxH,minS,maxS,minL,maxL;
	int sqr_blur;
	int erodeThreshold;
	
	bool camera;
	
	int minThrs,maxThrs;
	
	int droppingframes;
	int print_now;
	
	bool readyTo2Dtrack;
	
	vector<clsParticle *> particles_curr;
	vector<clsParticle *> particles;
	
	vector<clsParticle *> particles_limbo;
	
	//PERSPECTIVE MATRICES AUX
	CvMat *PERSPECTIVE_MATRICES_intrinsicParams;
        CvMat *PERSPECTIVE_MATRICES_distortionCoefficients;
	CvMat *PERSPECTIVE_MATRICES_mx;
	CvMat *PERSPECTIVE_MATRICES_my;
	
	CvMat *PERSPECTIVE_MATRICES_perspectiveMatrixes;	
	
	CvPoint2D32f* PERSPECTIVE_MATRICES_arenaPointsInCm;
	int  PERSPECTIVE_MATRICES_calculateMatrix;
	CvFont PERSPECTIVE_MATRICES_font;
	
	clsParticle *particle_curr;
	
	char myFilename[128];
	
	#ifndef DO_NOT_USE_INTRINSIC
	CvMat *allMatricesIntrinsic[2][2];
	CvMat *allMatricesDistCoeffs[2][2];
	#endif
	CvMat *allMatricesPerspective[2][2];
	CvMat *perspectiveMatrix;
	CvMat *perspectiveLines[2][2];
	
	int chg_mode;
	
	options2dTrack *options;

	void colorThreshold();
	void framesubtraction();
	void bgfgCodeBook();
	void zoomHandler(IplImage *frame);
	
	void findParticle(IplImage *frame_countors, pts2d *point);
	void findParticle2(IplImage *frame_countors, pts2d *point);
	CvMat *arenaSizes;
	
	int GUImode;
	bool setRawVideo;
	
	IplImage *frame_to_send_buffer_tracing;
	CvPoint2D32f pDst;
	
	int refZoom;
	int zoomsetX,zoomsetY,zoomwidth,zoomheight;
	
	clsTimers overlapXSideTimer;
	clsTimers overlapXTopTimer;
	
	bool findMotion(IplImage *frame_countors);
	bool camSpecs();
	
};

#endif
