#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <unistd.h>
#include <tuple>
#include <chrono>
#include <algorithm>
#include <bitset>

#include <cmath> // square root
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Reader.h"
#include "Combinations.h"
#include "GaussianMatrix.h"
#include "Apollonius.h"

std::vector< int > cluster( PositionsList, std::vector< std::vector< int > >, int );
std::vector< std::vector< double > > delays; ///< delays

/**  
 * processing of all collected signals
 */
void processSignalData()
{
    std::cout << "********* PROCESSING SIGNALS *****************************" << std::endl;
    std::string lResultFileName( "resultFile.txt" );
    std::fstream lResultFile;
    lResultFile.open( lResultFileName, std::ios::in );
    if( !lResultFile.is_open() )
    	std::cout << "ERROR: problem with file containing results" << std::endl;

    Combinations aCombinations;
    std::vector< Signal >::iterator iter;
    std::cout << "mSignals.size()=" << mSignals.size() << std::endl;
    for( iter = mSignals.begin(); iter != mSignals.end(); ++iter )
    {
        int satId = (*iter).getSatId();
        long double timestamp = (*iter).getTimestamp();

        std::cout << "For satellite " << satId << " and sending time " << timestamp << " there are " << (*iter).getSize() << " GS" << std::endl;
        if( (*iter).getSize() >= 4 && (*iter).getSize() < 64 ) 
        {
            auto time1 = std::chrono::high_resolution_clock::now();
            std::cout << std::endl << "***********************************************************************************" << std::endl;
            if( (*iter).getSize() < 4)
                std::cout << "Not taken sat" << (*iter).getSatId() << " with " << (*iter).getSize() << " gs" << std::endl;
            Stations mStations;
            (*iter).convertStationToSignal( mStations );
            std::vector< std::vector< int > > stationsComb;
            PositionsList xyzr;
            int N = mStations.size();

            if( N>6 )
            {
                int k = std::max( N-2, 4 );
                std::cout << "N=" << N << ", k=" << k << std::endl;

                stationsComb = aCombinations.getStationsCombinations( N, k );
                std::vector< std::vector< int > >::iterator iterSt;
                int pos = 0;
                for( iterSt = stationsComb.begin(); iterSt != stationsComb.end(); ++iterSt )
                {
                    Stations aStations;
                    for( int i=0; i<k; ++i )
                    {
                        aStations.addStation( mStations.getStation( (*iterSt).at(i) ) );
                    }
                    xyzr.addPositions( solveApol( satId, timestamp, aStations, pos, false ) ); // pos - combination id
                    ++pos;
                }
                std::vector< int > selectedCombinations = cluster( xyzr, stationsComb, N );
                std::sort( selectedCombinations.begin(), selectedCombinations.end() );  

                std::vector< int > selectedGS;
                std::vector< int >::iterator itInt;
                Stations selectedStations;

                // selectedCombinations - combinations selected by clustering
                for( itInt = selectedCombinations.begin(); itInt != selectedCombinations.end(); ++itInt )
                {
                    // from stationsComb
                    std::vector< std::vector< int > >::iterator combIter;
                    for( int counter = 0; counter < stationsComb.at(*itInt).size(); ++counter )
                    {
                        selectedGS.push_back( stationsComb.at(*itInt).at(counter) );
                    }
                }
                std::vector< int >::iterator intIt2;
                std::sort( selectedGS.begin(), selectedGS.end() );
                intIt2 = std::unique(selectedGS.begin(), selectedGS.end());
                selectedGS.resize(std::distance( selectedGS.begin(), intIt2 ) );

                if( selectedGS.size() < N )
                {
                    std::cout << "Not all of stations taken!!! "<< satId << ", time: " << timestamp << std::endl;
                }
                std::cout << std::endl << "Selected stations: " << selectedGS.size() << "/" << N << " (" << timestamp << ")" << std::endl;
                for( intIt2 = selectedGS.begin(); intIt2 != selectedGS.end(); ++intIt2 )
                {
                    selectedStations.addStation( mStations.getStation( *intIt2 ) );
                }
                std::cout << std::endl;

                std::cout << "Result after clustering: ";
                solveApol( satId, timestamp, selectedStations );
                selectedStations.printDelayStats( satId, timestamp );

                auto time2 = std::chrono::high_resolution_clock::now();
                std::cout << "Time of processing signal with timestamp " << (*iter).getTimestamp() << ": " << std::chrono::duration_cast< std::chrono::milliseconds >( time2-time1 ).count() << " milliseconds. N=" << N << std::endl;
            
                std::cout << "Result without clustering: ";
                solveApol( satId, timestamp, mStations );
            }
            else
            {
                std::cout << "Not taken sat " << satId << " with " << (*iter).getSize() << " gs." << std::endl;
            }
        }
        lResultFile.close();
    }
}

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

    double ax, ay, az, delay;
    double at0, adt, at1;
    int satId = 0; 
    std::string op1;
    std::vector< int > satellites;
    while( lFile >> ax >> ay >> az >> at1 >> at0 >> satId >> delay ) // >> at0 >> satId >> op1  )
    {
        adt = at1-at0;
        std::vector< std::vector< double > >::iterator itDelay = delays.begin();
        while( itDelay != delays.end() && (satId!=(*itDelay).at(0) || at0!=(*itDelay).at(1)) )
        {
            ++itDelay;
        }
        if( itDelay != delays.end() )
            (*itDelay).push_back( delay );
        else
        {
            std::vector< double > vec = { satId, at0, delay };
            delays.push_back( vec );
        }
        bool satKnown = false;
        std::vector< Signal >::iterator iter;
        for( iter = mSignals.begin(); !satKnown && iter != mSignals.end(); ++iter )
        {
            if( (*iter).getSatId() == satId && (*iter).getTimestamp() == at0 )
            {
                if (!(*iter).positionKnown( ax, ay, az ))
                {
                    (*iter).addGroundStation( ax, ay, az, at0, adt, delay );
                }
                satKnown = true; 
            }
        }
        if( !satKnown )
        {
            Signal mSignal( satId, at0 );
            mSignal.addGroundStation( ax, ay, az, at0, adt, delay ); 
            mSignals.push_back( mSignal );
        }
    }
    lFile.close();
}

