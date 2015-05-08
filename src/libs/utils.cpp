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
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "../CONFIGS.h"
//camera2DCFG *camera2DCFGs[2][2];

clsTimers timerClock;
bool timerStarted = false;

bool fileexists(char *filename){
	FILE *f;
	f = fopen(filename,"r");
	if(f == NULL)
	{
		ERRMSG(("FILE NOT FOUND: %s",filename));
		return false;
	}
	fclose(f);
	return true;
}

camera2DCFG camera2DCFGsDEF[2][2];

void loadHARDCODEDOPTIONS(camera2DCFG *camera2DCFGs,int section,int position)
{
	
	/*chg_mode = camera2DCFGs[s][p]->type2Dtracking;
	minThrs = camera2DCFGs[s][p]->minThrs;
	erodeThreshold =camera2DCFGs[s][p]->erode;
	sqr_blur =camera2DCFGs[s][p]->sqr_blur;
	*/
	
	/** BGFG_CODEBOOK **/
	
	camera2DCFGsDEF[0][1].type2Dtracking  = DEFAULTMODE;
	camera2DCFGsDEF[0][1].sqr_blur        = TOP_BLUR;
	camera2DCFGsDEF[0][1].minThrs         = TOP_THRESHOLD;
	camera2DCFGsDEF[0][1].erode           = TOP_ERODE;
	
	camera2DCFGsDEF[1][1].type2Dtracking  = DEFAULTMODE;
	camera2DCFGsDEF[1][1].sqr_blur        = TOP_BLUR;
	camera2DCFGsDEF[1][1].minThrs         = TOP_THRESHOLD;
	camera2DCFGsDEF[1][1].erode           = TOP_ERODE;
	
	camera2DCFGsDEF[0][0].type2Dtracking  = DEFAULTMODE;
	camera2DCFGsDEF[0][0].sqr_blur        = SIDE_BLUR;
	camera2DCFGsDEF[0][0].minThrs         = SIDE_THRESHOLD;
	camera2DCFGsDEF[0][0].erode           = SIDE_ERODE;
	
	camera2DCFGsDEF[1][0].type2Dtracking  = DEFAULTMODE;
	camera2DCFGsDEF[1][0].sqr_blur        = SIDE_BLUR;
	camera2DCFGsDEF[1][0].minThrs         = SIDE_THRESHOLD;
	camera2DCFGsDEF[1][0].erode           = SIDE_ERODE;
	
	
	*camera2DCFGs = camera2DCFGsDEF[section][position];
	/*
	camera2DCFGs[0][0]->type2Dtracking  = BGFG_CODEBOOK;
	camera2DCFGs[0][0]->sqr_blur        = 5;
	camera2DCFGs[0][0]->minThrs         = 20;
	camera2DCFGs[0][0]->nframes         = 100;
	camera2DCFGs[0][0]->erode           = 2;
	
	//SECTION 0, CAMERA TOP
	camera2DCFGs[0][1]->type2Dtracking  = BGFG_CODEBOOK;
	
	camera2DCFGs[0][1]->sqr_blur        = 5;
	
	camera2DCFGs[0][1]->minThrs         = 20;
	camera2DCFGs[0][1]->maxThrs         = 255;
	
	camera2DCFGs[0][1]->minH            = 10;
	camera2DCFGs[0][1]->maxH            = 40;
	camera2DCFGs[0][1]->minS            = 0;
	camera2DCFGs[0][1]->maxS            = 193;
	camera2DCFGs[0][1]->minL            = 0;
	camera2DCFGs[0][1]->maxL            = 255;
	
	camera2DCFGs[0][1]->nframes         = 100;
	
	//SECTION 1, CAMERA SIDE
	camera2DCFGs[1][0]->type2Dtracking  = BGFG_CODEBOOK;
	
	camera2DCFGs[1][0]->sqr_blur        = 5;
	
	camera2DCFGs[1][0]->minThrs         = 20;
	camera2DCFGs[1][0]->maxThrs         = 255;
	
	camera2DCFGs[1][0]->minH            = 10;
	camera2DCFGs[1][0]->maxH            = 40;
	camera2DCFGs[1][0]->minS            = 0;
	camera2DCFGs[1][0]->maxS            = 193;
	camera2DCFGs[1][0]->minL            = 0;
	camera2DCFGs[1][0]->maxL            = 255;
	
	camera2DCFGs[1][0]->nframes         = 100;
	
	//SECTION 1, CAMERA TOP
	camera2DCFGs[1][1]->type2Dtracking  = BGFG_CODEBOOK;
	
	camera2DCFGs[1][1]->sqr_blur        = 5;
	
	camera2DCFGs[1][1]->minThrs         = 20;
	camera2DCFGs[1][1]->maxThrs         = 255;
	
	camera2DCFGs[1][1]->minH            = 10;
	camera2DCFGs[1][1]->maxH            = 40;
	camera2DCFGs[1][1]->minS            = 0;
	camera2DCFGs[1][1]->maxS            = 193;
	camera2DCFGs[1][1]->minL            = 0;
	camera2DCFGs[1][1]->maxL            = 255;
	
	camera2DCFGs[1][1]->nframes         = 100;
	*/
	
	// exit(1);
}

