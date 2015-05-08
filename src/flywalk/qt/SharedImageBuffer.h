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
#ifndef SHAREDIMAGEBUFFER_H
#define SHAREDIMAGEBUFFER_H

// Qt
#include <QHash>
#include <QSet>
#include <QWaitCondition>
#include <QMutex>
#include <QQueue>
#include <QSemaphore>
#include <QByteArray>
#include <QDebug>

// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "utils.h"

#define MAX_BUFFER_SIZE 100

using namespace cv;

class SharedImageBuffer
{
    public:
        SharedImageBuffer();
        void start(int id, bool isVideo);
        
        bool isFull(int id);
        int maxSize();
        
        void remove(int id);
        void wakeAll();
        
        void add(int idx, Mat &imageBuffer);
        Mat get(int idx);
        bool hasNew(int idx);
        int getSize(int idx);
        
        void clear(int idx);
    private:
        QWaitCondition wc;
        QMutex mutex;
        
        QMap <int,bool> stop;
        QMap <int,bool> isVideo;
        
        QMap <int,QVector<Mat> > ImgBuffer;
};

#endif // SHAREDIMAGEBUFFER_H
