#ifndef READER_H
#define READER_H

#include "Def.h"
#include "Combinations.h"

class Reader
{
public:
    void loadGSData( const char* lFileName );
    void loadFromDirectory( char* lDirName );
    void loadStations( char* lFileName, Stations& mStations );
};

#endif //READER_H
