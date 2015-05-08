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

#define MAXFILENAME_DATALOGGER 128
class clsDatalogger
{
public:
	clsDatalogger();

	bool addNewLine(pts3D point, int timestamp);
	bool addNewLine(char *line);
	
	bool addHeadline(char *line);
	
	void record(char *filename);
	void stop();

private:
	bool createNewFile(char *filename);
	bool addHeadline();
	
	bool closeFile();
	
	
	
	bool isRecording;
	char filename[MAXFILENAME_DATALOGGER];
	FILE *dataloggerFile;
	
	char dateTime[30];
};
