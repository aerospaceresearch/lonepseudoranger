#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cmath>

#include "Stats.h"

void SignalStats::addPresice( std::string aStatInput )
{

}

void SignalStats::copyFromCalculated()
{

}

void SignalStats::calculateStats()
{
    //mNbGSbefore = mN;
    //mNbGSbefore = mCalculatedPositions.size();
    mRMSbefore = 0, mRMSafter = 0;
    mAverBefore = 0, mAverAfter = 0;
    std::cout << "mNbGSbefore: " << mNbGSbefore << std::endl;
    std::cout << "mNbGSafter: " << mNbGSafter << std::endl;
    for( int i=0; i<mCalculatedPositions.size(); ++i )
    //for( int i=0; i<mNbGSbefore; ++i )
    {
        double diff =  difference( mPrecisePosition, mCalculatedPositions.getPosition( i ) );
        diffCalculated.push_back( diff );
        mRMSbefore += pow( diff, 2 );
        mAverBefore += diff;
        std::cout << "Difference: before+= " << diff << std::endl;
        std::cout << "stats_diff: mCalculated(" << i << ") = " << diff << std::endl;
    }
    std::cout << "mCalculatedPositions.size() = " << mCalculatedPositions.size() << ", mNbGSbefore = " << mNbGSbefore << std::endl;
//    mRMSbefore = sqrt( mRMSbefore/mNbGSbefore ); // TODO workaround 
 //   mAverBefore = mAverBefore/mNbGSbefore;
    
    //mNbGSafter = mClusteredPositions.size();
    for( int i=0; i<mClusteredPositions.size(); ++i )
    {
        double diff =  difference( mPrecisePosition, mClusteredPositions.getPosition( i ) );
        diffClustered.push_back( diff );
        mRMSafter += pow( diff, 2 );
        mAverAfter += diff;
        std::cout << "Difference: after+= " << diff << std::endl;
        std::cout << "stats_diff: mClustered(" << i << ") = " << diff << std::endl;
    }
    std::cout << "mClusteredPositions.size() = " << mClusteredPositions.size() << ", mNbGSafter = " << mNbGSafter << std::endl;
//    mRMSafter = sqrt( mRMSafter/mNbGSafter );
//    mAverAfter = mAverAfter/mNbGSafter;

    std::cout << "Difference2: time=" << mTimestamp << ". mAverBefore = " << mAverBefore << ", mAverAfter = " << mAverAfter << std::endl;
}
    
double SignalStats::difference( PrecPosition aPrec, Position aCalc )
{
   double x1 = std::get<0>( aPrec ); 
   double y1 = std::get<1>( aPrec ); 
   double z1 = std::get<2>( aPrec ); 

   double x2 = aCalc.at(0);
   double y2 = aCalc.at(1);
   double z2 = aCalc.at(2);

   double dx = x1-x2;
   double dy = y1-y2;
   double dz = z1-z2;

   double r2 = dx*dx + dy*dy + dz*dz;
   double r = sqrt( r2 );

//   std::cout << mTimestamp << " Difference between: " << x1 << ", " << y1 << ", " << z1 << " and " << x2 << ", " << y2 << ", " << z2 << " = " << r << std::endl;
   return r;
}

void SignalStats::printStats( std::string outputStats )
{
    bool toFile;
    if( outputStats != std::string("") )
        toFile = true;
    else
        toFile = false;

    if( toFile )
    {
        std::cout << "statsfile: " << outputStats << std::endl;
        std::fstream fileStats;
        fileStats.open( outputStats.c_str(), std::ios::out | std::ofstream::app );
        if( !fileStats.is_open() )
            std::cout << "STATS FILE CLOSED!" << std::endl;

//        fileStats << "Satellite; Timestamp; N; k; Clustered; Difference_before; Difference_after; Min_delay_before; Min_delay_after; Max_delay_before; Max_delay_after; Mean_delay_before; Mean_delay_after;\n ";

        double diffAfter;
        double diffBefore;

        double sumClustered = 0, sumCalculated = 0;
 
        std::vector< double >::iterator it;
        for( it = diffCalculated.begin(); it != diffCalculated.end(); ++it )
        {
            sumCalculated += *it;
        }
        diffBefore = sumCalculated/diffCalculated.size();

        if( mClustered )
        {
            for( it = diffClustered.begin(); it != diffClustered.end(); ++it )
                sumClustered += *it;
            if( diffClustered.size() < 1 )
            {
                diffAfter = diffBefore;
                mMinDelayAfter = mMinDelayBefore;
                mMeanDelayAfter = mMeanDelayBefore;
                mMaxDelayAfter = mMaxDelayBefore;
                mNbGSafter = mNbGSbefore;
            }
            else
            {
                diffAfter = sumClustered/diffClustered.size();
            }
        }
        else
        {
    //        diffAfter = -1;
            diffAfter = diffBefore;
            mMinDelayAfter = mMinDelayBefore;
            mMeanDelayAfter = mMeanDelayBefore;
            mMaxDelayAfter = mMaxDelayBefore;
        }
        mAverAfter = diffAfter;

        // printing stats for one signal:
        fileStats << mSatId << ";" << mTimestamp << ";" << mN << ";" << mK << ";" << (int)mClustered << ";";
        fileStats << std::setprecision(15) << diffBefore << ";" << diffAfter << ";" << mNbGSbefore << ";" << mNbGSafter << ";" ;
        fileStats << mMinDelayBefore << ";" << mMeanDelayBefore << ";" << mMaxDelayBefore << ";" << mMinDelayAfter << ";" << mMeanDelayAfter << ";" << mMaxDelayAfter << ";\n";
        fileStats.close();
    }
}
