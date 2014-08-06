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

    std::cout << "mSignals.size() = " << mSignals.size() << std::endl;

    Combinations aCombinations;

    std::vector< Signal >::iterator iter;
    for( iter = mSignals.begin(); iter != mSignals.end(); ++iter )
    {
        int satId = (*iter).getSatId();
        long double timestamp = (*iter).getTimestamp();

//        std::cout << "size of signal: " << (*iter).getSize() << std::endl;
        if( (*iter).getSize() >= 4 && (*iter).getTimestamp()>1352 && (*iter).getTimestamp()<1353 )
        {
            std::cout << std::endl << "***********************************************************************************" << std::endl;
            std::cout << "For satellite " << (*iter).getSatId() << " and sending time " << (*iter).getTimestamp() << " there are " << (*iter).getSize() << " GS" << std::endl;
//            (*iter).printSignal();
            Stations mStations;
            
            (*iter).convertSignalToStation( mStations );

            std::vector< std::vector< int > > stationsComb;
            
  //          std::cout << "Number of ground stations: " << mStations.size() << std::endl;
            PositionsList xyzr;
            //xyzr.addPositions( solveApol( satId, timestamp, mStations ) );
            
            int N = mStations.size();
            std::cout << "N = " << N << std::endl;
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
            
            xyzr.addPositions( solveApol( satId, timestamp, mStations ) );

            if( N>5 )
            {
                int k = N-1;
//                std::cout << "For satellite " << satId << ", "<< N << " ground stations and time of sending signal " << timestamp << " I want to set of satellites of size: ";
//                std::cin >> k;

                stationsComb = aCombinations.getStationsCombinations( N, k );
                std::cout << stationsComb.size() << " combinations" << std::endl;
                std::vector< std::vector< int > >::iterator iterSt;
                int pos = 0;
                for( iterSt = stationsComb.begin(); iterSt != stationsComb.end()/* && pos<10*/ ; ++iterSt )
                {
                    std::cout << std::endl << ++pos << "/" << stationsComb.size() << ": " ;
                    Stations aStations;
                    for( int i=0; i<k; ++i )
                    {
                        aStations.addStation( mStations.getStation( (*iterSt).at(i) ) );
                        std::cout << (*iterSt).at(i) << " ";
                    }
                    std::cout << std::endl;
/*                    for( int i=0; i<5; ++i )
                    {
                        std::cout << mStations.getStation((*iterSt).at(i)).getX() << " ";
                        std::cout << mStations.getStation((*iterSt).at(i)).getY() << " ";
                        std::cout << mStations.getStation((*iterSt).at(i)).getZ() << " ";
                        std::cout << std::endl;
                    }*/
                    xyzr.addPositions( solveApol( satId, timestamp, aStations ) );

                }
            }
           
           /* 
            xyzr.printPositions();
            for( int i=0; i<xyzr.size(); ++i )
            {
                lResultFile << timestamp << " " << xyzr.getX( i ) << " " << xyzr.getY( i ) << " " << xyzr.getZ( i ) << "\n";
            }*/
        }
        else
        {
    //        std::cout << "For satellite " << (*iter).getSatId() << " with sending time " << (*iter).getTimestamp() << " only " << (*iter).getSize() << " GS found" << std::endl;
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
void Reader::loadGSData( const char* lFileName )
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
    while( lFile >> ax >> ay >> az >> adt >> at0 ) //>> satId ) // >> at0 >> satId >> op1  )
    {
    //    std::cout << lFileName << " " << at0 << ", x, y, z: " << ax << " " << ay << " " << az << std::endl;
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
            mSignal.addGroundStation( ax, ay, az, at0, adt ); 
            mSignals.push_back( mSignal );
        }
    }

    lFile.close();
}

/** loading data from all .gsd files in given folder
 *  @param lDirName name of folder with .gsd files
 */
void Reader::loadFromDirectory( char* lDirName )
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

//            std::cout << "Processing file: " << file << std::endl;
            loadGSData( file.c_str() );          

        }
        closedir( dir );
    }
}

/** loading data from file "stations.txt"
 *  @param lFileName a name of file which contains data
 *  @param mStations a contener of stations 
 */
void Reader::loadStations( char* lFileName, Stations& mStations )
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

