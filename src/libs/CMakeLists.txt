#    
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

FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})


FIND_PACKAGE(YARP REQUIRED)

ADD_LIBRARY( utils utils def fps timers datalogger)

ADD_LIBRARY( videoproc videoproc videorecord)
TARGET_LINK_LIBRARIES(videoproc utils yarp ${OpenCV_LIBS})

ADD_LIBRARY( yarp yarp )
TARGET_LINK_LIBRARIES(yarp utils ${YARP_LIBRARIES})

ADD_LIBRARY( videoproc3d videoproc3d )
TARGET_LINK_LIBRARIES(videoproc3d videoproc )

ADD_LIBRARY( videointrinsic videointrinsic )
TARGET_LINK_LIBRARIES(videointrinsic videoproc)

ADD_LIBRARY( camerav4l camerav4l )
TARGET_LINK_LIBRARIES(camerav4l)
