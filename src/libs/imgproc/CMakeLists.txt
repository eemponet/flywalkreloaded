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

FIND_PACKAGE(OpenCV)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("../")

#set(CMAKE_CXX_FLAGS "-fpermissive")

#ADD_LIBRARY( particles particles )
#TARGET_LINK_LIBRARIES(particles )

ADD_LIBRARY( tracking2d tracking2d videocapture particles)
TARGET_LINK_LIBRARIES(tracking2d utils ${OpenCV_LIBS})
