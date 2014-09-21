#ifndef COMBINATIONS_H
#define COMBINATIONS_H

#include <iostream>
#include <stdint.h>
#include <vector>

/**
 * @brief Combinations class
 */
class Combinations
{
  public:
    uint64_t initialBitCombination( int n, int k );
    uint64_t nextBitCombination( uint64_t currentBitCombination );
    std::vector< std::vector< int > > getStationsCombinations( int n, int k );
};

#endif // COMBINATIONS_H

