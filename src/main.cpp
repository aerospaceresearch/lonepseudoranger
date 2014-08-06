#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <unistd.h>
#include <tuple>

#include <cmath> // square root
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Reader.h"
//#include "Combinations.h"
//#include "GaussianMatrix.h"

void processSignalData();

int main( int argc, char* argv[] )
{
    Stations mStations;
    Reader reader;
    bool test = false;
    int c = getopt( argc, argv, "fd:" );
    switch( c ){
    case 'd':
        reader.loadFromDirectory( optarg );
        processSignalData();
        break;
	case 'f':
//	    loadStations( optarg, mStations );
        test = true;
	    break;
    default:
	    std::cout << "Missing file name! Use parameter -f filename." << std::endl;
	    return -1;
    }
/*
    if( test )
    {
        std::vector< std::vector< int > > stationsComb;
        std::vector< Station > takenStations;

        if( mStations.size() > 3 )
        {
            std::cout << "Number of ground stations: " << mStations.size() << std::endl;
            stationsComb = getStationsCombinations( mStations.size(), 4 );
	        std::cout << "Number of combinations: " << stationsComb.size() << std::endl;
            PositionsList xyzr;
        
            std::vector< std::vector< int > >::iterator iter; 
        	for( iter = stationsComb.begin(); iter != stationsComb.end(); ++iter )
	        {
                xyzr.addPositions( solveApol( 0, 0, mStations ) );
            }
            xyzr.printAveragePosition();
        }
        else
        {
            std::cout << "File stations.txt have to contain more ground stations!" << std::endl;
        }
    }*/
    return 0;
}
