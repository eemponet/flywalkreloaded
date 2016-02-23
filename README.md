
 * Flywalk Homepage](https://img.shields.io/badge/YARP-Yet_Another_Robot_Platform-orange.svg)](https://flywalk.eempo.net)

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
 
License
-------

GPL, with some libraries linked to projects with different licenses all of them either apache or GPL compatible.

