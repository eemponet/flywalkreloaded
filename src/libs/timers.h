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

#ifndef __INCLUDE_TIMERS
#define __INCLUDE_TIMERS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

class clsTimers{

public:
	clsTimers();
	~clsTimers();
	
	void reset();
	void start();
	void stop();
	
	unsigned long long getElapsedTime();
	void getElapsedTime(char *msg);
	char *getElapsedTimeStr();
	// char *getCurrentTimeC();
	bool isRunning();
	void setUseMicro();	
	
private:

	unsigned long long timeStartMilis;
	unsigned long long timeElapsedMilis;
	
	bool stopped;
	bool isMicro; 
	
	unsigned long long getCurrTime();
};

#endif
