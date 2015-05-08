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
#ifndef UTILS_H

#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <map>
#include <string>
#include "def.h"

#include "timers.h"
#define DEBUG_ON 1
#define TODO(exp) printf("TODO::");printf exp;DEBUG_LINES

#ifdef DEBUG_ON
        #define DEBUGMSG(exp) printf("DEBUG:: ");printf exp;DEBUG_LINES
#else
        #define DEBUGMSG(exp)
#endif

#define INFOMSG(exp) printf("INFO:: ");printf exp;DEBUG_LINES
// #define WARNMSG(exp) printf("\n-------------------------------\n-------------/!\\---------------\n-------------------------------\nWARN:: ");printf exp;DEBUG_LINES;printf("-------------------------------\n-------------/!\\---------------\n-------------------------------\n");
#define ERRMSG(exp) printf("ERROR:: ");printf exp;DEBUG_LINES;exit(1);
#define WARNMSG(exp) printf("WARN:: ");printf exp;DEBUG_LINES;
#define DEBUG_LINES printf("(%s,%d)\n",__FILE__,__LINE__);
// std::map<std::string,int> paramsInt;
//#define DEBUG_LINES printf("\n");
typedef struct pts2d
{
	int id;
	int timestamp;
	int cameraSection;
	int cameraPosition;
	
	double x;
	double y;
	
	int xPx;
	int yPx;
	
	bool avail;
	
	float fps;
	
}pts2d;

struct pts3D{
	int xPx,yPx,zPx;
	float xCm,yCm,zCm;
	
	int timestampReconstr;

	int sideSection;
	int topSection;

	int camTimestamp[2][2];
	int camxPx[2][2];
	int camxPy[2][2];
	double fps[2][2];
};

typedef struct color
{
	int r;
	int g;
	int b;
}color;

enum{
	PERSPECTIVE_MATRICES = 0,
	INTRINSIC_PARAMETERS,
	PERSPECTIVE_LINES
};

enum{
	SIDE_CAMERA = 0,
	TOP_CAMERA,
};

enum
{
	IMG_SENT = 0,
	IMG_COPIED,
	IMG_STOP,
};

struct camera2DCFG{
	/**
	0 - simple frame subtraction
	1 - color threshold
	2 - bgfg codebook
	**/
	int type2Dtracking; 
	
	/** All **/
	int sqr_blur;
	
	/** simple frame subtraction **/
	int minThrs,maxThrs;
	
	/** color threshold: **/
	int minH,maxH,minS,maxS,minL,maxL;
	
	/** bgfg codebook **/
	int nframes;
	int erode;
};

struct options2dTrack{
	int cameraId;
	int cameraPosition;
	int cameraSection;
	bool showfps;
	bool gui;
	bool video;
	char filename[MAX_FILENAME_SIZE];
	bool sendImgYarp;
	bool clickTracking;
	camera2DCFG *cameraOptions;
	bool showGUIsettings;
	bool calibration;
	bool autoRecordOnMotion;
	bool testMode;
};

typedef struct resolution
{
	int w;
	int h;
	std::string caption;
}resolution;

int getTimestamp();

int getTimestampMicro();

void milisToStr(char *str, int timeInMilis);
	
bool mkpath( char *path );
bool fileexists(char *filename);
void loadHARDCODEDOPTIONS(camera2DCFG *camera2DCFGs,int section,int position);
float getFreeRAM();
float getRAMusage();
float getTotalRAM();
    
#endif
