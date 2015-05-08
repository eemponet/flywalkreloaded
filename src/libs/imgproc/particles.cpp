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
#include "particles.h"
#include "utils.h"
#include "def.h"
#include "fps.h"
#include "timers.h"
#include "../../flywalk/qt/Settings.h"

#define YARP_USE_PARTICLES
#include "../yarp.h"

#include <unistd.h>

std::map<std::string,int> paramsInt;
static CvFont particlesFont;

clsParticles::clsParticles()
{
        processParticlesStarted = false;
        frameCount = 0;
        particleId = 0;
        
        
}

clsParticles::~clsParticles()
{
}

void clsParticles::startLines(std::vector<int> linesCfg)
{
	
        heightSeparate = linesCfg;
        if(heightSeparate.size() < 1)
        {
        	heightSeparate.push_back(0);
        	heightSeparate.push_back(5000);
        }
        std::sort(heightSeparate.begin(), heightSeparate.begin()+heightSeparate.size());
        //std::reverse(heightSeparate.begin(), heightSeparate.begin()+heightSeparate.size());
        std::unique(heightSeparate.begin(), heightSeparate.begin()+heightSeparate.size());
}

void clsParticles::reinitialize()
{
        INFOMSG(("clsParticles.. reinitialized"));
        // processParticlesStarted = false;
        particleId = 0;
        frameCount = 0;
        particles.clear();
        limboParticles.clear();
        currentParticles.clear();
        
        processParticlesStarted = false;
        
}
void clsParticles::processManyParticles(IplImage *foregroundImage)
{
        
        if(!processParticlesStarted)
        {
                processParticlesStarted = true;

                cvInitFont(&particlesFont,CV_FONT_HERSHEY_SIMPLEX, 1,1,0,2);

                DEBUGMSG(("Allocating process particles"));

                countorsImage = cvCreateImage( cvSize(foregroundImage->width,foregroundImage->height), IPL_DEPTH_8U, 1 );

        }
        cvCopy(foregroundImage,countorsImage);
        
        findNewParticles(countorsImage);
        associateParticlesWLines();
        // associateParticles();
        return;
}
void clsParticles::drawManyParticles(IplImage *imageOut)
{

        unsigned int t = 0;
        t = 0;
        while(heightSeparate.size() > t)
        {
                cvLine(imageOut,cvPoint(0,heightSeparate[t]),cvPoint(imageOut->width,heightSeparate[t]),CV_RGB(77,255,0),2);
                t++;
        }
        t = 0;
        while(particles.size() > t)
        {
                particles[t].drawTail(imageOut);
                t++;
        }
        
        t = 0;
        while(limboParticles.size() > t)
        {
                limboParticles[t].drawTail(imageOut,true);
                t++;
        }
        
}

void clsParticles::findNewParticles(IplImage *countorsImage)
{
        currentParticles.clear();
        vector<clsParticle>(currentParticles).swap( currentParticles );
        
        CvMemStorage *storage = cvCreateMemStorage(0);
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
                
        CvSeq* contour;
        
        cvFindContours( countorsImage, storage, &contour, sizeof(CvContour),CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );
        
        if(contour != NULL )
        {
                double x,y;
                CvRect rectTmp;
                double area;
                
                for( ; contour != 0; contour = contour->h_next )
                {
                        
                        CvSeq *cesq=contour;
                        cvContourMoments(cesq,moments);
                        
                        area = moments->m00;
                        if(area == 0)
                                continue;
                        
                        x = moments->m10 / area;
                        y = moments->m01 / area;
                        
                        rectTmp = cvBoundingRect(contour);
                        
                        x = x + paramsInt["roiX"];
                        y = y + paramsInt["roiY"];
                        
                        currentParticles.push_back(clsParticle(0,x,y,rectTmp,area));
                        
                        // free(cesq);
                }
        }
        
        cvClearMemStorage(storage);
        cvReleaseMemStorage(&storage);
        
        free(moments);
        
        // free(contour);

}

