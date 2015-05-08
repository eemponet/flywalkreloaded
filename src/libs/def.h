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
#define SET_DEBUG_MODE
#define DO_NOT_USE_INTRINSIC

#define CAMERA_WIDTH 800
#define CAMERA_HEIGHT 600

#define MAX_NUM_POINTS 1
#define MEMLEAKCHECK
#define FRAME_DROP_TO_PRINTF 20

#define FRAME_SLEEP 20

#define FINAL_FPS 30

#define DATACSV_FPS 30

#define TRAIL_ON

#define MAX_DISTANCE 15

// #define PRINTF printf(

#define CHESSBOARD_X 10
#define CHESSBOARD_Y 7
#define CHESSBOARD_N CHESSBOARD_X*CHESSBOARD_Y
#define SAMPLES_N    10
#define SAMPLES_3DCALIBRATION_N 10
#define CHESSBOARD_SQ_SIZE 25

#define INTRISIC_MATRICES_NAME "matrices/intrisic"
#define DISTORTION_MATRICES_NAME "matrices/distortion"
#define LINES_MATRICES_NAME "matrices/lines"
#define PERSPECTIVE_MATRICES_CM "matrices/arena_sizes_cms.xml"
#define PERSPECTIVE_MATRICES_NAME "matrices/perspective"

#define RADIUS_TO_DRAG 10

#define NUMBER_CAMERAS 4

#define MAX_FILENAME_SIZE 128

#define	COLOR_THRESHOLD 0
#define SIMPLE_FRAMESUBSTRACTION 1
#define BGFG_CODEBOOK 2
#define	DO_NOTHING 3

// #define THRESHOLD_TYPE BGFG_CODEBOOK
// // #define THRESHOLD_TYPE COLOR_THRESHOLD

#define FIND_THE_BIGGEST_PARTICLE_STRATEGY
// #define FIND_THE_CLOSEST_PARTICLE_STRATEGY

//reconstruct and save to excel!
 // 1000ms(1s)/30fps = 33.333 :::: 50 fps = 20
#define TIMECYCLE_3D_RECONSTRUCT 100

#define MAX_SIZE_OF_TRAIL_3D_RECONSTR 30
#define MAX_SIZE_OF_TRAIL 50

#define TIMECYCLE_EXCEL 10
#define TIMECYCLE_YARP_2DPT_SENDER 20
#define TIMECYCLE_YARP_IMG_SENDER 100
#define TIMECYCLE_YARP_IMG_RCV 100

#define FRAMES_OUT_OF_TARGET_TO_STOP 50

#define CAMREC_WIDTH 400
#define CAMREC_HEIGHT 300

//#define CAMREC_WIDTH 600
//#define CAMREC_HEIGHT 450

#define _2DTRACK_FPS_YARPSEND_IMG 30

#define GTK3D_VIDEOREC_BUFFERSIZE 500
#define VIDEOREC_2D_MAXBUFFERSIZE 100

#define CSV_SEPARATOR ";"
