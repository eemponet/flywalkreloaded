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
#define DEBUG_ON

#include "datalogger.h"
#include "utils.h"
#include "yarpports.h"

int main(int argc, char *argv[])
{
        
        clsYarp pYarpDataloggerCtrl;
        clsYarp pYarpTrackingData;
        
        // if(!pYarpDataloggerCtrl.initPortRcv(YARPPORT_RCV_DATALOGGERCTRL,YARPPORT_SEND_DATALOGGERCTRL,"tcp"))
        // {
                // ERRMSG(("Yarp or flywalk is not running?"));
        // }
	if(!pYarpTrackingData.initPortRcv(YARPPORT_RCV_DATALOGGER,YARPPORT_SEND_DATALOGGER,"udp"))
	{
	        ERRMSG(("Yarp or flywalk is not running?"));
	}
	
	pthread_create(&threadLabViewListener,NULL,labViewListener,NULL);
	pthread_create(&threadOdorsListener,NULL,odorsListener,NULL);
	
	bool recordingLabView = false;
	bool startExperiment = false;
	bool stopExperiment = false;
	if(argc > 1)
        {
                recordingLabView = true;
        }
	clsDataOrganizer pDataOrganizer;
	
	std::string stringCtrl;
	std::string stringParticles;
	std::string stringOdors;
	std::string timestamp;
	std::string filename;
	
	std::ofstream experimentFile;
	
	bool resetTimers = true;
	int frameCount = 0;
	int maximumLong = -1;
	clsTimers fpsTimer;
	fpsTimer.setUseMicro();
	fpsTimer.reset();
	fpsTimer.start();
	
	unsigned long frameRateFactor = ((double)1000.0/(double)RECORD_FRAMERATE)*1000;
	
	boost::filesystem::create_directories(DATA_FOLDER);
	
	bool headerCsvInserted = false;
	
	int checkConnection = CHECK_CONNECTION_ITR;
	unsigned long experimentTimestamp;
	
	while(!bExit)
	{
	        frameCount++;
	        fixFrameRate(fpsTimer.getElapsedTime(),frameCount*frameRateFactor);
	        
	        INFOMSG(("%d",frameCount));
	        INFOMSG(("%ld",fpsTimer.getElapsedTime()));
	        INFOMSG(("%ld",experimentTimestamp));
	        
	        if(frameCount*frameRateFactor > maximumLong - 100000)
	        {
	                WARNMSG(("resetting timers... overflow frameCount %d",frameCount));
	                resetTimers = true;
	        }
	        if(startExperiment || resetTimers)
	        {
	                WARNMSG(("reaching the end of timers count... resetting timers"));
	                fpsTimer.reset();
	                fpsTimer.start();
	                frameCount = 0;
	                resetTimers = false;
	        }
	        
	        experimentTimestamp = (unsigned long ) ((double)fpsTimer.getElapsedTime() /(double) 1000.0);
	        timestamp = to_iso_string(boost::posix_time::microsec_clock::local_time());
	        
	        if(checkConnection > 0)
	        {
	                checkConnection--;
	        }else
	        {
	                if(!pYarpTrackingData.checkConnection(YARPPORT_SEND_DATALOGGER,YARPPORT_RCV_DATALOGGER))
	                {
	                        WARNMSG(("tracking system not found... is FlyWalk running?"));
	                        continue;
	                }
	                checkConnection = CHECK_CONNECTION_ITR;
	        }
	        
	        if(startExperiment)
	        {
	                std::string timeFilename;
	                timeFilename = to_iso_string(boost::posix_time::microsec_clock::local_time());
	                timeFilename.resize(15);
	                
	                filename = DATA_FOLDER+timeFilename+".csv";
	                experimentFile.open(filename.c_str());
	                
	                INFOMSG(("created new file %s ...",filename.c_str()));
	                headerCsvInserted = false;
	                
	                startExperiment = false;
	        }
	        
	        if(recordingLabView)
	        {
	                pYarpTrackingData.rcv(&stringParticles,false);
	                std::ostringstream lineStream;
	                
	                if(!headerCsvInserted)
	                {
	                        std::ostringstream headerStream;
	                        int nLabViewStrings = std::count(stringOdors.begin(),stringOdors.end(), ';');
	                        headerStream << "systemTime;experimentTime;";
	                        for(int i= 0;i<=nLabViewStrings;i++)
	                        {
	                                headerStream << "labView" << i << ";"; 
	                        }
	                        
	                        int nParticlesStrings = std::count(stringParticles.begin(),stringParticles.end(), ';');
	                        nParticlesStrings = nParticlesStrings / 6;
	                        
	                        for(int i= 0;i<nParticlesStrings;i++)
	                        {
	                                headerStream << "xPixels" << i+1 << ";yPixels" << i+1 << ";xCm" << i+1 << ";yCm" << i+1 << ";area" << i+1 << ";"  << "timeNotFound" << i+1 << ";";
	                        }
	                        headerStream << endl;
	                        experimentFile <<  headerStream.str();
	                        headerCsvInserted = true;
	                }
	                
	                lineStream << timestamp << ";" << experimentTimestamp << ";" << stringOdors << ";" << stringParticles << endl;
	                
	                std::string lineString = lineStream.str();
	                INFOMSG(("%s",lineString.c_str()));
	                
	                experimentFile <<  lineStream.str();
	        }
	        
	        if(stopExperiment)
	        {
	                INFOMSG(("closing file %s ...",filename.c_str()));
	                experimentFile.close();
	                stopExperiment = false;
	        }
	        
	        
	        if(newMessageLabViewListener)
	        {
	                INFOMSG(("rcvd: %s",strLabViewListener.c_str()));
	                pthread_mutex_lock(&mutexLabViewListener);
	                
	                if(strLabViewListener == LABVIEW_STARTRECORDSTR )
	                {
	                        
	                        startExperiment = true;
	                        recordingLabView = true;
	                }else if(strLabViewListener == LABVIEW_STOPRECORDSTR)
	                {
	                        stopExperiment = true;
	                        recordingLabView = false;
	                }
	                
	                newMessageLabViewListener = false;
	                
	                pthread_mutex_unlock(&mutexLabViewListener);
	        }
	        
	        if(newMessageOdorListener)
	        {
	                INFOMSG(("rcvd: %s",strOdorListener.c_str()));
	                
	                pthread_mutex_lock(&mutexOdorsListener);
	                
	                stringOdors = strOdorListener;
	                
	                newMessageOdorListener = false;
	                
	                pthread_mutex_unlock(&mutexOdorsListener);
	        }
	        
	}
	
	return 0;
}

