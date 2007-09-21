#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static char DATE[] = "21";
	static char MONTH[] = "09";
	static char YEAR[] = "2007";
	static double UBUNTU_VERSION_STYLE = 7.09;
	
	//Software Status
	static char STATUS[] = "Alpha";
	static char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static long MAJOR = 0;
	static long MINOR = 3;
	static long BUILD = 0;
	static long REVISION = 0;
	
	//Miscellaneous Version Types
	static long BUILDS_COUNT = 62;
	#define RC_FILEVERSION 0,3,0,0
	#define RC_FILEVERSION_STRING "0, 3, 0, 0\0"
	static char FULLVERSION_STRING[] = "0.3.0.0";
	
	//SVN Version
	static char SVN_REVISION[] = "3";
	static char SVN_DATE[] = "2007-08-07T10:48:18.310453Z";
	
}
#endif //VERSION_h
