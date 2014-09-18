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
#include "Stats.h"

std::vector< int > cluster( PositionsList, std::vector< std::vector< int > >, int );
/**  
 * processing of all collected signals
 */
void processSignalData( std::string statInput = "", std::string statOutput = "", std::string directory = "" )
{
    bool generateStats = false;
    if( statInput != std::string("") && statOutput != std::string("") )
    {
        generateStats = true;
        std::cout << "generateStats = true" << std::endl;
    }

    std::cout << "********* PROCESSING SIGNALS *****************************" << std::endl;
    std::cout << "mSignals.size() = " << mSignals.size() << std::endl;

    Combinations aCombinations;

    Stats stats;
    double statsNbContacts = 0;
    double statsNbTracked = 0;

    std::vector< Signal >::iterator iter;
    for( iter = mSignals.begin(); iter != mSignals.end(); ++iter )
    {
        ++statsNbContacts;
        int satId = (*iter).getSatId();
        long double timestamp = (*iter).getTimestamp();

        std::cout << "For satellite " << satId << " and sending time " << timestamp << " there are " << (*iter).getSize() << " GS" << std::endl;
        if( (*iter).getSize() > 6 && (*iter).getSize()<64 ) //&& timestamp==0 && satId==0 ) // temporary
        {
            ++statsNbTracked;
            auto time1 = std::chrono::high_resolution_clock::now();
            (*iter).printSignal();
            std::cout << std::endl << "***********************************************************************************" << std::endl;
            if( (*iter).getSize() < 4)
                std::cout << "not taken sat" << (*iter).getSatId() << " with " << (*iter).getSize() << " gs" << std::endl;
            Stations mStations;
            (*iter).convertSignalToStation( mStations );
            std::vector< std::vector< int > > stationsComb;
            
            PositionsList xyzr;
            int N = mStations.size();
           // xyzr.addPositions( solveApol( satId, timestamp, mStations ) );

            SignalStats signalStats( satId, timestamp, N );
            if( N>=4 )
           // if( N>3 )
            {
                int k = 5;
                k = N-2;
                if( k<5 )
                    k=5;
                std::cout << "N=" << N << ", k=" << k << std::endl;

                std::cout << "For satellite " << satId << ", "<< N << " ground stations and time of sending signal " << timestamp << " I want to set of satellites of size: ";
//                std::cin >> k;


                // clustering:
                if( N>6 )
                {
                    signalStats.setClustered( true );
                    signalStats.setK( k );
                    stationsComb = aCombinations.getStationsCombinations( N, k );
                    std::vector< std::vector< int > >::iterator iterSt;
                    int pos = 0;
                    for( iterSt = stationsComb.begin(); iterSt != stationsComb.end(); ++iterSt )
                    {
                        Stations aStations;
                        for( int i=0; i<k; ++i )
                        {
                            aStations.addStation( mStations.getStation( (*iterSt).at(i) ) );
//                            std::cout << (*iterSt).at(i) << " ";
                        }
                //        std::cout << std::endl;
                        xyzr.addPositions( solveApol( satId, timestamp, aStations, pos, false ) ); // pos - combination id
                        ++pos;
                    }
                    std::cout << "stationsComb.size() = " << stationsComb.size() << std::endl;
                    std::vector< int > selectedCombinations = cluster( xyzr, stationsComb, N );
                    std::sort( selectedCombinations.begin(), selectedCombinations.end() );  

                    std::vector< int > selectedGS;
                    std::vector< int >::iterator itInt;
                    Stations selectedStations;

                
//                    std::cout << "Taken combinations: ";
                    // selectedCombinations - combinations selected by clustering
                    for( itInt = selectedCombinations.begin(); itInt != selectedCombinations.end(); ++itInt )
                    {
                        // *itInt - number of combination which should be taken
                        // from stationsComb
                        std::vector< std::vector< int > >::iterator combIter;
                        //std::cout << std::endl << "combination " << *itInt << ": " ;
                        std::cout << "*itInt = " << (*itInt) << std::endl;
                        for( int counter = 0; counter < stationsComb.at(*itInt).size(); ++counter )
                        {
                            std::cout << "pushing to selectedGS: " << stationsComb.at(*itInt).at(counter) << std::endl;
                            selectedGS.push_back( stationsComb.at(*itInt).at(counter) );
                          //  std::cout << stationsComb.at(*itInt).at(counter) << " ";
                        }
                    }
                    std::vector< int >::iterator intIt2;
                    std::sort( selectedGS.begin(), selectedGS.end() );
                    intIt2 = std::unique(selectedGS.begin(), selectedGS.end());
                    selectedGS.resize(std::distance( selectedGS.begin(), intIt2 ) );

                    if( selectedGS.size() == N )
                    {
                   //     signalStats.setClustered( false );
                        std::cout << "copy from calculated, time=" << timestamp << std::endl;
                        signalStats.copyFromCalculated();
                        signalStats.setClustered( true );
                    }
                    else if( selectedGS.size() == N-1 )
                    {
                        std::cout << "************!!!!!!!!!!!!!!! NOT ALL OF STATIONS TAKEN!!!! "<< satId << ", time: " << timestamp << std::endl;
                        std::cout << std::endl << "Selected stations: " << selectedGS.size() << "/" << N << " (" << timestamp << ")" << std::endl;
                        for( intIt2 = selectedGS.begin(); intIt2 != selectedGS.end(); ++intIt2 )
                        {
                            std::cout << (*intIt2) << " " ;
                            selectedStations.addStation( mStations.getStation( *intIt2 ) );
                        }
                        std::cout << std::endl;

                        std::cout << "Result after clustering: ";
                        //std::cout << "stats clustered: " << std::endl;
                        signalStats.setClusteredPositions( solveApol( satId, timestamp, selectedStations ) );
//                        if( generateStats )
 //                       {
                            std::vector< double > delaysAfter = selectedStations.printDelayStats( satId, timestamp );
                            signalStats.setMinDelayAfter( delaysAfter.at(0) );
                            signalStats.setMaxDelayAfter( delaysAfter.at(1) );
                            signalStats.setMeanDelayAfter( delaysAfter.at(2) );
                            signalStats.setNbGSafter( selectedGS.size() );
                            std::cout << "Reader: mNbGSafter: " << selectedGS.size() << std::endl;
//                        }
                   // } 
                }
                auto time2 = std::chrono::high_resolution_clock::now();
                std::cout << "Time of processing signal with timestamp " << (*iter).getTimestamp() << ": " << std::chrono::duration_cast< std::chrono::milliseconds >( time2-time1 ).count() << " milliseconds. N=" << N << std::endl;
            
                std::cout << "Result without clustering: ";
               // std::cout << "stats calculated: " << std::endl;
                signalStats.setCalculatedPositions( solveApol( satId, timestamp, mStations ) );
                if( generateStats )
                {
                    std::vector< double > delaysBefore = mStations.printDelayStats( satId, timestamp );
                    signalStats.setMinDelayBefore( delaysBefore.at(0) );
                    signalStats.setMaxDelayBefore( delaysBefore.at(1) );
                    signalStats.setMeanDelayBefore( delaysBefore.at(2) );
                    signalStats.setNbGSbefore( mStations.size() );
                    std::cout << "Reader: mNbGSbefore = " << mStations.size() << std::endl;
                    std::cout << "generate stats = true" << std::endl;
                    stats.push_back( signalStats );
                }
           // }
        }
    //        } 
            }
            else
            {
                std::cout << "Not taken sat " << satId << " with " << (*iter).getSize() << " gs." << std::endl;
                // TODO: trilateration for 3 gs
            }
        }
    }

    if( generateStats )
    {   
        stats.setNbContacts( statsNbContacts );
        stats.setNbTracked( statsNbTracked );
        stats.readBeaconFile( statInput );
        stats.calculateStats();
        stats.printStats( statOutput, directory );
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
    double at0, at1, adt;
    int satId = 0; 
    std::string op1;
//    std::vector< std::tuple< int, double, double > > delays;
    std::vector< int > satellites;
    while( lFile >> ax >> ay >> az >> at1 >> at0 >> op1 /* satId*/ >> delay ) // >> at0 >> satId >> op1  )
    //while( lFile >> ax >> ay >> az >> at0 >> at1 >> satId >> delay ) // >> at0 >> satId >> op1  )
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
//        std::cout << "delay " << satId << " " << std::setprecision(20) << time << " " << min << " " << sum/((*itDelay).size()-2) << " " << max << std::endl;
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

