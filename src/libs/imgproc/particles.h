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
#ifndef PARTICLES_H
#define PARTICLES_H

#include <highgui.h>
#include <cvaux.h>

class clsParticle;

class clsParticles
{
public:
        clsParticles();
        ~clsParticles();
        void reinitialize();
        
	void processManyParticles(IplImage *foregroundImage);
	void drawManyParticles(IplImage *paintedFrame);
	
	std::vector<clsParticle> toVector();
	bool toString(std::string *str, int numMaxParticles );
	
	std::vector<clsParticle> particles;
	std::vector<clsParticle> limboParticles;
	std::vector<clsParticle> currentParticles;
	std::vector<clsParticle> oldParticles;
	
	// std::vector<clsParticle *> currentParticles;
	// std::vector<clsParticle *> limboParticles;
	
	// CvBlobSeq *currBlobs;
	bool processParticlesStarted;
	
	void startLines(std::vector<int> linesCfg);
private:
	int particleId;
	int frameCount;
	
	IplImage *countorsImage;
	
	
	void findNewParticles(IplImage *countorsImage);
	void associateParticlesWLines();
	void associateParticles();
	
	std::vector<int> heightSeparate;
	
	std::vector<clsParticle> associateParticles(std::vector<clsParticle> particles,std::vector<clsParticle> currParticles);
};

class clsParticle
{
	
public:
	clsParticle();
	// clsParticle(int id,int x,int y,CvRect rect);
	clsParticle(int id,double x,double y,CvRect rect,double area = 0);
	clsParticle(int id,double x,double y,double area = 0);
	~clsParticle();
	
	
	// bool isInside(CvSeq *cesq);
	double calcDistance(double newx,double newy);
	double calcDistance(clsParticle *particle);
	double calcDistance(clsParticle particle);
	
	void addPosition(double x, double y,CvRect rect);
	void addPosition(clsParticle p);
	
	void setPosition(clsParticle p);
	
	void drawTail(IplImage *frame, bool isLimbo = false);
	
	bool isInRange(int x,int y);
	bool isInRange(clsParticle p);
	
	bool setFolder(std::string fld);
	
	double current_distance;
	double x; //in pixels
	double y; //in pixels
	
	double radius;
	unsigned int id;
	CvScalar color;
	int timenotfound;
	CvRect countorRect;
	double area;
	
private:
	std::vector <CvPoint> points;
	std::string folder;
	
};


#endif
