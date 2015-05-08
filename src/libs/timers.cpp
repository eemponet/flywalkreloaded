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
#include "timers.h"

clsTimers::clsTimers()
{
	timeElapsedMilis = 0;
	stopped = true;
	isMicro = false;
}


clsTimers::~clsTimers()
{
	
}

void clsTimers::setUseMicro()
{
        isMicro = true;
}
void clsTimers::reset()
{
	timeElapsedMilis = 0;
	stopped = true;
}

void clsTimers::start()
{
        
        timeStartMilis = getCurrTime();
	
	stopped = false;
}

void clsTimers::stop()
{
	if(!stopped){
		timeElapsedMilis = getElapsedTime();
	}else
	{
		timeElapsedMilis = getElapsedTime();
	}
	stopped = true;
}

unsigned long long clsTimers::getElapsedTime()
{
	unsigned long long elapsedTimeMilis;
	
	
	elapsedTimeMilis = getCurrTime()-timeStartMilis;
        
	if(stopped)
	{
		elapsedTimeMilis = 0;
	}
	
	return elapsedTimeMilis+timeElapsedMilis;

}

unsigned long long clsTimers::getCurrTime()
{
        struct timeval  tv;
	struct timezone tz;
	// struct tm      *tm;
 
	gettimeofday(&tv, &tz);
	// tm = localtime(&tv.tv_sec);
	
	// return tm->tm_yday * 24 * 3600 * 1000 + tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 + tm->tm_sec * 1000 + tv.tv_usec / 1000;
	
	if(isMicro){
	        return 1000000 * tv.tv_sec + tv.tv_usec;
	}
	
	return (1000000 * tv.tv_sec + tv.tv_usec) / 1000;
	
}

void clsTimers::getElapsedTime(char *msg)
{
	unsigned long long elapssd = 0;
	elapssd = getElapsedTime();
	
	int minutesFromSecs = (elapssd/1000)/60;
	int secsFromMinutes = (elapssd/1000) % 100;
	
	snprintf(msg,128,"%02d:%02d:%03lld",minutesFromSecs,secsFromMinutes,elapssd%1000);
}

char *clsTimers::getElapsedTimeStr()
{
	unsigned long long elapssd = 0;
	elapssd = getElapsedTime();
	
	int minutesFromSecs = (elapssd/1000)/60;
	int secsFromMinutes = (elapssd/1000) % 100;
	
	char msg[128];
	snprintf(msg,128,"%02d:%02d:%03lld",minutesFromSecs,secsFromMinutes,elapssd%1000);
	
	return msg;
}

bool clsTimers::isRunning()
{
	return !stopped;
}
