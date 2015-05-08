/*#    This file is part of Flywalk Reloaded. <http://flywalk.eempo.net>
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
#include <iostream>
#include <string>
#include <stdio.h>
#include <map>
#include <vector> 

#define SETTINGS_FILE "settings.ini"
#define SETTINGS_PROC_FILE "settings_proc.ini"

void saveSetting(std::string name,int cfgId, std::string val);
void saveSetting(std::string name,int cfgId, int val);

int loadCameraId(std::string cameraName);
bool loadSetting(std::string name,int cfgId, int &val);
int loadSetting(std::string name,int cfgId, std::string val);

void saveProcessingSettings(std::string filename,std::map<std::string,int>  processingSettings);
std::map<std::string,int>  loadProcessingSettings(std::string filename);

int getNumberCfgCams();

int loadLinesConfiguration(std::vector<int> &heightSeparate);
