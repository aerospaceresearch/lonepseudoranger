#include <utility>
#include <list> 
#include <vector>
#include <cmath>

#include "Def.h"

class Edge
{
    public: 
    Edge( int aIndexFirst, int aIndexSecond, long double aDistance )
        : mIndexFirst( aIndexFirst )
        , mIndexSecond( aIndexSecond )
        , mDistance( aDistance )
    {}
    int mIndexFirst;
    int mIndexSecond;
    long double mDistance;
};

bool compareEdges( const Edge& first, const Edge& second )
{
    if( first.mDistance < second.mDistance )
        return true;
    else
        return false;
}

class Cluster
{
  public:
    Cluster( int aId ) : mId( aId ){}
    void addPoint( int point ) { mPoints.push_back( point ); }
    void addVector( std::vector< int > points ) 
    { 
        std::vector< int >::iterator it;
        for( it = points.begin(); it != points.end(); ++it )
            mPoints.push_back( *it );
    }

    int mId;
    std::vector< int > mPoints;
};

void cluster( PositionsList aList )
{
    const int N = std::min(aList.size(), 10000);
    std::cout << "clustering " << N << " positions" << std::endl;
    std::vector< int > clusterId;
    clusterId.insert( clusterId.begin(), N, 0 );
    std::vector< int >::iterator iter;
/*    for( iter = clusterId.begin(); iter != clusterId.end(); ++iter )
        std::cout << (*iter) << " ";
    std::cout << std::endl;
*/
    long double distance;
    std::list< Edge > mEdges;
    for( int i=0; i<N-1; ++i )
        for( int j=i+1; j<N; ++j )
        {
            distance = aList.getDistance( i, j );
  //          std::cout << i << " <-> " << j << ": " << distance << std::endl;
            mEdges.push_back( Edge( i, j, distance ) );       
        }

    mEdges.sort( compareEdges );
    std::cout << std::endl;
    std::list< Edge >::iterator it; 
/*    for( it = mEdges.begin(); it != mEdges.end(); ++it )
        std::cout << " " << (*it).mIndexFirst << " <-> " << (*it).mIndexSecond << ": " << (*it).mDistance << std::endl;
*/
    std::vector< Cluster > mClusters;
    int readyPoints = 0;
    int nbClusters = 0;
    while( readyPoints < N )
    {
        int firstPoint = mEdges.front().mIndexFirst;
        int secondPoint = mEdges.front().mIndexSecond;
//        std::cout << "  processing " << firstPoint << "<->" << secondPoint << std::endl;
        // both points are not clustered
        if( clusterId.at( firstPoint ) == 0 && clusterId.at( secondPoint ) == 0 ) 
        {
            Cluster aCluster( ++nbClusters );
            aCluster.addPoint( firstPoint );
            aCluster.addPoint( secondPoint );
            mClusters.push_back( aCluster );
            clusterId.at( firstPoint ) = nbClusters;
            clusterId.at( secondPoint ) = nbClusters;
            readyPoints += 2;
        }
        else if( clusterId.at( firstPoint ) == 0 || clusterId.at( secondPoint ) == 0 )
        {
            int addingPoint = clusterId.at( firstPoint )==0 ? firstPoint : secondPoint;
            int currentCluster = std::max( clusterId.at( firstPoint ), clusterId.at( secondPoint ) );
            mClusters.at( currentCluster-1 ).addPoint( addingPoint );
            clusterId.at( addingPoint ) = currentCluster;
            ++readyPoints;
        }
        else
        { 
            if( clusterId.at( firstPoint ) != clusterId.at( secondPoint ) )
            {
                int currentCluster = std::min( clusterId.at( firstPoint ), clusterId.at( secondPoint ) )-1;
                int movingCluster = std::max( clusterId.at( firstPoint ), clusterId.at( secondPoint ) )-1;
                mClusters.at( currentCluster ).addVector( mClusters.at( movingCluster ).mPoints );
                std::vector< int >::iterator intIt;

    
                for( intIt = mClusters.at( movingCluster ).mPoints.begin(); intIt != mClusters.at( movingCluster ).mPoints.end(); ++intIt )
                {
                    clusterId.at( *intIt ) = currentCluster;
                }
            }
        }
        mEdges.pop_front();
    }

    std::vector< Cluster >::iterator itClust;
    int maxSize = 0; 
    int maxSizeIndex;
    for( itClust = mClusters.begin(); itClust != mClusters.end(); ++itClust )
    {
        std::cout << "Cluster " << (*itClust).mId << ": ";
        std::vector< int >::iterator itInt2;
        long double x=0, y=0, z=0;
        int n = (*itClust).mPoints.size();
        for( itInt2 = (*itClust).mPoints.begin(); itInt2 != (*itClust).mPoints.end(); ++itInt2 )
        {
            x += aList.getX( *itInt2 );
            y += aList.getY( *itInt2 );
            z += aList.getZ( *itInt2 );
            std::cout << *itInt2 << " ";
        }
        std::cout << " n=" << n << ", position:" << x/n << " " << y/n << " " << z/n << std::endl;

        if( n > maxSize )
        {
            maxSizeIndex = (*itClust).mId;
            maxSize = n;
        }
    }

    std::cout << "The best cluster: " << maxSizeIndex << ": ";
    std::cout <<  aList.getX( maxSizeIndex ) << " " << aList.getY( maxSizeIndex ) << " " << aList.getZ( maxSizeIndex ) << std::endl;
 
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
