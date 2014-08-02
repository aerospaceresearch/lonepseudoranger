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

#include "Def.h"
#include "Combinations.h"
#include "GaussianMatrix.h"
#include "Apollonius.h"

/**  
 * processing of all collected signals
 */
void processSignalData()
{
    std::cout << "********* PROCESSING SIGNALS *****************************" << std::endl;
    std::string lResultFileName( "resultFile" );
    std::fstream lResultFile;
    lResultFile.open( lResultFileName, std::ios::in );
    if( !lResultFile.is_open() )
    	std::cout << "ERROR: problem with file containing results" << std::endl;


    std::vector< Signal >::iterator iter;
    for( iter = mSignals.begin(); iter != mSignals.end(); ++iter )
    {
        int satId = (*iter).getSatId();
        long double timestamp = (*iter).getTimestamp();

        if( (*iter).getSize() >= 4 )
        {
            std::cout << "For satellite " << (*iter).getSatId() << " and sending time " << (*iter).getTimestamp() << " there are " << (*iter).getSize() << " GS" << std::endl;
            (*iter).printSignal();
            Stations mStations;
            
            (*iter).convertSignalToStation( mStations );

            std::vector< std::vector< int > > stationsComb;
            
            std::cout << "Number of ground stations: " << mStations.size() << std::endl;
            PositionsList xyzr;
            //xyzr.addPositions( solveApol( satId, timestamp, mStations ) );
            
            int N = mStations.size();
/* // last two combinations:
            if( N>6 )
            {
                Stations aStations1;
                Stations aStations2;
                aStations1.addStation( mStations.getStation( N-7 ) );
                for( int i=6; i>1; --i )
                {
                    aStations1.addStation( mStations.getStation( N-i ) );
                    aStations2.addStation( mStations.getStation( N-i ) );
                }
                aStations2.addStation( mStations.getStation( N-1 ) );

                xyzr.addPositions( solveApol( satId, timestamp, aStations1 ) );
                xyzr.addPositions( solveApol( satId, timestamp, aStations2 ) );
            }*/
            // only  sets of 5
        /*    if( N>5 )
            {
                for( int i=0; i<N-5; ++i )
                {
                    Stations aStations;
                    for( int j=0; j<5; ++j )
                    {
                        aStations.addStation( mStations.getStation( i+j ) );
                    }
                    xyzr.addPositions( solveApol( satId, timestamp, aStations ) );
                }
            }*/
            /*
            if( N>5 )
            {
                stationsComb = getStationsCombinations( N, 5 );
                std::vector< std::vector< int > >::iterator iterSt;
                for( iterSt = stationsComb.begin(); iterSt != stationsComb.end(); ++iter )
                {
                    Stations aStations;
                    for( int i=0; i<5; ++i )
                        aStations.addStation( mStations.getStation( (*iterSt).at(i) ) );

                    xyzr.addPositions( solveApol( satId, timestamp, aStations ) );

                }
            }*/
//            else
            {
                xyzr.addPositions( solveApol( satId, timestamp, mStations ) );
            }
            xyzr.printPositions();
            for( int i=0; i<xyzr.size(); ++i )
            {
                lResultFile << timestamp << " " << xyzr.getX( i ) << " " << xyzr.getY( i ) << " " << xyzr.getZ( i ) << "\n";
            }
        }
        else
        {
            std::cout << "For satellite " << (*iter).getSatId() << " with sending time " << (*iter).getTimestamp() << " only " << (*iter).getSize() << " GS found" << std::endl;
        }
    }
    lResultFile.close();
}
/** loading data from file "stations.txt"
 *  @param lFileName a name of file which contains data
 *  @param mStations a contener of stations 
 */

/** loading data from file 
 *  @param name of file with data
 */
void loadGSData( const char* lFileName )
{
    Stations mStations;
    std::fstream lFile;
    lFile.open( lFileName, std::ios::in );
    if( !lFile.is_open() )
    	std::cout << "ERROR: problem with file" << std::endl;

    double ax, ay, az;
    long double at0, adt;
    int satId = 0; // TODO: tymczasowo 0
    std::string op1;
    while( lFile >> ax >> ay >> az >> adt >> at0 >> satId ) // >> at0 >> satId >> op1  )
    {
        bool satKnown = false;
        std::vector< Signal >::iterator iter;
        for( iter = mSignals.begin(); !satKnown && iter != mSignals.end(); ++iter )
        {
            if( (*iter).getSatId() == satId && (*iter).getTimestamp() == at0 )
            {
                if (!(*iter).positionKnown( ax, ay, az ))
                {
                    (*iter).addGroundStation( ax, ay, az, at0, adt );
                }
                satKnown = true; // bez tego tez sie da
            }
        }

        if( !satKnown )
        {
            Signal mSignal;
            mSignal.setSatId( satId );
            mSignal.setTimestamp( at0 );
//        std::cout << "Adding new satellite: " << ax << " " << ay << " " <<  az << " " <<  at0 << " " <<  adt << " " <<  satId << " " << op1 << std::endl;
            mSignal.addGroundStation( ax, ay, az, at0, adt ); 
            mSignals.push_back( mSignal );
        }
    }

    lFile.close();
}

/** loading data from all .gsd files in given folder
 *  @param lDirName name of folder with .gsd files
 */
void loadFromDirectory( char* lDirName )
{
    std::string file;
    DIR *dir;
    struct dirent *dirEnt;
    struct stat filestat;
    dir = opendir( lDirName );
    if( dir == NULL )
    {
        std::cout << "ERROR: Problem with directory" << std::endl;
    }
    else
    {
        while( dirEnt = readdir( dir ) )
        {
            file = std::string(lDirName) + "/" + dirEnt->d_name;

            if( stat( file.c_str(), &filestat )) continue;
            if( S_ISDIR( filestat.st_mode ) ) continue;

            std::cout << "Processing file: " << file << std::endl;
            loadGSData( file.c_str() );          

        }
        closedir( dir );
    }
}

/** loading data from file "stations.txt"
 *  @param lFileName a name of file which contains data
 *  @param mStations a contener of stations 
 */
void loadStations( char* lFileName, Stations& mStations )
{
    std::fstream lFile;
    lFile.open( lFileName, std::ios::in );
    if( !lFile.is_open() )
	std::cout << "ERROR: problem with file" << std::endl;

    double ax, ay, az, at;
    lFile >> at;
    mStations.setTime( at );
    while( lFile >> ax >> ay >> az >> at )
    {
    	mStations.addStation( Station( ax, ay, az, at ) );
    }
    lFile.close();
}

int main( int argc, char* argv[] )
{
    Stations mStations;
    bool test = false;
    int c = getopt( argc, argv, "fd:" );
    switch( c ){
    case 'd':
        loadFromDirectory( optarg );
        processSignalData();
        break;
	case 'f':
	    loadStations( optarg, mStations );
        test = true;
	    break;
    default:
	    std::cout << "Missing file name! Use parameter -f filename." << std::endl;
	    return -1;
    }

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
    }
    return 0;
}
