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
#ifndef __INCLUDE_FPS
#define __INCLUDE_FPS

#include <iostream>

/**
clsFPS pfps = clsFPS();
pfps.resetCount();
printf(" >> %.2f << \n",pfps.updateFPS());
**/
class clsFPS{

public:
	clsFPS();
	~clsFPS();
	
	void resetCount();
	double getCurrentFPS();
	double updateFPS();
private:
	time_t start, end;
	double fps,sec;
	long int counter;
};

#endif
