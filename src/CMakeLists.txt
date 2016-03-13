#    
#    This file is part of Flywalk Reloaded. <http://flywalk.eempo.net>
#
#    flywalk@eempo.net - Pedro Gouveia
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

## SET TO INSTALL IT: CMAKE_INSTALL_PREFIX
## TYPE OF BUILD: CMAKE_BUILD_TYPE
## CHANGE BTWEEN SHARED AND NOT SHARED LIBS.. interesting! :) BUILD_SHARED_LIBS

PROJECT( flywalkreloaded )
cmake_minimum_required(VERSION 2.6)

set( BUILD_SHARED_LIBS  )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

ADD_DEFINITIONS( -Wall -g )

ADD_SUBDIRECTORY( libs )
ADD_SUBDIRECTORY( libs/imgproc )

ADD_SUBDIRECTORY( datalogger )
ADD_SUBDIRECTORY( oscsender )

ADD_SUBDIRECTORY( flywalk )

