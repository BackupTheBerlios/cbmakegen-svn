#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "26";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2007";
	static const double UBUNTU_VERSION_STYLE = 7.10;
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 3;
	static const long BUILD = 11;
	static const long REVISION = 10;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 93;
	#define RC_FILEVERSION 0,3,11,10
	#define RC_FILEVERSION_STRING "0, 3, 11, 10\0"
	static const char FULLVERSION_STRING[] = "0.3.11.10";
	
	//SVN Version
	static const char SVN_REVISION[] = "3";
	static const char SVN_DATE[] = "2007-08-07T10:48:18.310453Z";
	

}
#endif //VERSION_h
