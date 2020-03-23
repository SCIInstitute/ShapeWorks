#include <gtest/gtest.h>

#include <cstdio>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <cstring>
#include <fstream>

#include <Eigen/Core>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkReinitializeLevelSetImageFilter.h> // for distance transform computation

#include "TestConfiguration.h"

#include "ShapeWorksRunApp.h"
#include "itkParticleShapeStatistics.h"

#include "RBFShape.h"

double *ellipsoid_grid ( int n, double r[3], double c[3], int ng );

//---------------------------------------------------------------------------
TEST(PointCloudTests, sample_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/point_cloud_ellipsoid");
  chdir(test_location.c_str());

  int n = 10;
  double r[3] = {1.,2.,3.};
  double c[3] = {0.,0.,0.};
  int ng = n*r[0]*2*n*r[1]*2*n*r[2]*2 ;
  std::cout << "ng: " << ng << std::endl;
  double* ellipsoid_pts = ellipsoid_grid(n, r, c, ng);

  int count = 0;

  for(size_t i = 0; i < ng; i++){
    double ix = ellipsoid_pts[i*3];
    double iy = ellipsoid_pts[i*3+1];
    double iz = ellipsoid_pts[i*3+2];
    //std::cout << "Point " << ix << " " << iy << " " << iz << std::endl;
    if(ix != 0. && iy != 0 && iz != 0){
        count++;
    }
  }

  std::cout << "Count: " << count << std::endl;

  Eigen::MatrixXd ellipsoid(count, 3);

  for(size_t i = 0; i < count; i++){
    double ix = ellipsoid_pts[i*3];
    double iy = ellipsoid_pts[i*3+1];
    double iz = ellipsoid_pts[i*3+2];

    ellipsoid(i, 0) = ix;
    ellipsoid(i, 1) = iy;
    ellipsoid(i, 2) = iz;
  }

  shapeworks::RBFShape RBFEllipsoid = shapeworks::RBFShape();

  RBFEllipsoid.solve_system(ellipsoid);

  double value = 99;
  ASSERT_LT(value, 100);
}

//****************************************************************************80

int i4_ceiling ( double x )

//****************************************************************************80
//
//  Purpose:
//
//    I4_CEILING rounds an R8 up to the next I4.
//
//  Example:
//
//    X        I4_CEILING(X)
//
//   -1.1      -1
//   -1.0      -1
//   -0.9       0
//   -0.1       0
//    0.0       0
//    0.1       1
//    0.9       1
//    1.0       1
//    1.1       2
//    2.9       3
//    3.0       3
//    3.14159   4
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    10 November 2011
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the number whose ceiling is desired.
//
//    Output, int I4_CEILING, the ceiling of X.
//
{
  int value;

  value = ( int ) x;

  if ( value < x )
  {
    value = value + 1;
  }

  return value;
}

//****************************************************************************80

double r8vec_min ( int n, double r8vec[] )

//****************************************************************************80
//
//  Purpose:
//
//    R8VEC_MIN returns the value of the minimum element in an R8VEC.
//
//  Discussion:
//
//    An R8VEC is a vector of R8's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 July 2005
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the array.
//
//    Input, double R8VEC[N], the array to be checked.
//
//    Output, double R8VEC_MIN, the value of the minimum element.
//
{
  int i;
  double value;

  value = r8vec[0];

  for ( i = 1; i < n; i++ )
  {
    if ( r8vec[i] < value )
    {
      value = r8vec[i];
    }
  }
  return value;
}

//****************************************************************************80

double *ellipsoid_grid ( int n, double r[3], double c[3], int ng )

