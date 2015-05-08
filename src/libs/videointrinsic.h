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

class clsVideoIntrinsic : public clsVideo {

public: 
	clsVideoIntrinsic();
	~clsVideoIntrinsic();
	
	
	bool startUndistortedImage(int section,int position);
	bool showUndistortImage();
	
	bool startCalibrateIntrisicParameters();
	bool calibrateIntrisicParameters();
	bool finishCalibrateIntrisicParameters(int cameraSection, int cameraPosition);
	
private:
	
	IplImage* mapx;
	IplImage* mapy;
	
	//INTRINSIC PARAMETERES INCLUDED DISTORTION VARIABLES!!!
	CvMat *persChessboardPoints;
	IplImage *persFrameGray;
	CvPoint2D32f* perCurrentCorners;
	bool persAddNewSample;
	int perSampleNr;
};