void fixFrameRate(unsigned long measuredTimeElapsed, unsigned long calculatedTimeElapsed)
{
        
        unsigned long timeToSleep = calculatedTimeElapsed-measuredTimeElapsed;
        if(timeToSleep < 0)
        {
                return;
                
        }
        
        usleep(timeToSleep);
        
        return;
}

void *labViewListener(void *)
{
        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct sockaddr_in remaddr;
        socklen_t addrlen = sizeof(remaddr);
        int recvlen;
        
        char buf[BUFSIZE], bufprev[BUFSIZE] = {0};
        
        portno = PORT_STARTING;
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
                ERRMSG(("ERROR opening socket"));
                return 0;
        }
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		ERRMSG(("bind failed"));
		return 0;
	}
	
	while(!bExit){
		// DEBUGMSG(("waiting on port %d\n", PORT_STARTING));
		recvlen = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		// DEBUGMSG(("received %d bytes\n", recvlen));
		if (recvlen > 0) {
			buf[recvlen] = 0;
			int compare = strcmp(bufprev,buf);
			// DEBUGMSG(("received message: \"%s\"\n", buf));
			// DEBUGMSG(("%s ? %s = %d",buf,bufprev,compare));
			if(compare != 0)
			{
			        
			        pthread_mutex_lock(&mutexLabViewListener);
			        newMessageLabViewListener = true;
			        strLabViewListener.clear();
			        strLabViewListener.append((const char*)buf);
			        pthread_mutex_unlock(&mutexLabViewListener);
			        
			        strcpy(bufprev,buf);
			}
		}
	}
	
	return 0;
}


