#include "GaussianMatrix.cpp"
#include "Apollonius.h"
#include "Def.h"

int main()
{
    std::vector< double > r1, r2, r3, r4, r5, r6, r7;
    r1.push_back( 6344814.403772608 );
    r1.push_back( 399181.88699951884 );
    r1.push_back( -416683.3363252414 );
    r1.push_back( 0.0017773658089111194 );

    r2.push_back( 6357359.199953143 );
    r2.push_back( 3.892759797626297E-10 );
    r2.push_back( -416683.3363252414 );
    r2.push_back( 0.0016418828479264841 );

    r3.push_back( 6358428.286816518 );
    r3.push_back( 400038.39992125647 );
    r3.push_back( 0.0 );
    r3.push_back( 0.0010797998094915886 );

    r4.push_back( 6371000.0 );
    r4.push_back( 3.901112378683894E-10 );
    r4.push_back( 0.0 );
    r4.push_back( 8.37689509514476E-4 );

    r5.push_back( 6358428.286816518 );
    r5.push_back( -400038.3999212557 );
    r5.push_back( 0.0 );
    r5.push_back( 0.001978136977685107 );

    r6.push_back( 6344814.403772608 );
    r6.push_back( 399181.88699951884 );
    r6.push_back( 416683.3363252414 );
    r6.push_back( 0.0017773658089111194 );

    r7.push_back( 6357359.199953143 );
    r7.push_back( 3.892759797626297E-10 );
    r7.push_back( 416683.3363252414 );
    r7.push_back( 0.0016418828479264841 );

    std::vector< std::vector< double > > matrix;
    matrix.push_back( r1 );
    matrix.push_back( r2 );
    matrix.push_back( r3 );
    matrix.push_back( r4 );
    matrix.push_back( r5 );
    matrix.push_back( r6 );
    matrix.push_back( r7 );


    std::vector< Station > stations;
    stations.push_back( Station(r1) );
    stations.push_back( Station(r2) );
    stations.push_back( Station(r3) );
    stations.push_back( Station(r4) );
    stations.push_back( Station(r5) );
    stations.push_back( Station(r6) );
    stations.push_back( Station(r7) );

    solveApol( 1, 0, stations );
/*    GaussianMatrix gMatrix( matrix );
    gMatrix.printData();
*/
    return 0;
}
