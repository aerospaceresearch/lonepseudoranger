#include "GaussianMatrix.h"
#include <vector>

class GaussianMatrix;

int main()
{
    std::vector< double > r1;
    r1.push_back( 2 );
    r1.push_back( 1 );
    r1.push_back( 4 );
    r1.push_back( 5 );
    r1.push_back( 6 );
    std::vector< double > r2;
    r2.push_back( 3 );
    r2.push_back( 3 );
    r2.push_back( 7 );
    r2.push_back( 5 );
    r2.push_back( 6 );
    std::vector< double > r3;
    r3.push_back( 1 );
    r3.push_back( 1 );
    r3.push_back( 3 );
    r3.push_back( 2 );
    r3.push_back( 7 );

    std::vector< std::vector< double > > matrix;
    matrix.push_back( r1 );
    matrix.push_back( r2 );
    matrix.push_back( r3 );

    GaussianMatrix gMatrix( matrix );
    gMatrix.printData(); 
}
