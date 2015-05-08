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
#include "Settings.h"
#include "utils.h"

#include <QSettings>
#include <QStringList>

void saveSetting(std::string name,int cfgId, std::string val)
{
	QSettings settings(SETTINGS_FILE, QSettings::NativeFormat);
	
	char tmp[10];
	sprintf(tmp,"device%d",cfgId);
	settings.beginGroup(tmp);
	
	settings.setValue(QString::fromStdString(name),QString::fromStdString(val));
	
	settings.endGroup();
}

void saveSetting(std::string name,int cfgId, int val)
{
	QSettings settings(SETTINGS_FILE, QSettings::NativeFormat);
	
	char tmp[10];
	
	sprintf(tmp,"device%d",cfgId);
	settings.beginGroup(QString::fromLocal8Bit(tmp));
	
	settings.setValue(QString::fromStdString(name),val);
	
	settings.endGroup();
}

int loadCameraId(std::string cameraName)
{
	// char *camAddrTmp;
	QSettings settings(SETTINGS_FILE, QSettings::NativeFormat);
	QStringList listGroups = settings.childGroups();
	foreach (const QString &childKey, listGroups)
 	{
 		settings.beginGroup(childKey);
 		
 		std::string cameraNameToCompare = settings.value("busname").toString().toStdString();
 		
 		if(cameraName.compare(cameraNameToCompare) == 0)
 		{
 			return settings.value("id").toInt();
 		}
 		settings.endGroup();
 		
 	}
 	
 	return -1;
}

bool loadSetting(std::string name,int cfgId, int &val)
{
        bool iRet = false;
        
	QSettings settings(SETTINGS_FILE, QSettings::NativeFormat);
	
	char tmp[10];
	sprintf(tmp,"device%d",cfgId);
	
	settings.beginGroup(QString::fromLocal8Bit(tmp));
	
	if(settings.childKeys().indexOf(QString::fromStdString (name)) >= 0)
	{
	        val = settings.value(QString::fromStdString(name)).toInt();
	        iRet = true;
	}else
	{
	        settings.endGroup();
	        iRet = false;
	}
	settings.endGroup();
	
	return iRet;
}

int loadSetting(std::string name,int cfgId, std::string val)
{
	QSettings settings(SETTINGS_FILE, QSettings::NativeFormat);
	
	char tmp[10];
	sprintf(tmp,"device%d",cfgId);
	settings.beginGroup(QString::fromLocal8Bit(tmp));
	val = settings.value(QString::fromStdString(name)).toString().toLocal8Bit().data() ;
	settings.endGroup();
}

int getNumberCfgCams()
{
	QSettings settings(SETTINGS_FILE, QSettings::NativeFormat);
	QStringList listGroups = settings.childGroups();
	
	int max = 0;
	
	foreach (const QString &childKey, listGroups)
 	{
 	        settings.beginGroup(childKey);
 	        
 		int camId = settings.value("id").toInt();
 		
 		if(camId > max)
 		{
 			max = camId;
 		}
 		
 		settings.endGroup();
 	}
 	
	return max;
}

void saveProcessingSettings(std::string filename,std::map<std::string,int>  processingSettings)
{
	QSettings settings(QString::fromStdString(filename), QSettings::NativeFormat);
	for (std::map<std::string,int>::iterator iter = processingSettings.begin();iter != processingSettings.end(); ++iter) {
		settings.setValue(QString::fromStdString(iter->first),iter->second);
	}
	
}
std::map<std::string,int>  loadProcessingSettings(std::string filename)
{
	std::map<std::string,int> processingSettings;
	
	QSettings settings(QString::fromStdString(filename), QSettings::NativeFormat);
	
	QStringList listKeys = settings.allKeys();
	
	foreach (const QString &childKey, listKeys)
 	{
 		processingSettings[childKey.toStdString().c_str()] = settings.value(childKey).toInt();
 	}
 	
 	return processingSettings;
 	
	// for (std::map<std::string,int>::iterator iter = processingSettings.begin();iter != processingSettings.end(); ++iter) {
	// 	settings.setValue(QString::fromStdString(iter->first),iter->second);
	// }
	
}

int loadLinesConfiguration(std::vector<int> &heightSeparate)
{
	QSettings settings(SETTINGS_PROC_FILE, QSettings::NativeFormat);
	
	settings.beginGroup(QString::fromLocal8Bit("lines"));
	QStringList listKeys = settings.allKeys();
	
	foreach (const QString &childKey, listKeys)
 	{
 		heightSeparate.push_back(settings.value(childKey).toInt());
 	}
	settings.endGroup();
}
