#ifndef DEF_H
#define DEF_H

#include <iostream>
#include <vector>
#include <array> 
#include <string>
#include <tuple>

/// Row type
typedef std::vector< double > Row;
/// Matrix type
//typedef std::vector< Row > Matrix;
/**
 * @brief Station class
 */
class Station
{
  public:
    /**
     * @brief Constructor.
     * @param ax coordinate x
     * @param ay coordinate y
     * @param az coordinate z
     * @param at time of receiving signal
     */
    Station( double ax, double ay, double az, long double at )
    : x( ax )
    , y( ay )
    , z( az )
    , t( at )
    {
    }

    /**
     * @brief Constructor.
     * @param ax coordinate x
     * @param ay coordinate y
     * @param az coordinate z
     * @param at0 time of sending signal
     * @param atR time of receiving signal
     */
    Station( double ax, double ay, double az, long double at0, long double aR )
    : x( ax )
    , y( ay )
    , z( az )
    , r( aR )
    {
     //   setR( at0 );
        std::cout << "time of sending of a new signal: " << at0 << std::endl;
    }

    Station( std::vector< double > vec )
    {
        x = vec.at(0);
        y = vec.at(1);
        z = vec.at(2);
        r = vec.at(3);
    }

    /**
     * @brief Sets radius.
     * @param aT0 time of sending signal
     */
    void setR( long double aT0 ) 
    {
    //    double clight = 300000000; // [m/s]
        double clight = 299792458; // [m/s]
    	dt = t-aT0;
	    r = dt*clight;
    }

    /**
     * @brief Returns coordinate x.
     * @return coordinate x
     */
    double getX() const { return x; }

    /**
     * @brief Returns coordinate y.
     * @return coordinate y
     */
    double getY() const { return y; }

    /**
     * @brief Returns coordinate z.
     * @return coordinate z
     */
    double getZ() const { return z; }

    /**
     * @brief Returns radius.
     * @return radius r
     */
    double getR() const { return r; }
    void addToZ( int a )
    {
        z = z+a;
    }

    std::vector< double > stationToVector()
    {   
        std::vector< double > ret;
        ret.push_back( x );
        ret.push_back( y );
        ret.push_back( z );
        ret.push_back( r );
        return ret;
    }

  private:
    double x;  ///< Coordinate x.
    double y;  ///< Coordinate y.
    double z;  ///< Coordinate z.
    long double t;  ///< Time of receiving signal.
    double r;  ///< Distance of vehicle from the station at time t-dt.
    long double dt; ///< Difference between time of receiving signal by ground station and sending by the vehicle.
};

/**
 * @brief Stations class which stores stations.
 */
class Stations
{
  public:
    /**
     * @brief Constructor.
     */
    Stations(){}

    /**
     * @brief Adding a single station.
     * @param aStation a single station added to the storage
     */
    void addStation( Station aStation )
    {
//    	aStation.setR( mT );
        mStations.push_back( aStation );
 //       std::cout << "Stations::addStation(): mT=" << mT << ", R=" << mStations.back().getR() << std::endl;
    }

    void addStation( double ax, double ay, double az, long double at0, long double aR )
    {
        Station aStation( ax, ay, az, at0, aR );
        mStations.push_back( aStation );
     //   std::cout << "Stations::addStation( 5xdouble ), r = " << aR << std::endl;
    }

    /**
     * @brief Adding of station using its position and time of receiving signal.
     * @param ax coordinate x
     * @param ay coordinate y
     * @param az coordinate z
     * @param at time of receiving signal
     */
    void addStations( double ax, double ay, double az, long double at )
    {
	    Station aStation( ax, ay, az, at );
	    mStations.push_back( aStation );
    }

    /**
     * @brief Sets time of receiving signal.
     * @param aT time of receiving signal
     */
    void setTime( long double aT )
    {
	    mT = aT;
    }

    /**
     * @brief Getting number of ground stations.
     * @return Number of stations.
     */
    int size() const { return mStations.size(); }

    /**
     * @brief Getting stations of given index.
     * @param iStationNb index of station
     * @return Station with given index.
     */
    Station getStation( int iStationNb )
    {
        return mStations.at(iStationNb ); 
    }

    /**
     * @brief Printing information about all stations.
     */
    void const printStations()
    {
        std::vector< Station >::iterator iter;
	    std::cout << std::endl;
        for( iter = mStations.begin(); iter != mStations.end(); ++iter )
        {
		    std::cout << "***" << (*iter).getX() << ", " << (*iter).getY() << ", " << (*iter).getZ() << ", " << (*iter).getR() << std::endl;
        }
	    std::cout << std::endl;
    }

    void clear()
    {
        mStations.clear();
    }

//  private:
    std::vector< Station > mStations;  
    long double mT;
};

typedef std::array< double, 4 > Position; // xs,ys,zs,rs from Apollonius 

/**
 * @brief Class which stores list of positions.
 */
class PositionsList
{
  public:
    /**
     * @brief Default constructor.
     */
    PositionsList(){}

    /**
     * @brief Constructor.
     * @param aPosition first position which will be on the list
     */
    PositionsList( Position aPosition ) { mPositions.push_back( aPosition ); }

    /**
     * @brief Adding position to the list.
     * @param aPosition position which is added
     */
    void addPosition( Position aPosition ) { mPositions.push_back( aPosition ); }

    /**
     * @brief Adding position using its coordinates.
     * @param aPosVec vector of coordinates.
     */
    void addPosition( std::vector< double > aPosVec ) 
    {
    	Position aPosition = { aPosVec.at(0), aPosVec.at(1), aPosVec.at(2), aPosVec.at(3) };
        mPositions.push_back( aPosition ); 
    }

