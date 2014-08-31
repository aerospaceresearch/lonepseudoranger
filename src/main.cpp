#include <iostream>
#include <unistd.h>
#include <string>

#include "Reader.h"

void processSignalData( std::string, std::string, std::string );

int main( int argc, char* argv[] )
{
    Stations mStations;
    Reader reader;
    std::string statInput = "";//beacon222.sat";
    std::string statOutput = "output";//stats222.txt";
    std::string dir;
    int c;
    bool isDirectoryKnown = false;
    while( ( c = getopt( argc, argv, "d:i:o:" ) ) != -1 )
    {
        switch( c ){
        case 'i':
            statInput = std::string( optarg );
            std::cout << "Selected input: " << statInput << std::endl;
            break;
        case 'o':
            statOutput = std::string( optarg );
            std::cout << "Selected output: " << statOutput << std::endl;
            break;
        case 'd':
            isDirectoryKnown = true;
            reader.loadFromDirectory( optarg );
            dir = std::string( optarg );
            processSignalData( statInput, statOutput, dir.substr(dir.length()-12, 10 ) );
            break;
        default:
	        std::cout << "Missing path to the directory! Use parameter -d." << std::endl;
	        return -1;
        }
    }
    if( !isDirectoryKnown )
        std::cout << "Directory with .gsd files unknown!" << std::endl;
    return 0;
}
