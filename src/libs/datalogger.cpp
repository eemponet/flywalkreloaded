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
#include "datalogger.h"

clsDatalogger::clsDatalogger()
{
	isRecording = false;
}



bool clsDatalogger::createNewFile(char *filename)
{
	struct timeval tv;
	time_t curtime;
	gettimeofday(&tv, NULL); 
	curtime=tv.tv_sec;
	
	strftime(dateTime,30,"%d%m%y",localtime(&curtime)); //this is only for the lines
	
	dataloggerFile = fopen(filename,"w");
	
	INFOMSG(("new file created %s",filename));
	return true;
}
bool clsDatalogger::closeFile()
{
	return true;
}
bool clsDatalogger::addNewLine(pts3D point3D, int timestamp)
{
	if(isRecording && dataloggerFile != NULL)
	{
		//fprintf(dataloggerFile,"%s,%d,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d\n",dateTime,timestamp,point3D.timestampXYZ,point3D.xCm,point3D.yCm,point3D.zCm,point3D.timestampXY,point3D.timestampXZ,point3D.xPx,point3D.yPx,point3D.zPx);
		//fprintf(dataloggerFile,"%s,%d,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d\n",dateTime,timestamp,point3D.timestampXYZ,point3D.xCm,point3D.yCm,point3D.zCm,point3D.timestampXY,point3D.timestampXZ,point3D.xPx,point3D.yPx,point3D.zPx);
		return true;
	}
	
	return false;
}

bool clsDatalogger::addNewLine(char *line)
{
	if(dataloggerFile != NULL)
	{
		fprintf(dataloggerFile,"%s\n",line);
		return true;
	}
	
	return false;
}

void clsDatalogger::record(char *filename)
{
	createNewFile(filename);
	isRecording = true;
}
void clsDatalogger::stop()
{
	INFOMSG(("Stopping datalogger"));
	if(isRecording)
	{
		if(dataloggerFile != NULL)
		{
			fclose(dataloggerFile);
			dataloggerFile = NULL;
		}
	}
	isRecording = false;
	
}

bool clsDatalogger::addHeadline()
{
	fprintf(dataloggerFile,"daymonthyear,timestampXYZ,x,y,z,timestampXY,timestampZ,xPixels,yPixels,zPixels\n");
	
	return true;
}

bool clsDatalogger::addHeadline(char *line)
{
	fprintf(dataloggerFile,"%s\n",line);
	
	return true;
}
