#include <cppunit/config-auto.h>
#include <cstdlib>
#include <cstdio>

// Simple test against arbitrary versions > 1.10
#define MIN_MAJOR 1
#define MIN_MINOR 10

int main(int, char*[])
{
	char *base;
		
	if( strtol( CPPUNIT_VERSION, &base, 10 ) == MIN_MAJOR )
		if( strtol( ++base, NULL, 10 ) >= MIN_MINOR )
			return 1; 
		return 0;
	return 0;
}
