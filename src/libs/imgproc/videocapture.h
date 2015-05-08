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
#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include "utils.h"
#include "def.h"
#include "fps.h"
#include "timers.h"

/** TEST MODE **/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
// #include "videorecord.h"
/** END OF TEST MODE **/


#include "highgui.h"
#include "cvaux.h"

// #define VIDEOFRAME_SLEEP 100000
// #define VIDEOFRAME_SLEEP 10000
#define VIDEOFRAME_SLEEP 0
using namespace cv;

class clsVideoCapture
{
public:
	clsVideoCapture();
	~clsVideoCapture();
	
	bool startVideoCapture(int deviceNumber);
	bool startVideoCapture(int deviceNumber, int width, int height);
	bool startVideo(std::string filename);
	bool close();
	
	bool capture();
	bool setFrameCaptured(cv::Mat frame);
	
	bool getCaptureImage(IplImage *image);
	cv::Mat getCaptureImage();
	int width();
	int height();
	void setImageSize(int w,int h);
	
	bool isVideo;
	long frameCount;
	std::map<std::string,double> camSpecs();
protected: 
	cv::Mat rawImage;
	
private:
	cv::VideoCapture device;
	int rWidth,rHeight;

};

#endif
//docs:
// CvMat* cvGetMat( const CvArr* arr, CvMat* mat, int* coi = 0, int allowND );
// IplImage* cvGetImage( const CvArr* arr, IplImage* image_header );   
