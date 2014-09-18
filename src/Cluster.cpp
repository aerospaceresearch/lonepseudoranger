#include <utility>
#include <list> 
#include <vector>
#include <cmath>
#include <algorithm>

#include "Def.h"


class Edge
{
    public: 
    Edge() 
        : mIndexFirst( 0 )
        , mIndexSecond( 0 )
        , mDistance( 0 )
    {}
   
    Edge( uint16_t aIndexFirst, uint16_t aIndexSecond, double aDistance )
        : mIndexFirst( aIndexFirst )
        , mIndexSecond( aIndexSecond )
        , mDistance( aDistance )
    {}
    void printEdge() 
    {
        std::cout << mIndexFirst << "<->" << mIndexSecond << ": " << mDistance << std::endl;
    }
    uint16_t mIndexFirst;
    uint16_t mIndexSecond;
    double mDistance;
};

bool compareEdges( const Edge& first, const Edge& second )
{
    if( first.mDistance > second.mDistance )
        return true;
    else
        return false;
}

void printPosition( Position aPos )
{
    std::cout << aPos.at(0) << " " << aPos.at(1) << " " << aPos.at(2) << " ";
}

class Cluster
{
  public:
    Cluster() : mId( 0 ) { mPoints.push_back( 0 ); }
    Cluster( int aId ) : mId( aId ){}
    void addPoint( int point ) { mPoints.push_back( point ); }
    void addVector( std::vector< int > points ) 
    { 
        std::vector< int >::iterator it;
        for( it = points.begin(); it != points.end(); ++it )
        {
            mPoints.push_back( *it );
        }
    }

    int mId;
    std::vector< int > mPoints;
};

void printClusters( std::vector< Cluster > mClusters )
{
    std::vector< Cluster >::iterator itClust;
    int maxSize = 0; 
    int maxSizeIndex;
    int emptyClusters = 0;
    int summedPoints = 0;
    for( itClust = mClusters.begin(); itClust != mClusters.end(); ++itClust )
    {
        std::vector< int >::iterator itInt2;
        double x=0, y=0, z=0;
        int n = (*itClust).mPoints.size();
        if( (*itClust).mId != 0 )
        {
            std::cout << "    Cluster " << (*itClust).mId << ": ";
            std::cout << " n=" << n << ": " ;/*<< ", position:" << x/n << " " << y/n << " " << z/n << std::endl;*/
            summedPoints += n;
            for( itInt2 = (*itClust).mPoints.begin(); itInt2 != (*itClust).mPoints.end(); ++itInt2 )
            {
                std::cout << *itInt2 << " ";
            }
            std::cout << std::endl;

        }
        else
            ++emptyClusters;
        if( n > maxSize )
        {
            maxSizeIndex = (*itClust).mId;
            maxSize = n;
        }
    }
    std::cout << " taken points: " << summedPoints << ", maxSize: " << maxSize << " (index: " << maxSizeIndex << ")" << std::endl;
}

