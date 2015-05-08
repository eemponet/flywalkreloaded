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
#include "videocapture.h"

clsVideoCapture::clsVideoCapture()
{
	isVideo = false;
	rWidth = -1;
	rHeight = -1;
}
clsVideoCapture::~clsVideoCapture()
{
        
}

bool clsVideoCapture::startVideo(std::string filename)
{
        frameCount = 0;
	device.open(filename);
	isVideo = true;
	
	return true;
}
bool clsVideoCapture::startVideoCapture(int deviceNumber)
{
	// capture = cvCaptureFromAVI(deviceNumber);
	// device = cvCreateCameraCapture(deviceNumber);
	frameCount = 0;
	INFOMSG(("opening device %d",deviceNumber));
	device.open(deviceNumber);
	// capture();
	// camSpecs();
	
	return true;
}

bool clsVideoCapture::startVideoCapture(int deviceNumber, int width, int height)
{
	
        frameCount = 0;
        
	startVideoCapture(deviceNumber);
	
	if(width != -1)
		device.set(CV_CAP_PROP_FRAME_WIDTH, width);
	if(height != -1)
		device.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	
	return true;
}
 

bool clsVideoCapture::capture()
{
        // return false;
	if (!device.grab())
		return false;
	
	
	device.retrieve(rawImage);
	
	if(isVideo && VIDEOFRAME_SLEEP > 0){
	        usleep(VIDEOFRAME_SLEEP);
	}
	frameCount++;
	return true;
}

cv::Mat clsVideoCapture::getCaptureImage()
{	
	// if(rWidth != -1 && rHeight != -1)
	// {
	        // cv::resize(rawImage, rawImage, cvSize( rWidth,rHeight));
	// }
	
	return rawImage;
}

void clsVideoCapture::setImageSize(int w,int h)
{
        rWidth = w;
        rHeight = h;
}
bool clsVideoCapture::close()
{
	if(!device.isOpened())
	{
		return false;
		
	}
	// Disconnect camera
	device.release();
	return true;
}

int clsVideoCapture::width()
{
        if(rWidth != -1 )
                return rWidth;
	return device.get(CV_CAP_PROP_FRAME_WIDTH);
}
int clsVideoCapture::height()
{
        if(rHeight != -1 )
                return rHeight;
	return device.get(CV_CAP_PROP_FRAME_HEIGHT);
}

bool clsVideoCapture::setFrameCaptured(cv::Mat image)
{
	rawImage = image;
	return true;
}


std::map<std::string,double> clsVideoCapture::camSpecs()
{
        std::map<std::string,double> specs;
        specs["CV_CAP_PROP_POS_MSEC"] = device.get(CV_CAP_PROP_POS_MSEC);
	INFOMSG(("CV_CAP_PROP_POS_MSEC        %.4f\n",device.get(CV_CAP_PROP_POS_MSEC      )));
	INFOMSG(("CV_CAP_PROP_POS_FRAMES      %.4f\n",device.get(CV_CAP_PROP_POS_FRAMES    )));
	INFOMSG(("CV_CAP_PROP_POS_AVI_RATIO   %.4f\n",device.get(CV_CAP_PROP_POS_AVI_RATIO )));
	INFOMSG(("CV_CAP_PROP_FRAME_WIDTH     %.10f\n",device.get(CV_CAP_PROP_FRAME_WIDTH   )));
	INFOMSG(("CV_CAP_PROP_FRAME_HEIGHT    %.10f\n",device.get(CV_CAP_PROP_FRAME_HEIGHT  )));
	INFOMSG(("CV_CAP_PROP_FPS             %.4f\n",device.get(CV_CAP_PROP_FPS           )));
	INFOMSG(("CV_CAP_PROP_FOURCC          %.4f\n",device.get(CV_CAP_PROP_FOURCC        )));
	INFOMSG(("CV_CAP_PROP_FRAME_COUNT     %.4f\n",device.get(CV_CAP_PROP_FRAME_COUNT   )));
	INFOMSG(("CV_CAP_PROP_FORMAT          %.4f\n",device.get(CV_CAP_PROP_FORMAT        )));
	INFOMSG(("CV_CAP_PROP_MODE            %.4f\n",device.get(CV_CAP_PROP_MODE          )));
	INFOMSG(("CV_CAP_PROP_BRIGHTNESS      %.4f\n",device.get(CV_CAP_PROP_BRIGHTNESS    )));
	INFOMSG(("CV_CAP_PROP_CONTRAST        %.4f\n",device.get(CV_CAP_PROP_CONTRAST      )));
	INFOMSG(("CV_CAP_PROP_SATURATION      %.4f\n",device.get(CV_CAP_PROP_SATURATION    )));
	INFOMSG(("CV_CAP_PROP_HUE             %.4f\n",device.get(CV_CAP_PROP_HUE           )));
	INFOMSG(("CV_CAP_PROP_GAIN            %.4f\n",device.get(CV_CAP_PROP_GAIN          )));
	INFOMSG(("CV_CAP_PROP_EXPOSURE        %.4f\n",device.get(CV_CAP_PROP_EXPOSURE      )));
	INFOMSG(("CV_CAP_PROP_CONVERT_RGB     %.4f\n",device.get(CV_CAP_PROP_CONVERT_RGB   )));
	
	return specs;
}
