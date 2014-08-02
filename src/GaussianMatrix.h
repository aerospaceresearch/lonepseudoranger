#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
#include <stdlib.h>

typedef std::vector< double > Row;

class GaussianMatrix
{
  public:
    GaussianMatrix(){}
    GaussianMatrix( std::vector< std::vector< double > >  aData );
//    void setData( Matrix aMatrix );
    void setData( std::vector< std::vector< double > >  aData );
    void overdetermined();
    void makeGaussian();
    void makeGaussian2();
    void printData();
    double operator() ( int r, int c ){ return (mData.at(r-1)).at(c-1); } 
    int getRowsNb();
    int getColsNb();
    std::vector< double > getRow( int index );
    void addRow( std::vector< double > );
    double get( int row, int col ){ return (mData.at(row)).at(col); }

  private:
    void swapRows( int first, int second );
    int findMaxRow( int col ); 
    void multiply( int col );
    void subtractRow( int row );
    void makeDiagonalOnes();
    std::vector< Row > mData;
};

#endif
