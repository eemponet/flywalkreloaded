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

FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(OpenCV)
FIND_PACKAGE( Boost REQUIRED COMPONENTS filesystem date_time system regex)

INCLUDE_DIRECTORIES(${LIBS_SOURCE_DIR}../libs)
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )

#-lboost_system -lboost_filesystem -lboost_date_time

ADD_EXECUTABLE(datalogger datalogger)
TARGET_LINK_LIBRARIES(datalogger utils yarp tracking2d ${Boost_LIBRARIES} -lboost_system -lboost_filesystem -lboost_date_time)

install (TARGETS datalogger DESTINATION bin)