std::vector< int > cluster( PositionsList aList, std::vector< std::vector< int > > aCombinations, int nbGS )
{
    std::vector< bool > usedGs( nbGS, false );
    int usedGsCounter = 0;

    bool debug = false;
    //debug = true;
    std::vector< int > ret;
    const int N = aList.size();
    //const int N = std::min(aList.size(), 1000 );
    std::cout << std::endl << "CLUSTERING " << N << " COMBINATIONS" << std::endl;
    std::vector< int > clusterId;
    clusterId.insert( clusterId.begin(), N, 0 );
    std::vector< int >::iterator iter;
    const uint32_t nbEdges = N*(N-1)/2;
    std::cout << "N=" << N << ", nbEdges=" << nbEdges << std::endl;
/*    for( iter = clusterId.begin(); iter != clusterId.end(); ++iter )
        std::cout << (*iter) << " ";
    std::cout << std::endl;
*/
    double distance;
    std::vector< Edge > mEdges;
//    Edge* p;
//    p = mEdges.get_allocator().allocate( nbEdges );
    std::cout << "nbEdges: " << nbEdges << " ";
    if( mEdges.max_size() < nbEdges )
        std::cout << "ERROR! NOT ENOUGH MEMORY!" << std::endl;
    std::cout << "resizing vector... ";
    mEdges.resize( nbEdges );
//    mEdges.reserve( nbEdges );
    std::cout << "vector allocated! ";
    int counter = 0;
    for( int i=0; i<N-1; ++i )
        for( int j=i+1; j<N; ++j )
        {
            distance = aList.getDistance( i, j );
    //        std::cout << "distance=" << distance << " ";
            mEdges[counter++] = Edge( i, j, distance ) ;       
            //mEdges[counter++] push_back( Edge( i, j, distance ) );       
        }

    std::vector< Edge >::iterator it;
    std::cout << "distances calculated. sorting... ";
    std::sort( mEdges.begin(), mEdges.end(), compareEdges );
    std::cout << "sorting done! " << std::endl;
    double shortDistance = mEdges.back().mDistance; 
    double longDistance = mEdges.front().mDistance; 
    //debug = true;
    counter = 0;
    int limit_printed_dist = 20;
    if( debug )
    {
        std::cout << "List of distances: " << std::endl;
        for( it=mEdges.end()-1; it >= mEdges.begin() && limit_printed_dist>0 ; --it )
        {
            --limit_printed_dist;
            std::cout << ++counter << " ";
            (*it).printEdge();
            std::cout << "          ";
            printPosition( aList.getPosition( (*it).mIndexFirst ) );
            std::cout << " <-> ";
            printPosition( aList.getPosition( (*it).mIndexSecond ) );
            std::cout << std::endl;
        }
    }
    std::vector< Cluster > mClusters;
    int readyPoints = 0;
    int nbClusters = 0;
    int reduced = 0;
    counter = 0;
    std::cout << std::endl;

    std::cout << "CREATING CLUSTERS!" << std::endl;
    std::vector< Cluster >::iterator itClust;
    int maxSizeClu;
    int maxSizeCluIndex;
    double acceptance;
    //if( N<10 )
        acceptance = 1;
   /* else if( N<20 )
        acceptance = 0.8;
    else 
        acceptance = 0.7;*/
//    while( readyPoints < acceptance*N && mEdges.back().mDistance<10000 )
//    while( usedGsCounter < nbGS-1 )
    std::vector< std::vector< bool > > mGSinClusters;
//    while( maxSizeClu < nbGS-2 )
    while( usedGsCounter < nbGS-1 )
    {
        debug = true;
        ++counter;
        if( debug )
        {
            std::cout << "step " << counter << ". " ;
        }
        bool print = true;
   //     std::cout << std::endl;
        //mEdges.back().printEdge();
        int firstPoint = mEdges.back().mIndexFirst;
        int secondPoint = mEdges.back().mIndexSecond;
        if( debug )
        {
            std::cout << "  processing " << firstPoint << "<->" << secondPoint << ": " << mEdges.back().mDistance << std::endl;
        }
        // both points are not clustered
        if( clusterId.at( firstPoint ) == 0 && clusterId.at( secondPoint ) == 0 ) 
        {
            std::vector< bool > vectFalse( nbGS, false );
            mGSinClusters.push_back( vectFalse );
            if( debug )
            {
                mEdges.back().printEdge();
                std::cout << "--> adding points: " << firstPoint << " and " << secondPoint << std::endl;
            }
            Cluster aCluster( ++nbClusters );
            aCluster.addPoint( firstPoint );
            aCluster.addPoint( secondPoint );
            mClusters.push_back( aCluster );
            clusterId.at( firstPoint ) = nbClusters;
            clusterId.at( secondPoint ) = nbClusters;
            readyPoints += 2;
            std::vector< int >::iterator itComb;
            std::cout << "firstPoint=" << firstPoint << ", aCombinations.size()=" << aCombinations.size() << std::endl;
            std::cout << "secondPoint=" << secondPoint << ", aCombinations.size()=" << aCombinations.size() << std::endl;
            std::cout << "Adding: ";
            for( itComb = (aCombinations.at(firstPoint)).begin(); itComb != (aCombinations.at(firstPoint)).end(); ++itComb )
            {
                (mGSinClusters.at( nbClusters-1 )).at( *itComb ) = true;
                std::cout << *itComb << " ";
                if( !usedGs.at(*itComb) )
                {
                    std::cout << "(first time used) ";
                    usedGs.at(*itComb) = true;
                    ++usedGsCounter;
                }
            }
            std::cout << std::endl << "Adding: ";
            for( itComb = (aCombinations.at(secondPoint)).begin(); itComb != (aCombinations.at(secondPoint)).end(); ++itComb )
            {
                (mGSinClusters.at( nbClusters-1 )).at( *itComb ) = true;
                std::cout << *itComb << " ";
                if( !usedGs.at(*itComb) )
                {
                    std::cout << "(first time used) ";
                    usedGs.at(*itComb) = true;
                    ++usedGsCounter;
                }
            }
            std::cout << std::endl;
        }
        else if( clusterId.at( firstPoint ) == 0 || clusterId.at( secondPoint ) == 0 )
        {
            if( debug )
            {
                mEdges.back().printEdge();
            }
            int addingPoint = clusterId.at( firstPoint )==0 ? firstPoint : secondPoint;
            int currentCluster = std::max( clusterId.at( firstPoint ), clusterId.at( secondPoint ) );
            if( debug )
            {
                std::cout << "--> adding point " << addingPoint << " to cluster " << currentCluster << std::endl;
            }
            mClusters.at( currentCluster-1 ).addPoint( addingPoint );
            clusterId.at( addingPoint ) = currentCluster;
            ++readyPoints;
            std::vector< int >::iterator itComb;
            std::cout << "Adding: ";
            for( itComb = (aCombinations.at(addingPoint)).begin(); itComb != (aCombinations.at(addingPoint)).end(); ++itComb )
            {
                std::cout << *itComb << " ";
                (mGSinClusters.at( currentCluster-1 )).at( *itComb ) = true;
                if( !usedGs.at(*itComb) )
                {
                    std::cout << "(first time used) ";
                    usedGs.at(*itComb) = true;
                    ++usedGsCounter;
                }
            }
            std::cout << std::endl;
        }
        else
        { 
            if( clusterId.at( firstPoint ) != clusterId.at( secondPoint ) )
            {
                if( debug )
                {
                    mEdges.back().printEdge();
                }
                int currentCluster = std::min( clusterId.at( firstPoint ), clusterId.at( secondPoint ) );
                int movingCluster = std::max( clusterId.at( firstPoint ), clusterId.at( secondPoint ) );
                if( debug )
                {
                    std::cout << "--> joining clusters: " << currentCluster << " and " << movingCluster << std::endl;
                }
                mClusters.at( currentCluster-1 ).addVector( mClusters.at( movingCluster-1 ).mPoints );

                std::vector< int >::iterator intIt;
                for( intIt = mClusters.at( movingCluster-1 ).mPoints.begin(); intIt != mClusters.at( movingCluster-1 ).mPoints.end(); ++intIt )
                {
                    clusterId.at( *intIt ) = currentCluster;
                }
                mClusters.at( movingCluster-1 ).mId = 0;

                ++reduced;
            }
            else 
            {
                if( debug )
                {
                    std::cout << "--> both positions from the same cluster (" << clusterId.at( firstPoint ) << ")" << std::endl;
                }
                print = false;
            }
        }
        if( print )
        {
            std::cout << "usedGsCounter = " << usedGsCounter << std::endl;
            printClusters( mClusters );
        }
        
        maxSizeClu = 0;
        int ind = 0;
        std::vector< std::vector< bool > >::iterator itBool1; 
        std::cout << "nb of clusters: " << mGSinClusters.size() << std::endl;
        for( itBool1 = mGSinClusters.begin(); itBool1 != mGSinClusters.end(); ++itBool1 )
        {
            ++ind;
            std::cout << "Cluster " << ind << ": ";
            int nsize = 0;
            std::vector< bool >::iterator itBool2;
            int ind2 = 0;
            for( itBool2 = (*itBool1).begin(); itBool2 != (*itBool1).end(); ++itBool2 )
            {
                if( *itBool2 )
                {
                    std::cout << ind2 << " ";
                    ++nsize;
                }
                ++ind2;
            }
            if( nsize>maxSizeClu )
            {
                maxSizeClu = nsize;
                maxSizeCluIndex = ind;
            }
            std::cout << std::endl;
        }
        if( debug )
        {
            std::cout << "max size of cluster = " << maxSizeClu << " (cluster " << maxSizeCluIndex << ")" << std::endl;
        }
       mEdges.pop_back();
        if( debug )
        {
            std::cout << std::endl;
        }
    }
    std::cout << "The shortest distance: " << shortDistance << ", the longest: " << longDistance; 
    std::cout << ", first unused: " << mEdges.back().mDistance << ". Used: " << counter << "/" << nbEdges;
    std::cout << ", " << readyPoints << "/" << N << std::endl;

    int maxSize = 0; 
    int maxSizeIndex;
    int emptyClusters = 0;
    int summedPoints = 0;
    for( itClust = mClusters.begin(); itClust != mClusters.end(); ++itClust )
    {
        std::vector< int >::iterator itInt2;
        long double x=0, y=0, z=0;
        int n = (*itClust).mPoints.size();
        for( itInt2 = (*itClust).mPoints.begin(); itInt2 != (*itClust).mPoints.end(); ++itInt2 )
        {
            x += aList.getX( *itInt2 );
            y += aList.getY( *itInt2 );
            z += aList.getZ( *itInt2 );
//            std::cout << *itInt2 << " ";
        }
        if( (*itClust).mId != 0 )
        {
/*            std::cout << "Cluster " << (*itClust).mId << ": ";
            std::cout << " n=" << n << std::endl << "   ";
            if( debug )
            {
                for( itInt2 = (*itClust).mPoints.begin(); itInt2 != (*itClust).mPoints.end(); ++itInt2 )
                {
                    std::cout << *itInt2 << " ";
                }
                std::cout << std::endl;
            }*/
            summedPoints += n;
        }
        else
            ++emptyClusters;

        if( n > maxSize )
        {
            maxSizeIndex = (*itClust).mId;
            maxSize = n;
        }
    }

    maxSize = maxSizeClu;
    maxSizeIndex = maxSizeCluIndex;
    std::cout << "N=" << N << ". The best cluster: " << maxSizeIndex << " of size " << maxSize << std::endl;
    std::cout << "All clusters: " << mClusters.size() << ", empty clusters: " << emptyClusters << std::endl;

    PositionsList theBest;
    std::vector< int >::iterator itInt3; 
    for( itInt3 = mClusters.at( maxSizeIndex-1 ).mPoints.begin(); itInt3 != mClusters.at( maxSizeIndex-1 ).mPoints.end(); ++itInt3 )
    {
        ret.push_back( aList.getCombId( *itInt3 ) );

    }
    std::cout << "End of clustering!" << std::endl;
    return ret;
}
/*
int main()
{
    PositionsList list;
    std::vector< long double > pos1 = {503882.83343883759949, 4264513.1534340248622, 4264787.7220282863213};	
    std::vector< long double > pos2 = {564732.71876403203515, 4767576.1862530641238, 4767561.5908693681299};
    std::vector< long double > pos3 = {510549.81540419082182, 4262250.0116368334625, 4265095.9666724261479};
    std::vector< long double > pos4 = {564795.81967127418113, 4767571.8463724575308, 4767580.8135292027937};
    std::vector< long double > pos5 = {508682.00940381259818, 4264586.5077672573193, 4266581.8472906050179};
    std::vector< long double > pos6 = {564774.66796511342346, 4767557.1336597091567, 4767558.4464434829192};
    std::vector< long double > pos7 = {507076.10488872375956, 4263549.5509552511671, 4263790.7041408421805};
    std::vector< long double > pos8 = {564764.18040900702545, 4767585.5446454488811, 4767570.6326042477481};
    list.addPosition( pos1 );
    list.addPosition( pos2 );
    list.addPosition( pos3 );
    list.addPosition( pos4 );
    list.addPosition( pos5 );
    list.addPosition( pos6 );
    list.addPosition( pos7 );
    list.addPosition( pos8 );
    list.printPositions();

    PositionsList list2; 
    std::vector< long double > p1 = {10, 10, 0};
    std::vector< long double > p2 = {40, 10, 0};
    std::vector< long double > p3 = {20, 60, 0};
    std::vector< long double > p4 = {60, 60, 0};
    list2.addPosition( p1 );
    list2.addPosition( p2 );
    list2.addPosition( p3 );
    list2.addPosition( p4 );

    cluster( list );
    return 0;
}*/
