#ifndef SIGNAL_STATS_H
#define SIGNAL_STATS_H

#include <fstream>
#include <tuple>
#include <vector>
#include <cmath>
#include <string>

#include "Def.h"

// x, y, z, t, satId
typedef std::tuple< double, double, double, double, int > PrecPosition;

class SignalStats
{
    friend class Stats;
public:
    SignalStats( int aSatId, double aTimestamp, int N ) 
    : mSatId( aSatId )
    , mTimestamp( aTimestamp )
    , mN( N )
    , mK( 0 )
    {
        if( N<=5 )
        {
            mClustered = false;
            mRMSafter = 0;
            mAverAfter = 0;
            mNbGSafter = 0;
            mMinDelayAfter = 0;
            mMaxDelayAfter = 0;
            mMeanDelayAfter = 0;
        }
        else 
            mClustered = true;
    //    mPrecisePosition = std::make_tuple( 0, 0, 0, 0, 0 );
    }
    
    void addPresice( std::string aStatInput );

    void printStats( std::string outputStats = "" );

    double getTimestamp() { return mTimestamp; }
    int getSatId() { return mSatId; }

    void setN( int n )
    {
        mN = n;
    }

    void setK( int k )
    {
        mK = k;
    }
    void setClusteredPositions( PositionsList aClusteredPositions )
    {
        mClusteredPositions = aClusteredPositions;
        std::cout << "setting " << aClusteredPositions.size() << " clustered positions: " << std::endl;
    }

    void setCalculatedPositions( PositionsList aCalculatedPositions )
    {
        mCalculatedPositions = aCalculatedPositions;
    }

    void setPrecisePosition( PrecPosition aPrecisePosition )
    {
        mPrecisePosition = aPrecisePosition;
    }

    void setMinDelayBefore( double aMinDelayBefore )
    {
        mMinDelayBefore = aMinDelayBefore;
    }

    void setMinDelayAfter( double aMinDelayAfter )
    {
        mMinDelayAfter = aMinDelayAfter;
    }

    void setMaxDelayBefore( double aMaxDelayBefore )
    {
        mMaxDelayBefore = aMaxDelayBefore;
    }

    void setMaxDelayAfter( double aMaxDelayAfter )
    {
        mMaxDelayAfter = aMaxDelayAfter;
    }

    void setMeanDelayBefore( double aMeanDelayBefore )
    {
        mMeanDelayBefore = aMeanDelayBefore;
    }

    void setMeanDelayAfter( double aMeanDelayAfter )
    {
        mMeanDelayAfter = aMeanDelayAfter;
    }

    void setNbGSbefore( int aNbGSbefore )
    {
        mNbGSbefore = aNbGSbefore;
    }

    void setNbGSafter( int aNbGSafter )
    {
        mNbGSafter = aNbGSafter;
    }

    double difference( PrecPosition aPrec, Position aCalc );

    void calculateStats();
    PositionsList getClusteredPositions() { return mClusteredPositions; } 
    PositionsList getCalculatedPositions() { return mCalculatedPositions; } 

private:
    int mSatId;
    double mTimestamp;
    bool mClustered;
    int mN;
    int mK;
    PositionsList mClusteredPositions;
    PositionsList mCalculatedPositions;
    PrecPosition mPrecisePosition;
    std::vector< double > diffCalculated;
    std::vector< double > diffClustered;
    double mRMSbefore = 0;
    double mRMSafter = 0;
    double mAverBefore = 0;
    double mAverAfter = 0;
    int mNbGSbefore = 0;
    int mNbGSafter = 0;
    double mMinDelayBefore = 0;
    double mMinDelayAfter = 0;
    double mMaxDelayBefore = 0;
    double mMaxDelayAfter = 0;
    double mMeanDelayBefore = 0;
    double mMeanDelayAfter = 0;
};

class Stats
{
  public:
    void push_back( SignalStats aSignal )
    {
        mSignals.push_back( aSignal );
    }

