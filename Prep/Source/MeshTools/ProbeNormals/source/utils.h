/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: CS6350.h,v $
  Date:      $Date: 2014/03/24 01:17:40 $
  Version:   $Revision: 1.2 $
  Author:    $Author: shireen $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __utils_h
#define __utils_h

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>      // std::istringstream

#include <itkMath.h>
#include <cmath>
#include <algorithm>    // std::sort

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>


namespace utils
{

#define twopi_inv 0.5/M_PI
#define twopi  2.0*M_PI

#define RANDU ((double) rand()/RAND_MAX)
#define RANDN2(mu, sigma) (mu + (rand()%2 ? -1.0 : 1.0)*sigma*pow(-log(0.99999*RANDU), 0.5))
#define RANDN RANDN2(0, 1.0)

std::string num2str(int num)
{
    std::string Result;          // string which will contain the result
    std::ostringstream convert;   // stream used for the conversion
    convert << num;      // insert the textual representation of 'Number' in the characters in the stream
    Result = convert.str(); // set 'Result' to the contents of the stream
    return Result;
}

std::string num2str(float num)
{
    std::string Result;          // string which will contain the result
    std::ostringstream convert;   // stream used for the conversion
    convert << num;      // insert the textual representation of 'Number' in the characters in the stream
    Result = convert.str(); // set 'Result' to the contents of the stream
    return Result;
}

// ------------------- string manipulation ------------------------------------
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};
#else
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
#endif


std::string removeExtension( std::string const& filename )
{
    std::string::const_reverse_iterator
            pivot
            = std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
            ? filename
            : std::string( filename.begin(), pivot.base() - 1 );
}

std::string getPath( std::string const& filename )
{
    std::string::const_reverse_iterator
            pivot
            = std::find( filename.rbegin(), filename.rend(), '/' );
    return pivot == filename.rend()
            ? filename
            : std::string( filename.begin(), pivot.base() - 1 );
}

std::string getFilename( std::string const& pathname )
{
    return std::string(
                std::find_if( pathname.rbegin(), pathname.rend(),
                              MatchPathSeparator() ).base(),
                pathname.end() );
}

// ------------------- coordinates transformations -----------------------------
void spherical2cartesian(const double inPoint[3], double outPoint[3])
{
    double r        = inPoint[0];
    double sinphi   = sin(inPoint[1]);
    double cosphi   = cos(inPoint[1]);
    double sintheta = sin(inPoint[2]);
    double costheta = cos(inPoint[2]);

    outPoint[0] = r*sinphi*costheta;
    outPoint[1] = r*sinphi*sintheta;
    outPoint[2] = r*cosphi;
}

void cartesian2spherical(const double inPoint[3], double outPoint[3])
{
    double x = inPoint[0];
    double y = inPoint[1];
    double z = inPoint[2];

    double RR = x*x + y*y;
    double r  = sqrt(RR + z*z);

    outPoint[0] = r;
    if (r == 0)
        outPoint[1] = 0;
    else
        outPoint[1] = acos(z/r);

    if (RR == 0)
        outPoint[2] = 0;
    else
    {
        // Change range to [0, 2*Pi], otherwise the same as atan2(y, x)
        outPoint[2] = double(itk::Math::pi) + atan2(-y, -x);
    }
}

