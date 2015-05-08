/*#  This file is part of Flywalk Reloaded.
#
#    Flywalk Reloaded is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Flywalk Reloaded is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Flywalk Reloaded.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "remotecontrol.h"

int main(int argc, char *argv[])
{
        
	pthread_create(&threadStartListener,NULL,startListener,NULL);
	pthread_create(&threadOdorsListener,NULL,odorsListener,NULL);
        
	while(!bExit)
	{
	        if(newMessageStartListener)
	        {
	                
	                pthread_mutex_lock(&mutexStartListener);
	                INFOMSG(("new message received: %s",strStartListener.c_str()));
	                
	                newMessageStartListener = false;
	                strStartListener.clear();
	                pthread_mutex_unlock(&mutexStartListener);
	        }
	        
	        if(newMessageOdorListener)
	        {
	                
	                pthread_mutex_lock(&mutexOdorsListener);
	                INFOMSG(("new message received: %s",strOdorListener.c_str()));
	                
	                newMessageOdorListener = false;
	                strOdorListener.clear();
	                pthread_mutex_unlock(&mutexOdorsListener);
	        }
	}
	
	pthread_join(threadStartListener,NULL);
	pthread_join(threadOdorsListener,NULL);
	
	
        return 0;
        clsYarp pYarpRemoteControl;
        
        if(!pYarpRemoteControl.initPort((char *)YARPPORT_REMOTE_CTRL))
        {
                ERRMSG(("Error on initializing yarp, is yarp running?"));
        }
        pYarpRemoteControl.connectPort((char *)YARPPORT_REMOTE_CTRL,(char *)YARPPORT_REMOTE_CTRL_RCVR,(char *)"tcp");
        
        pYarpRemoteControl.sendCommand("startCam",0);
        
	return 0;
}

void *startListener(void *)
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
		DEBUGMSG(("waiting on port %d\n", PORT_STARTING));
		recvlen = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		DEBUGMSG(("received %d bytes\n", recvlen));
		if (recvlen > 0) {
			buf[recvlen] = 0;
			int compare = strcmp(bufprev,buf);
			DEBUGMSG(("received message: \"%s\"\n", buf));
			DEBUGMSG(("%s ? %s = %d",buf,bufprev,compare));
			if(compare != 0)
			{
			        
			        pthread_mutex_lock(&mutexStartListener);
			        newMessageStartListener = true;
			        strStartListener.clear();
			        strStartListener = strStartListener.append((const char*)buf);
			        pthread_mutex_unlock(&mutexStartListener);
			        
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
	
	while(!bExit){
		DEBUGMSG(("waiting on port %d\n", PORT_ODORS));
		recvlen = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		DEBUGMSG(("received %d bytes\n", recvlen));
		if (recvlen > 0) {
			buf[recvlen] = 0;
			DEBUGMSG(("received message: \"%s\"\n", buf));
			if (recvlen > 0) {
			        buf[recvlen] = 0;
			        int compare = strcmp(bufprev,buf);
			        DEBUGMSG(("received message: \"%s\"\n", buf));
			        DEBUGMSG(("%s ? %s = %d",buf,bufprev,compare));
			        if(compare != 0)
			        {
			                
			                pthread_mutex_lock(&mutexOdorsListener);
			                newMessageOdorListener = true;
			                strOdorListener.clear();
			                strOdorListener = strOdorListener.append((const char*)buf);
			                pthread_mutex_unlock(&mutexOdorsListener);
			                
			                strcpy(bufprev,buf);
			        }
			}
		}
	}
	
	return 0;
}