    void readBeaconFile( std::string beaconName )
    {
        double epsilon = 0.0000001;
        std::fstream precFile;
        precFile.open( beaconName.c_str(), std::ios::in );
        if( precFile.is_open() )
            std::cout << "STATS_INPUT open! " << std::endl;

        std::vector< PrecPosition > precPos;
        double x,y,z,t;
        int s;
        while( precFile >> x >> y >> z >> t >> s)
        {   
        //        std::cout << x << " " << y << " " << z << " " << t << "\n";
            auto pos = std::make_tuple( x, y, z, t, s );
            precPos.push_back( pos );
        }
        mNbSignals = precPos.size();
        precFile.close();
        std::cout << " STATS_INPUT closed!" << std::endl;
    
        std::vector< SignalStats >::iterator iter;
        std::cout << "mSignals.size() = " << mSignals.size() << std::endl;
        for( iter = mSignals.begin(); iter != mSignals.end(); ++iter )
        {
            std::vector< PrecPosition >::iterator iter2 = precPos.begin();
            while( ( abs( (*iter).getTimestamp() - std::get<3>( *iter2 ))>epsilon || (*iter).getSatId() != (std::get<4>( *iter2 )) ) && iter2 != precPos.end() )
            {
                ++iter2;
            }

            if( iter2 != precPos.end() )
            {
                (*iter).setPrecisePosition( *iter2 );
            }
            else
            {
                std::cout << "Error in looking for position for timestamp: " << (*iter).getTimestamp() << std::endl;
            }
        }
    }

    void calculateStats()
    {
        if( mSignals.empty() )
            std::cout << "There is no stats for calculations!!!" << std::endl;
        else
        {
            std::vector< SignalStats >::iterator it;
            for( it = mSignals.begin(); it != mSignals.end(); ++it )
            {
                (*it).calculateStats();
            }
        }
    }
    
    void setNbSignals( int aNbSignals )
    {
        mNbSignals = aNbSignals;
    }

    void setNbContacts( int aNbContacts )
    {
        mNbContacts = aNbContacts;
    }

