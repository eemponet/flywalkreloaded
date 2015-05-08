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
PROJECT( flywalk )
FIND_PACKAGE(OpenCV)
FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(Qt4 REQUIRED)
#FIND_PACKAGE(OpenGL)

INCLUDE(${QT_USE_FILE})

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_DEFINITIONS(-DFLYWALK_VERSION="Flywalk Reloaded v1.0")

SET(QT_USE_QTOPENGL TRUE)
SET(flywalk_SOURCES main.cpp qt/Settings.cpp qt/MainWindow.cpp qt/CameraView.cpp qt/CaptureThread.cpp qt/FrameLabel.cpp  qt/MatToQImage.cpp qt/ProcessingThread.cpp qt/SharedImageBuffer.cpp)
SET(flywalk_HEADERS qt/MainWindow.h qt/Config.h qt/MatToQImage.h qt/FrameLabel.h qt/Structures.h qt/CameraView.h qt/ProcessingThread.h qt/CaptureThread.h qt/SharedImageBuffer.h)
SET(flywalk_FORMS qt/MainWindow.ui qt/CameraView.ui )
SET(flywalk_RCCS resources.qrc)

SET( CMAKE_CXX_FLAGS "-std=c++11" )

QT4_ADD_RESOURCES(flywalk_RESOURCES ${flywalk_RCCS} )
QT4_WRAP_CPP(flywalk_HEADERS_MOC ${flywalk_HEADERS})
QT4_WRAP_UI(flywalk_FORMS_HEADERS ${flywalk_FORMS})

#INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIRS} )

INCLUDE_DIRECTORIES(${LIBS_SOURCE_DIR}../libs)
INCLUDE_DIRECTORIES(${LIBS_SOURCE_DIR}../remotecontrol)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("qt/")

#install (TARGETS yarptests DESTINATION bin)

ADD_EXECUTABLE(flywalk ${flywalk_SOURCES} ${flywalk_HEADERS_MOC} ${flywalk_FORMS_HEADERS}  ${flywalk_RESOURCES} )
TARGET_LINK_LIBRARIES(flywalk ${OpenCV_LIBS} ${QT_LIBRARIES} camerav4l tracking2d yarp)

install (TARGETS flywalk DESTINATION bin)
#file(COPY ${CMAKE_SOURCE_DIR}/misc/eep.png DESTINATION bin)
