
 * [Flywalk Homepage](https://flywalk.eempo.net)

                              8888                               
                             88888                               
                             888%888                             
                             88888888                            
                             888888888                           
                             8888888888                          
                  8888       88888888888                         
                  %%%%%8     88888888888                         
                  88%%%%88   88888888888                         
                   888%%%888 888888888888                        
                    8888%%%888888888888 8                        
                      8888%%%%888888888888                       
                       8888%%%%%8888888888                       
                        88888%BBB%8 888888                       
                         888%%%BBB%8888888                       
                           8888%B@BB%88%%%8                      
                              88%B@@@@@@@@B%                     
                               8%B@@@@@@@@@@B8                   
                            8%B@@@@@B@@@@@@@@@%8                 
                         8%B@@@@@@@@@@@@@@@@@@@BB%8              
                         B@@@@@@@@@@@@@@@@@@@@@@@@B8             
                       8B@@@@@@@@@@@@@@@@@@@@@@@@@@B8            
                       B@@@@@@@@@@@@@@@@@@@@@@@@@@@@%            
                      8@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@        
                      8@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       
                       8%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       
                           888  @@@@@@@@@@@@@@@@@@@@@@@@@@       
                                @@@@@@@@@@@@@@@@@@8o             
                                @@@@@@@@@@@@@@@@@@               
                                @@8@@@@@@@@@@@@@@@B              
                                B@ B@@@@@@@B@@   @@              
                               %@B     @@  B@    B@B             
                               B@      @@  B@@   8B@             
                             %B@B     B@@  8@@    BB             
                            %@@@     B@@@   @@    BB@            
                           %@@@    %@@@@ o  @@@    B@@           
                         BB@@@     B@@@      B@B   B@@B          
                        B@@@@                B@@     B           
                        B@ o                  @B                 
                        8 o                                      
                                                                 
                        
# flywalkreloaded
  Flywalk is a open source software for real-time tracking. Enhance your experiment samples making them more detailed, accurate and precise.
  
  
Compiling and running
------------

On debian:  

    sudo apt-get install libace-dev qmake cmake qt5-qmake libboost-filesystem-dev libboost-regex-dev libv4l-dev

    git clone 

    cmake ../src

    make flywalk

    cd bin/

    ./flywalk

On raspberry:


Reporting bugs
------------
    sudo apt-get install gdb
 
    gdb flywalk

In gdb:
        > r
  
send me the output! thank you.

Settings
-------
after you compile there are this 2 files in the bin folder:

settings.ini and settings_proc.ini

settings.ini
is where it records all the camera settings, you don't need to change this. but you can save it so you can load the same settings each time you start with a different setup.

settings_proc.ini
[General]
bg=2 /* type of background subtraction. 1 - BG codeBookModel 2- BG_COLORSUBTRACTION 3- BG_SEG (check opencv manual) */
blur=4 /* the amount of the blur, to apply to the image, to remove the noise of the image */
erosion=4 /* remove all the groups of pixels which are smaller than 4 pixel in area */
frameRate=50 /* the frame rate to record data */
maxDistanceNeigborX=100 /* maximum distance X to the neibourghd to consider this is not activate */
maxDistanceNeigborY=15 /* maximum distance Y to the neibourghd to consider this is not activate */
maxSizeTrail=100 /* size of the tail only to show on the GUI */
maxThreshold=20 /* maximum threshold (depends on the type of background subtraction u choose, please check code... manual to be done ) */
minThreshold=10 /* minimum threshold (depends on the type of background subtraction u choose, please check code... manual to be done ) */
showProcessed=0  /* toggle foreground mode */
showTraces=1 /* show or hide traces */
skipTailFPS=5 /* ?? skip first 5 frames?  */
timenotfoundPaintRedFPS=100 /* ?? fail at 100 frames dropped?  */
maxNumberParticles=30 /* maximum number of particles */
minH=0 /* minimum HUE (for BG_COLORSUBTRACTION) */
minL=0  /* minimum L (for BG_COLORSUBTRACTION) */
minS=0  /* minimum HUE (for BG_COLORSUBTRACTION) */
roiH=354 /* Heigth of the ROI (region of interest) in pixels */
roiW=620 /* Width of the ROI */
roiX=6  /* Start x of the ROI */
roiY=37  /* Heigth of the ROI */
xMm=0  /* width of area in milimeters convertion from the size of the ROI to mm */
yMm=0  /* height of area in milimeters convertion from the size of the ROI to mm */

[lines] /* define the lines to split each blob */
line2=469 /* first line in pixels */
line3=442 /* second line in pixels */
line4=405 /* ... u can add as many lines as you want */


License
-------

GPL, with some libraries linked to projects with different licenses all of them either apache or GPL compatible.


