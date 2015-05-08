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
#ifdef TIXML_USE_STL
	#include <iostream>
	#include <sstream>
	using namespace std;
#else
	#include <stdio.h>
#endif

#include "tinyxml/tinyxml.h"
#include "utils.h"

class clsXMLCFG
{
	TiXmlDocument doc;
	
	bool getOptions(char *filename,options2dTrack *options);
};