void clsParticles::associateParticlesWLines()
{
        for(unsigned int t=0;t <particles.size(); t++)
        {
                particles[t].timenotfound++;
        }
        
        while(currentParticles.size() > 0)
        {
                for(unsigned int t=0;t <heightSeparate.size(); t++)
                {
                        if(currentParticles.back().y > heightSeparate[t-1] && currentParticles.back().y < heightSeparate[t])
                        {
                                bool found = false;
                                        for(unsigned int k=0;k <particles.size(); k++)
                                        {
                                                if(particles[k].id == t )
                                                {
                                                        if(particles[k].timenotfound > 0)
                                                        {
                                                                DEBUGMSG(("1st: %.2f %.2f -> %d",particles[k].area,particles.back().area,k));
                                                                particles[k].addPosition(currentParticles.back());
                                                        }
                                                        else
                                                        {
                                                                currentParticles.back().x = (currentParticles.back().x +  particles[k].x)/2;
                                                                currentParticles.back().y = (currentParticles.back().y +  particles[k].y)/2;
                                                                particles[k].setPosition(currentParticles.back());
                                                        }
                                                        // else if(oldParticles[k].area < currentParticles.back().area)
                                                        // {//apanhar só a maior particula de todas ;)
                                                        //         DEBUGMSG(("add: %.2f %.2f -> %d",oldParticles[k].area,currentParticles.back().area,k));
                                                        //         oldParticles[k].addPosition(currentParticles.back());
                                                        // }
                                                        
                                                        found = true;
                                                }
                                        }
                                        if(!found)
                                        {
                                                currentParticles.back().id = t;
                                                particles.push_back(currentParticles.back());
                                        }
                        }
                }
                currentParticles.pop_back();
        }
        
}

std::vector<clsParticle> clsParticles::associateParticles(std::vector<clsParticle> oldParticles,std::vector<clsParticle> currParticles)
{
        
        // INFOMSG(("current: %d old: %d limbo: %d",currentParticles.size(),oldParticles.size(),limboParticles.size()));
        std::vector<clsParticle> newParticles;
        
        /* TIME NOT FOUND:: DISABLED FOR NOW
        for (std::vector<clsParticle>::iterator itr = oldParticles.begin();itr != oldParticles.end(); ++itr)
        {
                itr->timenotfound++;
        }
        
        
        for (std::vector<clsParticle>::iterator itr = limboParticles.begin();itr != limboParticles.end(); ++itr) 
        {
                if(itr->timenotfound < paramsInt["maxLimboFrameLost"])
                {
                        itr->timenotfound++;
                }else
                {
                        limboParticles.erase(itr);
                }
        }
        */
        while(currentParticles.size() > 0)
        {
                
                
                bool foundParticleInLimbo = false;
                std::vector<clsParticle>::iterator itrLimboParticles;
                for (itrLimboParticles = limboParticles.begin();itrLimboParticles != limboParticles.end(); ++itrLimboParticles)
                {
                        if(currentParticles.back().isInRange(*itrLimboParticles))
                        {
                                foundParticleInLimbo = true;
                                break;
                        }
                }
                if(foundParticleInLimbo)
                {
                        (*itrLimboParticles).addPosition(currentParticles.back());
                        newParticles.push_back((*itrLimboParticles));
                        limboParticles.erase(itrLimboParticles);
                }else
                {
                        bool foundParticle = false;
                        std::vector<clsParticle>::iterator itrOldParticles;
                        for (itrOldParticles = oldParticles.begin();itrOldParticles != oldParticles.end(); ++itrOldParticles)
                        {
                                if(currentParticles.back().isInRange(*itrOldParticles))
                                {
                                        foundParticle = true;
                                        break;
                                }
                        }
                        
                        if(foundParticle)//&& oldParticles[smallestParticleId].timenotfound > 0)
                        { //aqui se encontrou a particula....
                                // oldParticles[smallestParticleId].addPosition(currentParticles.back());
                                (*itrOldParticles).addPosition(currentParticles.back());
                                newParticles.push_back((*itrOldParticles));
                                
                                oldParticles.erase(itrOldParticles);
                        }else
                        {
                                
                                
                                currentParticles.back().id = particleId++;
                                // INFOMSG(("new particle %d",currentParticles.back().id));
                                newParticles.push_back(currentParticles.back());
                        }
                }
                
                currentParticles.pop_back();
        }
        
        while(oldParticles.size() > 0)
        {
                bool foundParticle = false;
                std::vector<clsParticle>::iterator itrNewParticles;
                for (itrNewParticles = newParticles.begin();itrNewParticles != newParticles.end(); ++itrNewParticles)
                {
                        if(oldParticles.back().isInRange(*itrNewParticles))
                        {
                                foundParticle = true;
                                break;
                        }
                }
                if(!foundParticle){
                        limboParticles.push_back(oldParticles.back());
                }else
                {
                        // INFOMSG(("particle ignored: %.2f %.2f",oldParticles.back().x,oldParticles.back().y));
                }
                
                oldParticles.pop_back();
        }
                
        return newParticles;
}

