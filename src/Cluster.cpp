#include <utility>
#include <list> 
#include <vector>
#include <cmath>
#include <algorithm>

#include "Def.h"

/**
 * @brief Edge class
 */
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

/**
 * @brief Comparing length of two edges
 * @param aFirst first of edges
 * @param aSecond second of edges
 * @return true if first edge is longer
 */
bool compareEdges( const Edge& aFirst, const Edge& aSecond )
{
    if( aFirst.mDistance > aSecond.mDistance )
        return true;
    else
        return false;
}

/**
 * @brief Printing of given position
 * @param aPos position to print
 */
void printPosition( Position aPos )
{
    std::cout << aPos.at(0) << " " << aPos.at(1) << " " << aPos.at(2) << " ";
}

/**
 * @brief Cluster class
 */
class Cluster
{
  public:
    /**
     * @brief Constructor
     */
    Cluster() : mId( 0 ) { mPoints.push_back( 0 ); }
    /**
     * @brief Constructor
     * @param aId Id of Cluster
     */
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

    int mId; ///< Id of cluster
    std::vector< int > mPoints; ///< points in the cluster
};

/**
 * @brief Printing clusters from vector
 * @param mClusters vector of clusters
 */
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
            std::cout << " n=" << n << ": " ;
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

/**
 * @brief Clustering of given positions
 * @param aList list of position to cluster
 * @param aCombinations list of combinations
 * @param nbGS number of ground stations
 */
std::vector< int > cluster( PositionsList aList, std::vector< std::vector< int > > aCombinations, int nbGS )
{
    std::vector< bool > usedGs( nbGS, false );
    int usedGsCounter = 0;

    bool debug = false;
    std::vector< int > ret;
    const int N = aList.size();
    std::cout << std::endl << "CLUSTERING " << N << " COMBINATIONS" << std::endl;
    std::vector< int > clusterId;
    clusterId.insert( clusterId.begin(), N, 0 );
    std::vector< int >::iterator iter;
    const uint32_t nbEdges = N*(N-1)/2;
    std::cout << "N=" << N << ", nbEdges=" << nbEdges << std::endl;
    if( debug )
    {
        for( iter = clusterId.begin(); iter != clusterId.end(); ++iter )
        std::cout << (*iter) << " ";
        std::cout << std::endl;
    }
    double distance;
    std::vector< Edge > mEdges;
    std::cout << "Number of edges: " << nbEdges << " ";
    if( mEdges.max_size() < nbEdges )
        std::cout << "ERROR! NOT ENOUGH MEMORY!" << std::endl;
    if( debug )
    {
        std::cout << "resizing vector... ";
    }
    mEdges.resize( nbEdges );
    if( debug )
    {
        std::cout << "vector allocated! ";
    }
    int counter = 0;
    for( int i=0; i<N-1; ++i )
    {
        for( int j=i+1; j<N; ++j )
        {
            distance = aList.getDistance( i, j );
            mEdges[counter++] = Edge( i, j, distance ) ;       
        }
    }

    std::vector< Edge >::iterator it;
    if( debug )
    {
        std::cout << "distances calculated. sorting... ";
    }
    std::sort( mEdges.begin(), mEdges.end(), compareEdges );
    if( debug )
    {
        std::cout << "sorting done! " << std::endl;
    }
    double shortDistance = mEdges.back().mDistance; 
    double longDistance = mEdges.front().mDistance; 
    counter = 0;
    if( debug )
    {
        std::cout << "List of distances: " << std::endl;
        for( it=mEdges.end()-1; it >= mEdges.begin(); --it )
        {
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
    while( usedGsCounter < nbGS-1 )
    {
        ++counter;
        if( debug )
        {
            std::cout << "step " << counter << ". " ;
        }
        bool print = true;
        int firstPoint = mEdges.back().mIndexFirst;
        int secondPoint = mEdges.back().mIndexSecond;
        if( debug )
        {
            std::cout << "  processing " << firstPoint << "<->" << secondPoint << ": " << mEdges.back().mDistance << std::endl;
        }
        // both points are not clustered
        if( clusterId.at( firstPoint ) == 0 && clusterId.at( secondPoint ) == 0 ) 
        {
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
            for( itComb = (aCombinations.at(firstPoint)).begin(); itComb != (aCombinations.at(firstPoint)).end(); ++itComb )
            {
                if( !usedGs.at(*itComb) )
                {
                    usedGs.at(*itComb) = true;
                    ++usedGsCounter;
                }
            }
            for( itComb = (aCombinations.at(secondPoint)).begin(); itComb != (aCombinations.at(secondPoint)).end(); ++itComb )
            {
                if( !usedGs.at(*itComb) )
                {
                    usedGs.at(*itComb) = true;
                    ++usedGsCounter;
                }
            }
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
            for( itComb = (aCombinations.at(addingPoint)).begin(); itComb != (aCombinations.at(addingPoint)).end(); ++itComb )
            {
                if( !usedGs.at(*itComb) )
                {
                    usedGs.at(*itComb) = true;
                    ++usedGsCounter;
                }
            }
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
        if( print && debug )
        {
            printClusters( mClusters );
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

    std::vector< Cluster >::iterator itClust;
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
        }
        if( (*itClust).mId != 0 )
        {
            std::cout << "Cluster " << (*itClust).mId << ": ";
            std::cout << " n=" << n << std::endl << "   ";
            if( debug )
            {
                for( itInt2 = (*itClust).mPoints.begin(); itInt2 != (*itClust).mPoints.end(); ++itInt2 )
                {
                    std::cout << *itInt2 << " ";
                }
                std::cout << std::endl;
            }
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
