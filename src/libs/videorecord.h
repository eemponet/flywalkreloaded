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
#ifndef VIDEO_RECORD_H
#define VIDEO_RECORD_H


#include "utils.h"
#define YARP_WITH_CV
#include "yarp.h"
#include <cvaux.h>
#include <highgui.h>

#define MAXFILENAME_DATALOGGER 128
class clsVideoRecord
{
public:
	clsVideoRecord();
	~clsVideoRecord();
	
	bool addNewFrame(IplImage *img);
	
	void record(char *filename,CvSize imgSz,int fps);
	void stop();

	void composeAllCameraImages(IplImage *img);
	void addTimerTrial(IplImage *img,char *str);
	
	void connectCameraImages(char *portIN, char *portOUT);
	
private:
	bool createNewFile(char *filename,CvSize imgSz);
	
	bool closeFile();
	
	bool isRecording;
	char filename[MAXFILENAME_DATALOGGER];
	
	CvVideoWriter* videoWriterFile;
	
	char dateTime[30];
	
	clsYarp *pYarpStream[2][2];
	
	clsTimers fpsTimer;
	
	int fps;
	
	
};

#endif