/**
 * Given a set of theta measurements, pick the "average" (approximately).
 *
 * More formally, given a set of orientations, we wish to identify a
 * "reference theta" such that the sum of the squared differences
 * between each theta and the reference theta is minimized. This can
 * be visualized: each theta (including the reference theta) can be
 * mapped onto the unit circle): we wish to minimize the distance
 * between the reference point and every other points by traveling
 * along the circumference of the unit circle.
 *
 * APPROXIMATE CHORD SOLUTION
 * --------------
 * This is hard, however, so instead of computing the distance along the
 * circumference, we compute the distance along the chord.
 *
 * This method is by ebolson@umich.edu, inspired by a similar problem
 * in Horn's "closed-form solution of absolute orientation using unit
 * quaternions".
 *
 * Let a be the set of input points, and R(a_i) represent a rotation
 * of point a_i around the origin:
 *
 * R(x) = [ cos(theta)*a_x - sin(theta)*a_y,] [ sin(theta)*a_x + cos(theta)*a_y ]
 *
 * The error is:
 *
 * X^2 = SUM ( R(a_i) - [1 0]' )' * (R(a_i) - [1 0]')
 *
 * = SUM R'R - 2[1 0]R(a) + [1 0][1 0]'
 *
 * Note that R'R is constant, because R and R' are orthogonal. (R'R =
 * I). Dropping constant terms:
 *
 * X^2 = SUM 2[1 0]R(a)
 *
 * Differentiating with respect to theta:
 *
 * dX^2/dtheta = SUM cos(theta)a_x - sin(theta)a_y = 0
 *
 * Collecting cos and sin terms:
 *
 * cos(theta) SUM a_x = sin(theta) SUM a_y
 *
 * e.g.,:
 *
 * theta = atan2( SUM a_y , SUM a_x )
 *
 * EXACT SOLUTION
 * ----------------
 * This solution runs in O(n log n).
 *
 * Let us suppose that all of the input angles are mapped to [-PI,
 * PI].

 * All the input points can be shifted to be within PI degrees of the
 * reference angle by adding a multiple of 2PI degrees. If all the
 * input angles are constrained to [-PI, PI], then we can find a
 * reference angle [-PI, 2PI] such that all input points are within PI
 * degrees by either adding 0 or exactly 2PI to individual input points.
 *
 * More so, the input points that we must add 2PI to are the M points
 * with the smallest theta, but we do not know M. This is necessary
 * when the correct reference angle is large: the smallest points will
 * be more than PI degrees away, so they need to be moved to the right
 * side of the reference angle.
 *
 * If we knew M, computing the reference angle is easy: it is simply
 * the average of the (possibly shifted) input points. Let x[i] be the
 * input point [-PI,PI] and y[i] be the possibly shifted version of
 * that point, y[i] = x[i] + 2PI if i < M, otherwise y[i] = x[i].
 *
 * r = reference angle = (1 / N) * SUM_i y[i]
 * error = SUM_i (y[i] - r)^2
 *
 * We simply search over each value of M (from 0 to N), and recompute
 * the error. Both the reference angle and error can be written in
 * terms of the first and second moments of y[i], which gives us the
 * following strategy:
 *
 * 1) Compute A1 and A2, the first and second moments of y[i],
 * assuming M = 0. (This is just the first and second moments of
 * x[i]). This involves iterating over each of the input points.
 *
 * 2) Considering the points in x[i] in sorted order, update A1 and A2
 * such that they reflect y[i] = x[i] + 2PI. Compute the new reference
 * theta and error after every point (an O(1) operation) and report
 * the theta whose error was the smallest.
 *
 * Total run time is O(N log N) due to the sort operation. The other
 * two passes are O(N). Memory usage is O(N), since all points must be
 * stored so they can be sorted.
 *
 * SUMMARY
 * -------
 *
 * method         runtime          memory         notes
 * --------------------------------------------------------
 * brute          O(2PI*N / eps)   O(N)           worst-case error is eps/2
 * exact          O(N log N)       O(N)
 * chord          O(N)             O(1)           minimizes squared chord length, not squared arc length.
 *
 * Real-world performance: the exact method is typically faster than
 * the chord method, presumably because of the high cost of computing
 * trigonometric functions used in the Chord method. This advantage
 * decreases with larger number of points (due to the super-linear
 * cost of sorting), but even at 50000 points, the optimal method is
 * (a bit) faster than the chord method.
 *
 *
 * Reference: Olson, Edwin. "On computing the average orientation of vectors and lines."
 *            In Robotics and Automation (ICRA), 2011 IEEE International Conference on, pp. 3869-3874. IEEE, 2011.
 *
 * Code is written in C++ from author's java implmentation by Shireen Elhabian - SCI institute, University of Utah
 **/

// only good for positive numbers.
double mod2pi_pos(double vin)
{
    double q = vin * twopi_inv + 0.5;
    int qi = (int) q;

    return vin - qi*twopi;
}