/**
 * @brief Printing statistics of delays
 */
void stats()
{
    std::vector< std::vector< double > >::iterator itDelay;
    for( itDelay = delays.begin(); itDelay != delays.end(); ++itDelay )
    {   
        double max = 0, min = 10000, sum = 0;
        int satId = (*itDelay).at(0);
        double time = (*itDelay).at(1);
        for( int i=2; i<(*itDelay).size(); ++i )
        {
            double current = (*itDelay).at(i);
            sum += current;
            if( current > max ) 
                max = current;
            if( current < min )
                min = current;
        }
        std::cout << "Delay " << satId << " " << std::setprecision(20) << time << " " << min << " " << sum/((*itDelay).size()-2) << " " << max << std::endl;
    }
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

	std::cout << "loading data from " << lDirName << std::endl;
    std::string fileFormat(".gsd");
    if( dir == NULL )
    {
        std::cout << "ERROR: Problem with directory" << std::endl;
    }
    else
    {
    	std::cout << "Directory is not empty... " << std::endl;
        while( dirEnt = readdir( dir ) )
        {
            file = std::string(lDirName) + "/" + dirEnt->d_name;
			std::cout << "File " << file << std::endl;
            if( stat( file.c_str(), &filestat )) continue;
            if( S_ISDIR( filestat.st_mode ) ) continue;

            if( file.substr( file.size()-4, 4) == std::string(".gsd") )
            {
                std::cout << "Processing file: " << file << std::endl;
                loadGSData( file.c_str() );          
            }
            else
            {
                std::cout << "File " << file << " does not have proper format! " << std::endl;
            }

        }
        closedir( dir );
    }
    stats();
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

