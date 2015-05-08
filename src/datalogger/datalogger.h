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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

#include <iostream>
#include <fstream>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>



#define REQUIRE_YARP
#define DATALOGGER

#define YARP_USE_PARTICLES

#include "utils.h"
#include "imgproc/particles.h"
#include "yarp.h"
#include "timers.h"

#define PORT_STARTING 58431
#define PORT_ODORS 58432
#define HOSTNAME "127.0.0.1"
#define BUFSIZE 256

#define FILE_DANTS "trace.dants"
#define FILE_ANT "trace.ant"
#define MAX_FILES 256

#define LABVIEW_STARTRECORDSTR "REC"
#define LABVIEW_STOPRECORDSTR "Stop"
// using namespace std;
#define RECORD_FRAMERATE 20
#define DATA_FOLDER "flywalk_data/"
#define CHECK_CONNECTION_ITR 50
// using namespace std; 

bool bExit = false;

pthread_t threadLabViewListener;
pthread_t threadOdorsListener;

pthread_mutex_t mutexLabViewListener = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexOdorsListener = PTHREAD_MUTEX_INITIALIZER;

void *labViewListener(void *);
void *odorsListener(void *);

std::string strLabViewListener;
std::string strOdorListener;

bool newMessageLabViewListener;
bool newMessageOdorListener;

void fixFrameRate(unsigned long measuredTimeElapsed, unsigned long calculatedTimeElapsed);

class clsDataOrganizer
{
        
public:
        bool startRecordingNewStimuli();
        bool stopRecordingStimuli();
        bool insertNewStimuli(std::string ventileLine);
        
        bool startRecordingNewExperiment();
        bool stopRecordingExperiment();
        
        bool insertNewTrackingData();
        
        std::string getSystemTimeMicro();
        std::string getSystemDate();
        
        // std::map
        
        std::string ventileFilename;
        std::string experimentDataFolder;
        std::string trackingDataFolder;
        
        std::ofstream ventileFile;
        std::ofstream experimentFile;
private:
        clsTimers timerTimestamp;
        
        clsYarp pYarpTrackingData;
        std::vector<clsParticle> particles;
        
        std::ofstream particlesFilesDants[MAX_FILES];
        std::ofstream particlesFilesAnt[MAX_FILES];
        // std::map<int,std::ofstream> particlesFilesDants;
        // std::map<int,std::ofstream> particlesFilesAnt;
        
        std::map<int,vector<std::string> > dantsBuffer;
        std::map<int,std::string> dantsFilenames;
};