// bool mkdir(char *pathname)
// {
// return boost::filesystem::create_directories("/tmp/a/b/c");
// }

bool mkpath( char *path )
{
	int nRC = mkdir( path, 0775 );
	if( nRC == -1 )
	{
		return false;
	}
	return true;
}


int getTimestamp()
{
	timeval time;
	gettimeofday(&time, NULL);
	char cur_time[128];
	strftime(cur_time,30,"%M%S",localtime(&time.tv_sec));
	
	int milisecs = time.tv_usec/1000;
	int secs = atoi(cur_time)%100;
	int minutes = atoi(cur_time)/100;
	
	return milisecs + secs*1000 + minutes*60*1000;
}

int getTimestampMicro()
{
	INFOMSG(("IMPLEMENT ME ... utils.cpp"));
	return 0;
	
        struct timeval tv;
        
        gettimeofday(&tv, NULL); 
        
        int milisecs = tv.tv_usec/1000;
	int minutes = tv.tv_sec/60;
	int secs = tv.tv_sec-(minutes*60);
        int microsecs = tv.tv_usec%100;
	int timeLastUpdateMicro = microsecs + milisecs*100 + secs*1000*100 + minutes*60*1000*100;
	
        return timeLastUpdateMicro;
}
void milisToStr(char *str, int timeInMilis)
{
	
	int minutesFromSecs = (timeInMilis/1000)/60;
	int secsFromMinutes = (timeInMilis/1000)-(minutesFromSecs*60);
	
	sprintf(str,"%02d:%02d:%03d",minutesFromSecs,secsFromMinutes,timeInMilis%1000);
}

#define MEM_FREE "MemFree:" //for getFree
#define MEM_TOTAL "MemTotal:" //for getFree
float getFreeRAM() {
	
	float ret = -1;
	
	char line[128];
	
	FILE *cpufile;
	cpufile = fopen("/proc/meminfo","r");
	if (cpufile == NULL){
		ERRMSG(("error opening file /proc/meminfo"));
		return -1;
	}
	
	while (fgets(line, sizeof (line),cpufile))
	{
		if(strncmp(MEM_FREE,line,strlen(MEM_FREE)) == 0)
		{
			char delims[] = " \r";
			char *result = NULL;
			result = strtok( line, delims );
			result = strtok( NULL, delims );
			
			ret = (float)atoi(result)/1024.0f;
		}
	}
	
	fclose(cpufile);
	
	return ret;
}

float getTotalRAM() {
	
	float ret = -1;
	
	char line[128];
	
	FILE *cpufile;
	cpufile = fopen("/proc/meminfo","r");
	if (cpufile == NULL){
		ERRMSG(("error opening file /proc/meminfo"));
		return -1;
	}
	
	while (fgets(line, sizeof (line),cpufile))
	{
	if(strncmp(MEM_TOTAL,line,strlen(MEM_TOTAL)) == 0)
		{
			char delims[] = " \r";
			char *result = NULL;
			result = strtok( line, delims );
			result = strtok( NULL, delims );
			
			ret = (float)atoi(result)/1024.0f;
		}
	}
	
	fclose(cpufile);
	
	return ret;
}

// #define _GNU_SOURCE          /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>    /* For SYS_xxx definitions */

float getRAMusage()
{
	float ret = -1;
	
	char line[128];
	char procDir[128];
	snprintf(procDir,128,"/proc/%d/statm",getpid());
	
	FILE *cpufile;
	cpufile = fopen(procDir,"r");
	if (cpufile == NULL){
		ERRMSG(("error opening file %s",procDir));
		return -1;
	}
	
	
	while (fgets(line, sizeof (line),cpufile))
	{
		
		char delims[] = " ";
		char *result = NULL;
		result = strtok( line, delims );
		ret = (float)atoi(result)/1024.0f;
	}
	
	fclose(cpufile);
	
	// return -1;
	
	return ret;
}


