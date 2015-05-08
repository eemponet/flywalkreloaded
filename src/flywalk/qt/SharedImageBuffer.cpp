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
#include "SharedImageBuffer.h"
#include <unistd.h> 

SharedImageBuffer::SharedImageBuffer()
{
}

void SharedImageBuffer::start(int id, bool isVideox)
{
        stop[id] = false;
        isVideo[id] = isVideox;
}

void SharedImageBuffer::remove(int idx)
{
        
	mutex.lock();
	ImgBuffer[idx].clear();
	mutex.unlock();
}

void SharedImageBuffer::wakeAll()
{
        INFOMSG(("WAKE ALL"));
        
        QMap<int, bool>::iterator i;
        for (i = stop.begin(); i != stop.end(); ++i)
        {
                stop[i.key()] = true;
        }
        
	QMutexLocker locker(&mutex);
	wc.wakeAll();
}

void SharedImageBuffer::add(int id, Mat& imageBuffer)
{
        if(isVideo[id])
        {
                while(getSize(id) >= maxSize() && !stop[id])
                {
                        usleep(5000);
                }
        }else{
                if(getSize(id) >= maxSize())
                {
                        usleep(50);
                        return;
                }
        }
        mutex.lock();
        ImgBuffer[id].push_back(imageBuffer);
        mutex.unlock();
}
int SharedImageBuffer::getSize(int idx)
{
        mutex.lock();
        int size = ImgBuffer[idx].size();
        mutex.unlock();
        return size;
}
bool SharedImageBuffer::hasNew(int idx)
{
        if(getSize(idx) > 0)
        {
                
                return true;
        }
        
        return false;
}

Mat SharedImageBuffer::get(int idx)
{
        
        Mat k ;
        
        mutex.lock();
        k = ImgBuffer[idx].first();
        ImgBuffer[idx].pop_front();
        mutex.unlock();
        
        return k;
}

void SharedImageBuffer::clear(int idx)
{
        
        mutex.lock();
        ImgBuffer[idx].clear();
        mutex.unlock();
        
}

bool SharedImageBuffer::isFull(int id)
{
        
        if(getSize(id) > MAX_BUFFER_SIZE)
        {
                return true;
        }
        return false;
}
int SharedImageBuffer::maxSize()
{
        return MAX_BUFFER_SIZE;
}