void clsParticles::associateParticles()
{
        
        // INFOMSG(("current: %d old: %d limbo: %d",currentParticles.size(),oldParticles.size(),limboParticles.size()));
        // std::vector<clsParticle> newParticles;
        
        /* TIME NOT FOUND:: DISABLED FOR NOW
        for (std::vector<clsParticle>::iterator itr = oldParticles.begin();itr != oldParticles.end(); ++itr)
        {
                itr->timenotfound++;
        }
        
        
        for (std::vector<clsParticle>::iterator itr = limboParticles.begin();itr != limboParticles.end(); ++itr) 
        {
                if(itr->timenotfound < paramsInt["maxLimboFrameLost"])
                {
                        itr->timenotfound++;
                }else
                {
                        limboParticles.erase(itr);
                }
        }
        */
        particles.clear();
        while(currentParticles.size() > 0)
        {
                
                
                bool foundParticleInLimbo = false;
                std::vector<clsParticle>::iterator itrLimboParticles;
                for (itrLimboParticles = limboParticles.begin();itrLimboParticles != limboParticles.end(); ++itrLimboParticles)
                {
                        if(currentParticles.back().isInRange(*itrLimboParticles))
                        {
                                foundParticleInLimbo = true;
                                break;
                        }
                }
                if(foundParticleInLimbo)
                {
                        (*itrLimboParticles).addPosition(currentParticles.back());
                        particles.push_back((*itrLimboParticles));
                        limboParticles.erase(itrLimboParticles);
                }else
                {
                        bool foundParticle = false;
                        std::vector<clsParticle>::iterator itrOldParticles;
                        for (itrOldParticles = oldParticles.begin();itrOldParticles != oldParticles.end(); ++itrOldParticles)
                        {
                                if(currentParticles.back().isInRange(*itrOldParticles))
                                {
                                        foundParticle = true;
                                        break;
                                }
                        }
                        
                        if(foundParticle)//&& oldParticles[smallestParticleId].timenotfound > 0)
                        { //aqui se encontrou a particula....
                                // oldParticles[smallestParticleId].addPosition(currentParticles.back());
                                (*itrOldParticles).addPosition(currentParticles.back());
                                particles.push_back((*itrOldParticles));
                                
                                oldParticles.erase(itrOldParticles);
                        }else
                        {
                                
                                
                                currentParticles.back().id = particleId++;
                                // INFOMSG(("new particle %d",currentParticles.back().id));
                                particles.push_back(currentParticles.back());
                        }
                }
                
                currentParticles.pop_back();
        }
        
        while(oldParticles.size() > 0)
        {
                bool foundParticle = false;
                std::vector<clsParticle>::iterator itrNewParticles;
                for (itrNewParticles = particles.begin();itrNewParticles != particles.end(); ++itrNewParticles)
                {
                        if(oldParticles.back().isInRange(*itrNewParticles))
                        {
                                foundParticle = true;
                                break;
                        }
                }
                if(!foundParticle){
                        limboParticles.push_back(oldParticles.back());
                }else
                {
                        // INFOMSG(("particle ignored: %.2f %.2f",oldParticles.back().x,oldParticles.back().y));
                }
                
                oldParticles.pop_back();
        }
                
        // return newParticles;
}

std::vector<clsParticle> clsParticles::toVector()
{
        std::vector<clsParticle> particulas;
        unsigned int t = 0;
        while(particles.size() > t)
        {
                particulas.push_back(particles[t]);
                t++;
        }
        t = 0;
        while(limboParticles.size() > t)
        {
                particulas.push_back(limboParticles[t]);
                t++;
        }
        
        return particulas;
}

