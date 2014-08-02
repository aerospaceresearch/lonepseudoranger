#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
#include <stdlib.h>

//#include "Def.h"
//
typedef std::vector< double > Row;

class Matrix
{
  public:
    Matrix(){}
    Matrix( std::vector< std::vector< double > >  aData );
    void setData( std::vector< std::vector< double > >  aData );
    void overdetermined();
    void printData();
    double operator() ( int r, int c ){ return (mData.at(r-1)).at(c-1); } 
    int getRowsNb();
    int getColsNb();
    std::vector< double > getRow( int index );
    void addRow( std::vector< double > );
    double get( int row, int col ){ return (mData.at(row)).at(col); }
    std::vector< std::vector< double > > const getData(){ return mData; }

  private:
    std::vector< Row > mData;
};

#endif