void *odorsListener(void *)
{
        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct sockaddr_in remaddr;
        socklen_t addrlen = sizeof(remaddr);
        int recvlen;
        
        char buf[BUFSIZE], bufprev[BUFSIZE] = {0};
        
        portno = PORT_ODORS;
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
                ERRMSG(("ERROR opening socket"));
                return 0;
        }
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		ERRMSG(("bind failed"));
		return 0;
	}
	
	strOdorListener = "0 0 0 0 0 0 0 0";
	newMessageOdorListener = true;
	while(!bExit){
		// DEBUGMSG(("waiting on port %d\n", PORT_ODORS));
		recvlen = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		// DEBUGMSG(("received %d bytes\n", recvlen));
		if (recvlen > 0) {
			buf[recvlen] = 0;
			// DEBUGMSG(("received message: \"%s\"\n", buf));
			if (recvlen > 0) {
			        buf[recvlen] = 0;
			        int compare = strcmp(bufprev,buf);
			        // DEBUGMSG(("received message: \"%s\"\n", buf));
			        // DEBUGMSG(("%s ? %s = %d",buf,bufprev,compare));
			        if(compare != 0)
			        {
			                
			                pthread_mutex_lock(&mutexOdorsListener);
			                newMessageOdorListener = true;
			                strOdorListener.clear();
			                strOdorListener.append((const char*)buf);
			                pthread_mutex_unlock(&mutexOdorsListener);
			                
			                strcpy(bufprev,buf);
			        }
			}
		}
	}
	
	return 0;
}

bool clsDataOrganizer::startRecordingNewExperiment(){
        
        INFOMSG(("starting to record a new experiment"));
        return true;
        //create new dir and ventile file
        
        std::string dateString = getSystemDate();
        
        experimentDataFolder = DATA_FOLDER+getSystemDate();
        ventileFilename = experimentDataFolder+"/"+dateString+".ventile";
        
        INFOMSG(("created folder: %s",experimentDataFolder.c_str()));
        boost::filesystem::create_directories(experimentDataFolder);
        
        pYarpTrackingData.initPortRcv(YARPPORT_RCV_DATALOGGER,YARPPORT_SEND_DATALOGGER,"udp");
        
        return true; 
}
bool clsDataOrganizer::stopRecordingExperiment()
{
        INFOMSG(("stoping to record a new experiment"));
        return true;
        
        //dunno ?
        timerTimestamp.stop();
        timerTimestamp.reset();
        
        experimentDataFolder.clear();
        ventileFilename.clear();
        
        return true; 
}

bool clsDataOrganizer::startRecordingNewStimuli()
{
        INFOMSG(("starting new stimuli"));
        return true;
        
        //add new line to ventile file, and add new folder for stimuli
        trackingDataFolder = experimentDataFolder+getSystemDate();
        INFOMSG(("created folder: %s",trackingDataFolder.c_str()));
        boost::filesystem::create_directories(trackingDataFolder);
        
        
        return true; 
}
bool clsDataOrganizer::stopRecordingStimuli()
{
        INFOMSG(("stopping new stimuli"));
        return true;
        //close all files
        // for (std::map<int,std::ofstream>::iterator iter = particlesFilesDants.begin();iter != particlesFilesDants.end(); ++iter) {
        //         iter->second.close();
        // }
        // for (std::map<int,std::ofstream>::iterator iter = particlesFilesAnt.begin();iter != particlesFilesAnt.end(); ++iter) {
        //         iter->second.close();
        // }
        
        // clear tracking data folder
        trackingDataFolder.clear();
        return true; 
}
bool clsDataOrganizer::insertNewStimuli(std::string ventileLine)
{
        INFOMSG(("insert new stimuli"));
        return true;
        
        //add new line to ventile file
        std::ofstream ventileFile(ventileFilename.c_str());
        ventileFile << getSystemTimeMicro() << " " << ventileLine << std::endl;
        ventileFile.close();
        
        return true; 
}



