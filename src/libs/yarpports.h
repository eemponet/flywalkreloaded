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
#ifndef YARPPORTS_H
#define YARPPORTS_H

#define YARPPORT_IMGCALIB_RECVR "/calibrationcamera/IMG"
#define YARPPORT_IMGCALIB_SENDER "/2Dtrack/calibIMG/%d/%d"

#define YARPPORT_IMG_SENDER "/2D_tracking_IMG/%d%d"
#define YARPPORT_IMG_TRACING_SENDER "/2D_tracking_IMG_TRACING/%d%d"

#define YARPPORT_IMG_SENDER_00 "/2D_tracking_IMG/00"
#define YARPPORT_IMG_SENDER_01 "/2D_tracking_IMG/01"
#define YARPPORT_IMG_SENDER_10 "/2D_tracking_IMG/10"
#define YARPPORT_IMG_SENDER_11 "/2D_tracking_IMG/11"

#define YARPPORT_IMG_RECVR_CALIBRATE "/2D_tracking_IMG/calibrate"
#define YARPPORT_IMG_RECVR_CALIBRATE_3D "/calibrate3d/cameraStreamIn/%d%d"

#define YARPPORT_IMG_RECVR "/3dreconstr/camerasImgs"
#define YARPPORT_IMG_RECVR_RECCAM "/2D_tracking_IMG/inREC/%d%d"
#define YARPPORT_IMG_RECVR_RECCAM2 "/2D_tracking_IMG/inREC2/%d%d"
#define YARPPORT_IMG_RECVR_STREAMCAM "/2D_tracking_IMG/inSTREAM/%d%d"

#define YARPPORT_2DPTS_RECVR "/2DPointsStream/in"

#define YARPPORT_3DPTS_SEND "/3DPointsStream/out"
#define YARPPORT_3DPTS_RECVR "/3DPointsStream/in"
#define YARPPORT_3DPTS_RECVR_PLOT "/3DPointsStream/inPLot"

#define YARPPORT_SEND_MILISECS "/3dctrlGTK/timestamp"

#define YARPPORT_3DCTRLGTK_SEND_CENTRALCMD "/3dctrlGTK/centralcmd"
#define YARPPORT_2DTRACK_RECVR_CENTRALCMD "/2dtrack/centralcmd/%d%d"

#define YARPPORT_3DCTRLGTK_SEND_TIMESTAMP "/3dctrlGTK/timestamp"
#define YARPPORT_TARGET_RECVR_TIMESTAMP "/target/timestamp/%d"

#define YARPPORT_SEND_DATALOGGER_QT "/flywalk/datalogger"
#define YARPPORT_RCV_DATALOGGER_QT "/datalogger/rcv"

#define YARPPORT_REMOTE_CTRL "/remotecontrol"
#define YARPPORT_REMOTE_CTRL_RCVR "/flywalk/remotecontrol"

#define YARPPORT_SEND_DATALOGGER "/flywalk/datalogger/send"
#define YARPPORT_RCV_DATALOGGER "/datalogger/rcv"

#define YARPPORT_SEND_DATALOGGERCTRL "/flywalk/dataloggerctrl/send"
#define YARPPORT_RCV_DATALOGGERCTRL "/dataloggerctrl/rcv"

#endif
