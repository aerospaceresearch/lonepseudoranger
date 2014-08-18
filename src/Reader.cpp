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

        std::cout << "For satellite " << satId << " and sending time " << timestamp << " there are " << (*iter).getSize() << " GS" << std::endl;
        if( (*iter).getSize() >= 4 ) 
//        if( (*iter).getSize() >= 4 && timestamp<1000 ) //>=884 && timestamp<885 ) 
        //if( (*iter).getSize() >= 4 && timestamp<1313 && timestamp>1312 ) 
//        if( (*iter).getSize() >= 4 && timestamp>3220 && timestamp<3221 ) // 22 - b16
    //    if( (*iter).getSize() >= 4 && timestamp>2264 && timestamp<2265 ) // 23 - b9 
        {
            auto time1 = std::chrono::high_resolution_clock::now();
            std::cout << std::endl << "***********************************************************************************" << std::endl;
            if( (*iter).getSize() < 4)
                std::cout << "not taken sat" << (*iter).getSatId() << " with " << (*iter).getSize() << " gs" << std::endl;
            Stations mStations;
            (*iter).convertSignalToStation( mStations );
            std::vector< std::vector< int > > stationsComb;
            
            PositionsList xyzr;
            int N = mStations.size();
           // xyzr.addPositions( solveApol( satId, timestamp, mStations ) );

            if( N>6 )
            {
                if( N<20 )
                    std::cout << N << " " << (*iter).getTimestamp() << std::endl;
                int k = N-2;
                if( N<6 )
                    k=4;
                std::cout << "N=" << N << ", k=" << k << std::endl;
//                std::cout << "For satellite " << satId << ", "<< N << " ground stations and time of sending signal " << timestamp << " I want to set of satellites of size: ";
//                std::cin >> k;

                stationsComb = aCombinations.getStationsCombinations( N, k );
                std::vector< std::vector< int > >::iterator iterSt;
                int pos = 0;
                for( iterSt = stationsComb.begin(); iterSt != stationsComb.end(); ++iterSt )
                {
                    Stations aStations;
                    for( int i=0; i<k; ++i )
                    {
                        aStations.addStation( mStations.getStation( (*iterSt).at(i) ) );
//                        std::cout << (*iterSt).at(i) << " ";
                    }
            //        std::cout << std::endl;
                    xyzr.addPositions( solveApol( satId, timestamp, aStations, pos, false ) ); // pos - combination id
                    ++pos;
                }
                std::vector< int > selectedCombinations = cluster( xyzr, stationsComb, N );
                std::sort( selectedCombinations.begin(), selectedCombinations.end() );  

                std::vector< int > selectedGS;
                std::vector< int >::iterator itInt;
                Stations selectedStations;

                
//                std::cout << "Taken combinations: ";
                // selectedCombinations - combinations selected by clustering
                for( itInt = selectedCombinations.begin(); itInt != selectedCombinations.end(); ++itInt )
                {
                    // *itInt - number of combination which should be taken
                    // from stationsComb
                    std::vector< std::vector< int > >::iterator combIter;
                    //std::cout << std::endl << "combination " << *itInt << ": " ;
                    for( int counter = 0; counter < stationsComb.at(*itInt).size(); ++counter )
                    {
                        selectedGS.push_back( stationsComb.at(*itInt).at(counter) );
                      //  std::cout << stationsComb.at(*itInt).at(counter) << " ";
                    }
                }
                std::vector< int >::iterator intIt2;
                std::sort( selectedGS.begin(), selectedGS.end() );
                intIt2 = std::unique(selectedGS.begin(), selectedGS.end());
                selectedGS.resize(std::distance( selectedGS.begin(), intIt2 ) );

                if( selectedGS.size() < N )
                {
                    std::cout << "************!!!!!!!!!!!!!!! NOT ALL OF STATIONS TAKEN!!!! "<< satId << ", time: " << timestamp << std::endl;
                }
                std::cout << std::endl << "Selected stations: " << selectedGS.size() << "/" << N << " (" << timestamp << ")" << std::endl;
                for( intIt2 = selectedGS.begin(); intIt2 != selectedGS.end(); ++intIt2 )
                {
//                    std::cout << (*intIt2) << " " ;
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
    //        }
           
           /* 
            xyzr.printPositions();
            for( int i=0; i<xyzr.size(); ++i )
            {
                lResultFile << timestamp << " " << xyzr.getX( i ) << " " << xyzr.getY( i ) << " " << xyzr.getZ( i ) << "\n";
            }*/
        }
        else
        {
            std::cout << "Not taken sat " << satId << " with " << (*iter).getSize() << " gs." << std::endl;
            // TODO: trilateration for 3 gs
        }
    }
    lResultFile.close();
}
}
/*
void stats( std::vector< std::tuple< int, double, double > > delays, std::vector< int > satellites )
{
    std::vector< int >::iterator satIt;
    for( satIt = satellites.begin(); satIt != satellites.end(); ++satIt )
    {
        int satId = *satIt;
        double min=10000, max=0, mean=0, sum=0;
        int counter = 0;
        std::vector< std::tuple< int, double, double > >::iterator it;
        for( it=delays.begin(); it!=delays.end(); ++it )
        {
            if( std::get<0>( *it ) == satId )
            {
                double current_delay = std::get<2>( *it );
                if( current_delay < min )
                   min = current_delay;
                if( current_delay > max )
                   max = current_delay;
                sum += current_delay;
                ++counter;
            }
        }
        std::cout << "For satellite " << satId << " min delay: " << min << ", max delay: " << max << ", mean delay: " << sum/counter << std::endl;
        
    }
}*/

std::vector< std::vector< double > > delays;
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

    double ax, ay, az, delay;
    double at0, adt;
    int satId = 0; 
    std::string op1;
//    std::vector< std::tuple< int, double, double > > delays;
    std::vector< int > satellites;
    while( lFile >> ax >> ay >> az >> adt >> at0 >> satId >> delay ) // >> at0 >> satId >> op1  )
    {

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
        // check if satId is already in vector satellites
    /*    std::vector< int >::iterator satIt = satellites.begin();
        while( (*satIt) != satId && satIt!=satellites.end() )
        {
            ++satIt;
        }

        if( satIt==satellites.end() )
            satellites.push_back( satId );
        std::cout << lFileName << " " << at0 << ", x, y, z: " << ax << " " << ay << " " << az << std::endl;
*/
    //    std::cout << "loading signal " << satId << " " << at0 << std::endl;
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
                satKnown = true; // bez tego tez sie da
            }
        }

        if( !satKnown )
        {
            Signal mSignal;
            mSignal.setSatId( satId );
            mSignal.setTimestamp( at0 );
            mSignal.addGroundStation( ax, ay, az, at0, adt, delay ); 
            mSignals.push_back( mSignal );
        }
  //      delays.push_back( std::make_tuple( satId, at0, op ) );
    }
//    stats( delays, satellites );
    lFile.close();
}

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
        std::cout << "delay " << satId << " " << std::setprecision(20) << time << " " << min << " " << sum/((*itDelay).size()-2) << " " << max << std::endl;
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

    std::string fileFormat(".gsd");
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

            if( file.substr( file.size()-4, 4) == std::string(".gsd") )
            {
    //            std::cout << "Processing file: " << file << std::endl;
                loadGSData( file.c_str() );          
            }
            else
            {
      //          std::cout << "File " << file << " does not have proper format! " << std::endl;
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

