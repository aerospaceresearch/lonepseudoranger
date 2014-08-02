#include <iostream>
#include <vector>
#include <stdlib.h>

#include "Matrix.h"

   
Matrix::Matrix( std::vector< std::vector< double > > aData )
: mData( aData )
{
}

void Matrix::addRow( std::vector< double > aRow ) 
{
    mData.push_back( aRow );
}

std::vector< double > Matrix::getRow( int index )
{
    return mData.at(index);
}

void Matrix::setData( std::vector< std::vector< double > >  aData )
{
    std::cout << "setting data! " << std::endl;
    mData = aData;
    printData(); 
    if( mData.size() > 4 )
    {
        std::cout << "overdetermination!" << std::endl;
        overdetermined();
        printData();
        std::cout << "after overdetermination" << std::endl;
    }
    printData(); 
}
 
int Matrix::getRowsNb()
{
    return mData.size();
}

int Matrix::getColsNb()
{
    return (mData.at(0)).size();
}

void Matrix::overdetermined()
{
    std::cout << "Matrix::overdetermined(): " << std::endl;
    int newN = 4;
    int N = getRowsNb();
    printData();
    double A[N][4];
    double b[N];

    for( int i=0; i<N; ++i )
    {
        for( int j=0; j<4; ++j )
        {
            A[i][j] = mData[i][j];
        }
        b[i] = mData[i][4];
    }
 
    mData.clear();
    mData.resize( newN );
    for( int i=0; i<N; ++i )
    {
        mData[i].resize( 5 );
    }

    double ATA[newN][4];
    double ATb[newN];
    for( int i=0; i<newN; ++i )
    {
        ATb[i] = 0;
        for( int j=0; j<4; ++j )
            ATA[i][j] = 0;
    }

    for( int i=0; i<newN; ++i )
    {
        for( int j=0; j<4; ++j )
        {
            for( int k=0; k<N; ++k )
            {
                ATA[i][j] = ATA[i][j] + A[k][i]*A[k][j];
            }
        }
    }

    for( int i=0; i<newN; ++i )
    {
        for( int k=0; k<N; ++k )
        {
            ATb[i] = ATb[i] + A[k][i]*b[k];
        }
    }

    for( int i=0; i<newN; ++i )
    {
        for( int j=0; j<4; ++j )
        {
            mData[i][j] = ATA[i][j];
        }
        mData[i][4] = ATb[i];
    }

    printData();
    std::cout << "end of overdetermination!" << std::endl;
}

void Matrix::printData()
{
    std::cout << std::endl;
    for( int i=0; i<getRowsNb(); ++i )
    {
	for( int j=0; j<getColsNb(); ++j )
	{
      std::cout.precision(15);
	  std::cout << (mData.at(i)).at(j) << " " ;
	}
	std::cout << std::endl;
    }
}

