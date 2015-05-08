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

#define REQUIRE_YARP
#define REMOTE_CTRL

#include "remotedefs.h"
#include "utils.h"
#include "yarpports.h"
#include "yarp.h"

#define PORT_STARTING 58433
#define PORT_ODORS 58434
#define HOSTNAME "127.0.0.1"
#define BUFSIZE 256

// using namespace std;

bool bExit = false;

pthread_t threadStartListener;
pthread_t threadOdorsListener;

pthread_mutex_t mutexStartListener = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexOdorsListener = PTHREAD_MUTEX_INITIALIZER;

void *startListener(void *);
void *odorsListener(void *);

std::string strStartListener;
std::string strOdorListener;

bool newMessageStartListener;
bool newMessageOdorListener;