bool clsDataOrganizer::insertNewTrackingData()
{ 
        INFOMSG(("insert new tracking data"));
        return true;
        //create folder for each item... add tracking to each item!
        //http://stackoverflow.com/questions/12394472/serializing-and-deserializing-json-with-boost
        std::string stringParticles;
        pYarpTrackingData.rcv(&stringParticles,false);
        
        /*
        std::vector<clsParticle> currentParticles;
        pYarpTrackingData.rcv(&currentParticles);
        
        // std::vector<clsParticle> lastParticles = particles;
        // particles.clear();
        for(unsigned int j = 0; j < particles.size(); j++)
        {
                particles[j].timenotfound = 2;
        }
        
        for(unsigned int i = 0; i < currentParticles.size(); i++)
        {
                bool foundParticle = false;
                for(unsigned int j = 0; j < particles.size(); j++)
                {
                        if(particles[j].id == currentParticles[i].id)
                        {
                                particles[j].timenotfound = 0;
                                particles[j].addPosition(currentParticles[i]);
                                foundParticle = true;
                                break;
                        }
                }
                if(!foundParticle)
                {
                        std::string particleFolder;
                        std::stringstream tmpstream;
                        tmpstream << trackingDataFolder << "/Item_" << currentParticles[i].id << "_" << getSystemTimeMicro();
                        particleFolder = tmpstream.str();
                        
                        
                        INFOMSG(("created folder: %s",particleFolder.c_str()));
                        boost::filesystem::create_directories(particleFolder);
                        
                        particles.back().setFolder(particleFolder);
                        
                        std::string particleFileDants = particleFolder+FILE_DANTS;
                        // std::string particleFileAnt = particleFolder+FILE_ANT;
                        dantsFilenames[particles.back().id] = particleFolder+FILE_DANTS;
                        
                        // particlesFilesDants[particles.back().id] = std::ofstream(particleFileDants.c_str());
                        // particlesFilesAnt[particles.back().id].open(particleFileAnt.c_str(), std::ofstream::out | std::ofstream::trunc);
                        // std::ofstream &t = particlesFilesAnt[0];
                        // for (std::map<int,std::ofstream>::iterator iter = particlesFilesAnt.begin();iter != particlesFilesAnt.end(); ++iter) {
                                // iter->second.close();
                        // }
                        // particlesFilesAnt[0].open("ola.txt",std::ofstream::out | std::ofstream::trunc);
                        
                        // t.open(particleFileAnt.c_str(), std::ofstream::out | std::ofstream::trunc);
                        
                        // particlesFilesDants[particles.back().id] << "0";
                        // particlesFilesAnt[particles.back().id] << "0"; //valor abitrario.. nao sei o que eh... ?
                }
        }
        
        std::string tmpstr;
        std::stringstream tmpstream;
        
        for(unsigned int j = 0; j < particles.size(); j++)
        {
                
                tmpstream  << getSystemTimeMicro() << " " << (int) particles[j].x << " " << (int) particles[j].y << " 0 0";
                tmpstr = tmpstream.str();
                dantsBuffer[particles[j].id].push_back(tmpstr);
                
                // if(particles[j].timenotfound > 0)
                // {
                        //means that it cannot be tracked.. should it send the old value? or what?
                // }
        }
        return true; 
        */
}

std::string clsDataOrganizer::getSystemTimeMicro()
{
        return to_iso_string(boost::posix_time::microsec_clock::local_time());
}
std::string clsDataOrganizer::getSystemDate()
{
        std::string myString;
        myString = to_iso_string(boost::posix_time::microsec_clock::local_time());
        myString.resize(15);
        return myString;
}