// Ensure that v is [-PI, PI]
double mod2pi(double vin)
{
    double v;

    if (vin < 0)
        v = -1.0*mod2pi_pos(-1.0*vin);
    else
        v = mod2pi_pos(vin);

    // Validation test:
    //	if (v < -Math.PI || v > Math.PI)
    //		System.out.printf("%10.3f -> %10.3f\n", vin, v);

    return v;
}

/* Returns a value of v wrapped such that ref and v differ by no
     * more +/-PI
     */
double mod2pi(double ref, double v)
{
    return ref + mod2pi(v-ref);
}

/* For a given theta, compute the MSE. A simple O(N) method used for testing. */
double computeMSE(std::vector<double> thetas, double theta)
{
    double sqerr = 0;
    for(std::vector<double>::iterator it = thetas.begin(); it != thetas.end(); ++it)
    {
        double err = mod2pi((*it) - theta);
        sqerr += (err * err);
    }
    return (sqerr / ((double)thetas.size()));
}


/* Trying every theta (with a step size of dtheta), find the theta that results in the smallest MSE.
     */
double averageThetaBruteForce(std::vector<double> thetas, double dtheta)
{
    double bestmse = 1e10;
    double besttheta = 0;

    for (double theta = 0; theta < 2 * M_PI; theta += dtheta)
    {
        double mse = computeMSE(thetas, theta);

        if (mse < bestmse) {
            bestmse = mse;
            besttheta = theta;
        }
    }
    return mod2pi(besttheta);
}

// the chord method
double averageThetaChord(std::vector<double> thetas)
{
    double M = 0, N = 0;

    for(std::vector<double>::iterator it = thetas.begin(); it != thetas.end(); ++it)
    {
        double x0 = cos(*it);
        double y0 = sin(*it);
        M += y0;
        N += x0;

    }

    return atan2(M, N);
}

// the exact method
double averageThetaArc(std::vector<double> thetas)
{
    std::vector<double> x;

    for(std::vector<double>::iterator it = thetas.begin(); it != thetas.end(); ++it)
        x.push_back(mod2pi(*it)); // map to [-PI, PI]
    std::sort (x.begin(), x.end()); // ascending numerical order.

    // compute first and second moments without adding 2PI
    double A1 = 0, A2 = 0;

    int sz = (int) x.size();
    for (int i = 0; i < sz; i++)
    {
        A1 += x[i];
        A2 += x[i]*x[i];
    }

    // now, go back through again, converting elements one-by-one
    // into their +2PI versions, recomputing the error and picking
    // the best one.

    // initialize with case where all points are the non-2PI
    // version.
    double bestError = 1e10;
    double bestTheta = -1;

    // note: i=-1 iteration tests case where none are flipped.
    for (int i = -1; i < sz; i++)
    {
        if (i >= 0) {
            // flip the i'th component into the +2PI version
            A1 += 2*M_PI;
            // A2 += (x[i] + 2*Math.PI)^2 - x[i]*x[i]
            A2 += 4*M_PI*x[i] + 4*M_PI*M_PI;
        }

        double theta = A1 / ((double)x.size());
        double error = A2 - 2*A1*theta + ((double)x.size())*theta*theta;

        if (error < bestError) {
            bestError = error;
            bestTheta = theta;
        }
    }

    return mod2pi(bestTheta);
}


/* Prateep */
// f(x) = (1-x)^2 / (1+x)^2
double f(double ctheta) 
{
    const double epsilon = 1.0e-10;
    double num = (1.0f * (1-ctheta)) * (1.0f * (1-ctheta));
    double den = (1 + ctheta + epsilon) * (1 + ctheta + epsilon);
    return num / den;
}

// f'(x) = -(4 * (1-x)) / (1+x)^3
double fprime(double ctheta)
{
  const double epsilon = 1.0e-10;
  double num = (1.0f - ctheta);
  double den = (1 + ctheta + epsilon);
  return (-4.0f*num) / (den * den * den);
}


}  // end namespace utils

#endif
