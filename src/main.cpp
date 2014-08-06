#include <iostream>
#include <unistd.h>

#include "Reader.h"

void processSignalData();

int main( int argc, char* argv[] )
{
    Stations mStations;
    Reader reader;
    bool test = false;
    int c = getopt( argc, argv, "d:" );
    switch( c ){
    case 'd':
        reader.loadFromDirectory( optarg );
        processSignalData();
        break;
    default:
	    std::cout << "Missing path to the directory! Use parameter -d." << std::endl;
	    return -1;
    }
    return 0;
}
