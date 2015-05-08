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
#include "videoprocAR.h"
#include "utils.h"

clsVideoAR::clsVideoAR()
{
	
}

clsVideoAR::~clsVideoAR()
{
	
}
int clsVideoAR::detectMarker(IplImage *imagem)
{
	ARUint8 *dataPtr; // unsigned chars, in format ARGB
	
	dataPtr = (ARUint8 *)imagem->imageData;
	
	int thresh = 100;
	ARMarkerInfo    *marker_info;
	int             marker_num;
	
	arDetectMarker(dataPtr, thresh, &marker_info, &marker_num);
	
	INFOMSG(("found %d markers",marker_num));
}

// static int setupMarker(const char *patt_name, int *patt_id)
// {
// 	// Loading only 1 pattern in this example.
// 	if ((*patt_id = arLoadPatt(patt_name)) < 0) {
// 		fprintf(stderr, "setupMarker(): pattern load error !!\n");
// 		return (FALSE);
// 	}

// 	return (TRUE);
// }