    /**
     * @brief Adding vector of positions.
     * @param aPosVec vector of positions
     */
    void addPositions( std::vector< Position > aPositions )
    {
        std::vector< Position >::iterator iter;
        for( iter = aPositions.begin(); iter != aPositions.end(); ++iter )
        {
            mPositions.push_back( *iter );
        }
    }

    /**
     * @brief Getting position with given index.
     * @param aIndex index of position which is returned
     */
    Position getPosition( int aIndex ) const { return mPositions.at(aIndex); }

    /**
     * @brief Adding on the end of the PositionsList another PositionsList.
     * @param aList PositionList which is added 
     */
    void addPositions( PositionsList aList )
    {
        for( int i = 0; i<aList.size(); ++ i )
        {
    	    mPositions.push_back( aList.getPosition(i) );
	    }
    }

    /**
     * @brief Getting number of stored positions.
     */
    int size() const { return mPositions.size(); }

    /**
     * @brief Getting coordinate x of given position.
     * @param aIndex index of position which coordinate is returned
     */
    double getX( int aIndex ) { return mPositions.at(aIndex).at(0); }

    /**
     * @brief Getting coordinate y of given position.
     * @param aIndex index of position which coordinate is returned
     */
    double getY( int aIndex ) { return mPositions.at(aIndex).at(1); }

    /**
     * @brief Getting coordinate z of given position.
     * @param aIndex index of position which coordinate is returned
     */
    double getZ( int aIndex ) { return mPositions.at(aIndex).at(2); }

    /**
     * @brief Getting distance between vehicle and ground station.
     * @param aIndex index of position
     */
    double getR( int aIndex ) { return mPositions.at(aIndex).at(3); }

    /**
     * @brief Printing data of all positions on the list.
     */
    void printPositions()
    {
    	std::cout << std::endl << "CALCULATED POSITIONS: " << std::endl;
        std::vector< Position >::iterator iter;
    	for( iter = mPositions.begin(); iter != mPositions.end(); ++iter )
	    {
	        std::cout << "calculated: " << (*iter).at(0) << ", " << (*iter).at(1) << ", " << (*iter).at(2) << std::endl;
    	}
    }
    
    /**
     * @brief Printing average of all stored positions.
     */
    void printAveragePosition()
    {
        double x = 0, y = 0, z = 0;
    	int positionsNb = mPositions.size();
    	std::vector< Position >::iterator iter;
        for( iter = mPositions.begin(); iter != mPositions.end(); ++iter )
        {
	        x += (*iter).at(0)/positionsNb;
	        y += (*iter).at(1)/positionsNb;
    	    z += (*iter).at(2)/positionsNb;
        }
	    std::cout << "Average position: " << x << ", " << y << ", " << z << std::endl;
    }

  private:
    std::vector< Position > mPositions; ///< Container of positions.
};


class Signal
{
public:
    Signal(){} // pozbyc sie go
    Signal( int satId, long double timestamp ) // ma zostac 
    : mSatId( satId )
    , mTimestamp( timestamp )
    {
    
    }
    void addGroundStation( double ax, double ay, double az, double ar )
    {
        mGroundStations.push_back( std::make_tuple( ax, ay, az, ar ) );
    }

    void addGroundStation( double ax, double ay, double az, long double at0, double adt )
    {
    //    std::cout << "Signal::addGroundStation(): mTimestamp = " << mTimestamp << std::endl;
        double clight = 299792458; // [m/s]
        double r = clight*adt;
        mGroundStations.push_back( std::make_tuple( ax, ay, az, r ) );
    //    std::cout << "Signal::addGroundStation(): r = " << r << std::endl;
    }

    void convertSignalToStation( Stations& aStations )
    {
        std::vector< std::tuple< double, double, double, double > >::iterator it;
        for( it = mGroundStations.begin(); it != mGroundStations.end(); ++it )
        {
      //      std::cout << "Signal::convertSignalToStation(): " << std::get<3>(*it) << std::endl;
            Station lStation( std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), mTimestamp, std::get<3>(*it) );
            aStations.addStation( lStation );
        }
    }

    bool positionKnown ( double ax, double ay, double az )
    {
        bool isKnown = false;
        std::vector< std::tuple< double, double, double, double > >::iterator it;
        for( it = mGroundStations.begin(); it != mGroundStations.end(); ++it )
        {
            if( std::get<0>(*it) == ax &&
                    std::get<1>(*it) == ay &&
                    std::get<2>(*it) == az )
                isKnown = true;
        }
        return isKnown;
    }

    void printSignal()
    {
        std::cout << "Satellite Id: " << mSatId << ", time of sending singal: " << mTimestamp << std::endl;
        std::vector< std::tuple< double, double, double, double > >::iterator iter;
        int lCounter = 0;
        for( iter = mGroundStations.begin(); iter < mGroundStations.end(); ++iter )
        {
            std::cout << ++lCounter << ". Position (" <<  std::get<0>(*iter) << ", " << std::get<1>(*iter) << ", " ;
            std::cout << std::get<2>(*iter) << "). Distance: " << std::get<3>(*iter) << std::endl;
        }
    }
    void setSatId( int aId ){ mSatId = aId; }
    void setTimestamp( long double aTimestamp ){ mTimestamp = aTimestamp; }
    int getSatId() const { return mSatId; }
    long double getTimestamp() const { return mTimestamp; }
    int getSize() const { return mGroundStations.size(); }

private: 
    int mSatId; 
    long double mTimestamp; 
    std::vector< std::tuple< double, double, double, double > > mGroundStations;
};


#endif