//****************************************************************************80
//
//  Purpose:
//
//    ELLIPSOID_GRID generates the grid points inside an ellipsoid.
//
//  Discussion:
//
//    The ellipsoid is specified as
//
//      ( ( X - C1 ) / R1 )^2
//    + ( ( Y - C2 ) / R2 )^2
//    + ( ( Z - C3 ) / R3 )^2 = 1
//
//    The user supplies a number N.  There will be N+1 grid points along
//    the shortest axis.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    12 November 2011
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of subintervals.
//
//    Input, double R[3], the half axis lengths.
//
//    Input, double C[3], the center of the ellipsoid.
//
//    Input, int NG, the number of grid points.
//
//    Output, double XYZ[3*NG], the grid point coordinates.
//
{
  double h;
  int ii;
  int i;
  int j;
  int k;
  int m;
  int ng2;
  int ni;
  int nj;
  int nk;
  int np;
  double p[3*8];
  double rmin;
  double x;
  double *xyz;
  double y;
  double z;

  ng2 = 0;

  xyz = new double[3*ng];

  rmin = r8vec_min ( 3, r );

  if ( r[0] == rmin )
  {
    h = 2.0 * r[0] / ( double ) ( 2 * n + 1 );
    ni = n;
    nj = i4_ceiling ( r[1] / r[0] ) * ( double ) ( n );
    nk = i4_ceiling ( r[2] / r[0] ) * ( double ) ( n );
  }
  else if ( r[1] == rmin )
  {
    h = 2.0 * r[1] / ( double ) ( 2 * n + 1 );
    nj = n;
    ni = i4_ceiling ( r[0] / r[1] ) * ( double ) ( n );
    nk = i4_ceiling ( r[2] / r[1] ) * ( double ) ( n );
  }
  else
  {
    h = 2.0 * r[2] / ( double ) ( 2 * n + 1 );
    nk = n;
    ni = i4_ceiling ( r[0] / r[2] ) * ( double ) ( n );
    nj = i4_ceiling ( r[1] / r[2] ) * ( double ) ( n );
  }

  for ( k = 0; k <= nk; k++ )
  {
    z = c[2] + ( double ) ( k ) * h;
    for ( j = 0; j <= nj; j++ )
    {
      y = c[1] + ( double ) ( j ) * h;
      for ( i = 0; i <= ni; i++ )
      {
        x = c[0] + ( double ) ( i ) * h;
//
//  If we have left the ellipsoid, the I loop is completed.
//
        if ( 1.0 < pow ( ( x - c[0] ) / r[0], 2 )
                 + pow ( ( y - c[1] ) / r[1], 2 )
                 + pow ( ( z - c[2] ) / r[2], 2 ) )
        {
          break;
        }
//
//  At least one point is generated, but more possible by symmetry.
//
        np = 0;
        p[0+np*3] = x;
        p[1+np*3] = y;
        p[2+np*3] = z;

        np = 1;

        if ( 0 < i )
        {
          for ( m = 0; m < np; m++ )
          {
            p[0+(np+m)*3] = 2.0 * c[0] - p[0+m*3];
            p[1+(np+m)*3] = p[1+m*3];
            p[2+(np+m)*3] = p[2+m*3];
          }
          np = 2 * np;
        }

        if ( 0 < j )
        {
          for ( m = 0; m < np; m++ )
          {
            p[0+(np+m)*3] = p[0+m*3];
            p[1+(np+m)*3] = 2.0 * c[1] - p[1+m*3];
            p[2+(np+m)*3] = p[2+m*3];
          }
          np = 2 * np;
        }

        if ( 0 < k )
        {
          for ( m = 0; m < np; m++ )
          {
            p[0+(np+m)*3] = p[0+m*3];
            p[1+(np+m)*3] = p[1+m*3];
            p[2+(np+m)*3] = 2.0 * c[2] - p[2+m*3];
          }
          np = 2 * np;
        }

        for ( m = 0; m < np; m++ )
        {
          for ( ii = 0; ii < 3; ii++ )
          {
            xyz[ii+(ng2+m)*3] = p[ii+m*3];
          }
        }
        ng2 = ng2 + np;
      }
    }
  }
  return xyz;
}