bool clsParticles::toString(std::string *str, int numMaxParticles)
{
        char stringg[1024] = {0};
        int t = 1, idxParticle = 0;
        
        str->clear();
        while(t < numMaxParticles+1)
        {
                bool foundParticle = false;
                for(unsigned int k = 0; k < particles.size();k++)
                {
                        if(t == particles[k].id)
                        {
                                idxParticle = k;
                                foundParticle = true;
                                break;
                        }
                }
                if(foundParticle)
                {
                        double xPixels = particles[idxParticle].x-paramsInt["roiX"];
                        double yPixels = particles[idxParticle].y-paramsInt["roiY"];
                        double xCm = (double)paramsInt["xMm"] * xPixels / (double)paramsInt["roiW"];
                        double yCm = (double)paramsInt["yMm"] * yPixels / (double)paramsInt["roiH"];
                        sprintf(stringg,"%.2f;%.2f;%.2f;%.2f;%.2f;%d;",xPixels,yPixels,xCm,yCm,particles[idxParticle].area,particles[idxParticle].timenotfound);
                }else
                {
                        sprintf(stringg,"NA;NA;NA;NA;NA;NA;");
                }
                str->append(stringg);
                
                t++;
        }
        
        return true;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

clsParticle::clsParticle(int idx, double x,double y,CvRect rect,double area)
{
        this->x = x;
        this->y = y;
        this->countorRect = rect;
        this->area = area;
        points.push_back(cvPoint(x,y));

        id = idx;
        timenotfound = 0;
        //color = CV_RGB( rand()&200, rand()&200, rand()&200 );
        color = CV_RGB( 0, 0, 255);
}

clsParticle::clsParticle(int idx, double x,double y,double area)
{
        this->x = x;
        this->y = y;
        this->area = area;
        points.push_back(cvPoint(x,y));

        id = idx;
        timenotfound = 0;
        //color = CV_RGB( rand()&200, rand()&200, rand()&200 );
        color = CV_RGB( 0, 0, 255);
}
clsParticle::clsParticle()
{}
clsParticle::~clsParticle()
{
        points.clear();
        vector<CvPoint>(points).swap( points );
        
}

// bool clsParticle::isInside(CvSeq *cesq)
// {
//         CvPoint2D32f tmp = cvPoint2D32f(x,y );
//         double t = cvPointPolygonTest(cesq,tmp,0);

//         return true;
// }

double clsParticle::calcDistance(double newx,double newy)
{

        current_distance = fabs(sqrt( (newx-x)*(newx-x) + (newy-y)*(newy-y)));
        return current_distance;
}
double clsParticle::calcDistance(clsParticle *particle)
{
        current_distance = fabs(sqrt( (particle->x-x)*(particle->x-x) + (particle->y-y)*(particle->y-y)));
        return current_distance;
}
double clsParticle::calcDistance(clsParticle particle)
{
        current_distance = fabs(sqrt( (particle.x-x)*(particle.x-x) + (particle.y-y)*(particle.y-y)));
        return current_distance;
}
void clsParticle::addPosition(double x, double y, CvRect rect)
{
        timenotfound = 0;

        this->x = x;
        this->y = y;
        countorRect = rect;

        points.push_back(cvPoint(x,y));

        while((int)points.size() > paramsInt["maxSizeTrail"])
        {
                points.erase(points.begin());
        }
}

void clsParticle::addPosition(clsParticle p)
{
        timenotfound = 0;

        x = p.x;
        y = p.y;
        countorRect = p.countorRect;
        area = p.area;
        points.push_back(cvPoint(x,y));

        while((int)points.size() > paramsInt["maxSizeTrail"])
        {
                points.erase(points.begin());
        }
}
void clsParticle::setPosition(clsParticle p)
{
        timenotfound = 0;

        x = p.x;
        y = p.y;
        countorRect = p.countorRect;
        
        points.pop_back();
        points.push_back(cvPoint(x,y));

        while((int)points.size() > paramsInt["maxSizeTrail"])
        {
                points.erase(points.begin());
        }
}
void clsParticle::drawTail(IplImage *frame, bool isLimbo)
{
        int t = 0;
        char tmp_txt[10];

        CvScalar colorRect =  CV_RGB(0,202,0);
        if(isLimbo || timenotfound > 0)
        {
                colorRect = CV_RGB(255,214,0);
        }
        
        if(isLimbo || timenotfound > paramsInt["timenotfoundPaintRedFPS"])
        {
                colorRect = CV_RGB(255,0,0);
        }
        CvPoint rect1 = cvPoint(x-(paramsInt["maxDistanceNeigborX"]/2),(y-paramsInt["maxDistanceNeigborY"]/2));
        CvPoint rect2 = cvPoint(x+(paramsInt["maxDistanceNeigborX"]/2),(y+paramsInt["maxDistanceNeigborY"]/2));
        cvRectangle(frame, rect1,rect2, colorRect, 2, 40, 0);
        
        // INFOMSG(("%d",paramsInt["maxDistanceNeigborX"]));
        // int radius = (int)paramsInt["maxDistanceNeigbor"]/2;

        // cvCircle(frame,cvPoint(x,y),radius,colorRect);
        // sprintf(tmp_txt,"%d (a: %.2f)",id,area);
        sprintf(tmp_txt,"%d",id);
        cvPutText(frame,tmp_txt,cvPoint(x, y-20),&particlesFont,CV_RGB(255,255,255));

        while((int)points.size()-paramsInt["skipTailFPS"] > t)
        {
                // cvRectangle(frame, cvPoint(points[t].x,points[t].y),cvPoint(points[t].x+1,points[t].y+1), color, 1, 40, 0);
                cvLine(frame, cvPoint(points[t].x,points[t].y),cvPoint(points[t+1].x,points[t+1].y), CV_RGB(255,255,255), 1, 40, 0);
                t++;
        }

}

bool clsParticle::isInRange(int anX,int anY)
{
        //usleep(1000000);
        
        if(anX > x-(paramsInt["maxDistanceNeigborX"]/2) &&
                anX < x+(paramsInt["maxDistanceNeigborX"]/2) &&
        anY > y-(paramsInt["maxDistanceNeigborY"]/2) &&
        anY < y+(paramsInt["maxDistanceNeigborY"]/2))
        {
                return true;
        }
        return false;
}

bool clsParticle::isInRange(clsParticle p)
{
        if(p.x > x-(paramsInt["maxDistanceNeigborX"]/2) &&
                p.x < x+(paramsInt["maxDistanceNeigborX"]/2) &&
        p.y > y-(paramsInt["maxDistanceNeigborY"]/2) &&
        p.y < y+(paramsInt["maxDistanceNeigborY"]/2))
        {
                // INFOMSG(("X:: %.2f < %.2f < %.2f Y:: %.2f < %.2f < %.2f",x-(paramsInt["maxDistanceNeigborX"]/2),p.x,x+(paramsInt["maxDistanceNeigborX"]/2),
                //         y-(paramsInt["maxDistanceNeigborY"]/2),p.y,y+(paramsInt["maxDistanceNeigborY"]/2)
                //         ));
                return true;
        }
        return false;
}

bool clsParticle::setFolder(std::string fld)
{
        folder = fld;
        return true;
}

//------------------------------------------
//------------------YARP CLASSES
//------------------------------------------
bool clsYarp::send(std::vector<clsParticle> particles)
{
        yarp::os::Bottle& input = port.prepare();
        input.clear();
	
	
        yarp::os::Bottle garrafa;
        for(unsigned int t=0; t < particles.size();t++)
        {
                garrafa.clear();
                garrafa.addInt(particles[t].id);
                garrafa.addDouble(particles[t].x);
                garrafa.addDouble(particles[t].y);
                garrafa.addDouble(particles[t].area);
                input.addList() = garrafa;
        }
        
        port.write();
        
        return true;
}

bool clsYarp::rcv(std::vector<clsParticle> *particles,bool locked)
{
        particles->clear();
        if(port.getPendingReads() > 0)
	{
		input = port.read(locked);
		if (input!=NULL) {
		        // yarp::os::Bottle lst = 		        input->get(0)->asList();
		        yarp::os::Bottle *lst;
		        lst = input->get(0).asList();
		        int id = lst->get(0).asInt();
		        double x = lst->get(0).asDouble();
		        double y = lst->get(0).asDouble();
		        double area = lst->get(0).asDouble();
		        
		        particles->push_back(clsParticle(id,x,y,area));
		}
	}
        
        return false;
}