    void printStats( std::string outputStats, std::string directory )
    {
        double mAverRMSbeforeClOnly = 0;
        double mAverRMSafterClOnly = 0;
        double mAverDiffBeforeClOnly = 0;
        double mAverDiffAfterClOnly = 0;
        double mAverNbGSbeforeClOnly = 0;
        double mAverNbGSafterClOnly = 0;
        double mAverMinDelayBeforeClOnly = 0;
        double mAverMinDelayAfterClOnly = 0;
        double mAverMaxDelayBeforeClOnly = 0;
        double mAverMaxDelayAfterClOnly = 0;
        double mAverMeanDelayBeforeClOnly = 0;
        double mAverMeanDelayAfterClOnly = 0;


        if( mSignals.empty() )
            std::cout << "There is no stats!!!" << std::endl;
        else
        {
            int n = mSignals.size();
            
            std::fstream fileStats;
            fileStats.open( outputStats.c_str(), std::ios::out | std::ofstream::app );
            if( !fileStats.is_open() )
                std::cout << "STATS FILE CLOSED!" << std::endl;

            fileStats << "Satellite; Timestamp; N; k; Clustered; RMS_before; RMS_after; Diff_before; Diff_after; Nb_GS_before; Nb_GS_after; ";
            fileStats << "Min_delay_before; Min_delay_after; Max_delay_before; Max_delay_after; Mean_delay_before; Mean_delay_after;\n ";
            fileStats.close();

            std::vector< SignalStats >::iterator it;
            for( it = mSignals.begin(); it != mSignals.end(); ++it )
            {
                std::cout << "stats time: " << (*it).getTimestamp() <<  " " << ((*it).getClusteredPositions()).size() << " " <<  ((*it).getCalculatedPositions()).size() << ", n=" << n << std::endl;
                (*it).printStats( outputStats );

                std::cout << "mNb = " << (*it).mNbGSbefore << " " << (*it).mNbGSafter << std::endl;
                mAverRMSbefore += (*it).mRMSbefore;
                mAverDiffBefore += (*it).mAverBefore;
                mAverNbGSbefore += (double)(*it).mNbGSbefore;
                mAverMinDelayBefore += (*it).mMinDelayBefore;
                mAverMaxDelayBefore += (*it).mMaxDelayBefore;
                mAverMeanDelayBefore += (*it).mMeanDelayBefore;

                if( (*it).mClustered )
                {
                    ++mNbClustered;
                    mAverRMSafter += (*it).mRMSafter;
                    mAverDiffAfter += (*it).mAverAfter;
                    mAverNbGSafter += (double)(*it).mNbGSafter;
                    mAverMinDelayAfter += (*it).mMinDelayAfter;
                    mAverMaxDelayAfter += (*it).mMaxDelayAfter;
                    mAverMeanDelayAfter += (*it).mMeanDelayAfter;

                    mAverRMSbeforeClOnly += (*it).mRMSbefore;
                    mAverDiffBeforeClOnly += (*it).mAverBefore;
                    mAverNbGSbeforeClOnly += (double)(*it).mNbGSbefore;
                    mAverMinDelayBeforeClOnly += (*it).mMinDelayBefore;
                    mAverMaxDelayBeforeClOnly += (*it).mMaxDelayBefore;
                    mAverMeanDelayBeforeClOnly += (*it).mMeanDelayBefore;

                    mAverRMSafterClOnly += (*it).mRMSafter;
                    mAverDiffAfterClOnly += (*it).mAverAfter;
                    mAverNbGSafterClOnly += (double)(*it).mNbGSafter;
                    mAverMinDelayAfterClOnly += (*it).mMinDelayAfter;
                    mAverMaxDelayAfterClOnly += (*it).mMaxDelayAfter;
                    mAverMeanDelayAfterClOnly += (*it).mMeanDelayAfter;
                }
                else
                {
                    mAverRMSafter += (*it).mRMSbefore;
                    mAverDiffAfter += (*it).mAverBefore;
                    mAverNbGSafter += (double)(*it).mNbGSbefore;
                    mAverMinDelayAfter += (*it).mMinDelayBefore;
                    mAverMaxDelayAfter += (*it).mMaxDelayBefore;
                    mAverMeanDelayAfter += (*it).mMeanDelayBefore;

                }
            }
            mAverRMSbefore /= n;
            mAverDiffBefore /= n;
            mAverNbGSbefore /= n;
            mAverMinDelayBefore /= n;
            mAverMaxDelayBefore /= n;
            mAverMeanDelayBefore /= n;

            mAverRMSafter /= n;
            mAverDiffAfter /= n;
            mAverNbGSafter /= n;
            mAverMinDelayAfter /= n;
            mAverMaxDelayAfter /= n;
            mAverMeanDelayAfter /= n;

            if( mNbClustered != 0 )
            {
                mAverRMSbeforeClOnly /= mNbClustered;
                mAverDiffBeforeClOnly /= mNbClustered;
                mAverNbGSbeforeClOnly /= mNbClustered;
                mAverMinDelayBeforeClOnly /= mNbClustered;
                mAverMaxDelayBeforeClOnly /= mNbClustered;
                mAverMeanDelayBeforeClOnly /= mNbClustered;

                mAverRMSafterClOnly /= mNbClustered;
                mAverDiffAfterClOnly /= mNbClustered;
                mAverNbGSafterClOnly /= mNbClustered;
                mAverMinDelayAfterClOnly /= mNbClustered;
                mAverMaxDelayAfterClOnly /= mNbClustered;
                mAverMeanDelayAfterClOnly /= mNbClustered;
            }
        }

        std::string fileName("stats.csv");
        std::fstream file;
        file.open( fileName.c_str(), std::ios::out | std::ofstream::app );
        file << directory << "; " << mAverRMSbefore << "; " << mAverRMSafter << "; ";
        file << mAverRMSbeforeClOnly << "; " << mAverRMSafterClOnly << "; ";

        file << mAverDiffBefore << "; " << mAverDiffAfter << "; ";
        file << mAverDiffBeforeClOnly << "; " << mAverDiffAfterClOnly << "; ";

        file << mNbSignals << "; " << mNbContacts << "; " << mNbClustered << "; ";
        file << mAverNbGSbefore << "; " <<  mAverNbGSafter << "; ";
        file <<  mAverMinDelayBefore << "; " << mAverMinDelayAfter << "; ";
        file << mAverMaxDelayBefore << "; " << mAverMaxDelayAfter << "; " << mAverMeanDelayBefore << "; " << mAverMeanDelayAfter << ";\n";
    }

  private:
    std::vector< SignalStats > mSignals;
    double mAverRMSbefore = 0;
    double mAverRMSafter = 0;
    double mAverDiffBefore = 0;
    double mAverDiffAfter = 0;
    int mNbSignals = 0;
    int mNbContacts = 0;
    int mNbClustered = 0;
    double mAverNbGSbefore = 0;
    double mAverNbGSafter = 0;
    double mAverMinDelayBefore = 0;
    double mAverMinDelayAfter = 0;
    double mAverMaxDelayBefore = 0;
    double mAverMaxDelayAfter = 0;
    double mAverMeanDelayBefore = 0;
    double mAverMeanDelayAfter = 0;

};
#endif // SIGNAL_STATS_H
