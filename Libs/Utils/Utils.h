/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Date:      $Date: 2014/03/24 01:17:40 $
  Version:   $Revision: 1.2 $
  Author:    $Author: elhabian $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#ifndef UTILS_H
#define UTILS_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>      // std::istringstream

#include <itkMath.h>
#include <cmath>
#include <algorithm>    // std::sort

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <itkImage.h>
#include <itkPoint.h>

namespace utils //TODO: -> namespace shapeworks (need to change everywhere it's used
{
#define twopi_inv 0.5/M_PI
#define twopi  2.0*M_PI

#define RANDU ((double) rand()/RAND_MAX)
#define RANDN2(mu, sigma) (mu + (rand()%2 ? -1.0 : 1.0)*sigma*pow(-log(0.99999*RANDU), 0.5))
#define RANDN RANDN2(0, 1.0)

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
}

class Utils{

public:
    static std::vector<int> randperm(int n);

    //--------------------------- IO-----------------------------------
    static void readSparseShape(vtkSmartPointer<vtkPoints>& points, char* filename, int number_of_particles = -1);
    static void writeSparseShape(char* filename, vtkSmartPointer<vtkPoints> particles);

    static void readSparseShape(std::vector<itk::Point<double> > & points, char* filename, int number_of_particles = -1);
    static void writeSparseShape(char* filename, std::vector<itk::Point<double, 3> > points);

    static std::vector<int> readParticleIds(char* filename);
    static void writeParticleIds(char* filename, std::vector<int> ids);

    //--------------- point cloud queries --------------------------------
    static void computeCenterOfMassForShapeEnsemble (std::vector< std::vector< itk::Point< double, 3 > > > points_list, itk::Point< double, 3 > & center);
    static void computeCenterOfMassForShape (std::vector< itk::Point< double, 3 > >  points, itk::Point< double, 3 > & center);

    static void updateMin(double curVal, double& minVal);
    static void updateMax(double curVal, double& maxVal);

    static void getBoundingBoxForShapeEnsemble (std::vector< std::vector< itk::Point< double, 3 > > > points_list,
                                                double& min_x, double& min_y, double& min_z,
                                                double& max_x, double& max_y, double& max_z);
    static void getBoundingBoxForShape (std::vector< itk::Point< double, 3 > >  points,
                                        double& min_x, double& min_y, double& min_z,
                                        double& max_x, double& max_y, double& max_z);


    //------------------------- string manipulation-------------------------------
    static std::string removeExtension( std::string const& filename );
    static std::string getPath( std::string const& filename );
    static std::string getFilename( std::string const& pathname );

    //--------------- coordinates transformations --------------------------------
    static void spherical2cartesian(const double inPoint[3], double outPoint[3]);
    static void cartesian2spherical(const double inPoint[3], double outPoint[3]);

    static vtkSmartPointer<vtkPoints> convertToPhysicalCoordinates(vtkSmartPointer<vtkPoints> particles, int number_of_particles,
                                                                   const itk::Image< float,  3 >::SpacingType& spacing ,
                                                                   const itk::Image< float,  3 >::PointType& origin);

    static vtkSmartPointer<vtkPoints> convertToImageCoordinates(vtkSmartPointer<vtkPoints> particles, int number_of_particles,
                                                                const itk::Image< float,  3 >::SpacingType& spacing ,
                                                                const itk::Image< float,  3 >::PointType& origin);

    static std::string num2str(float num);
    static std::string num2str(int num);
    static std::vector<double> linspace(double a, double b, size_t N);
    static std::string int2str(int n, int number_of_zeros);

    //--------------- average normal directions --------------------------------
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
    /* Trying every theta (with a step size of dtheta), find the theta that results in the smallest MSE.
         */
    static double averageThetaBruteForce(std::vector<double> thetas, double dtheta);

    // the chord method
    static double averageThetaChord(std::vector<double> thetas);

    // the exact method
    static double averageThetaArc(std::vector<double> thetas);

private:
    // only good for positive numbers.
    static double mod2pi_pos(double vin);

    // Ensure that v is [-PI, PI]
    static double mod2pi(double vin);

    /* Returns a value of v wrapped such that ref and v differ by no
         * more +/-PI
         */
    static double mod2pi(double ref, double v);

    /* For a given theta, compute the MSE. A simple O(N) method used for testing. */
    static double computeMSE(std::vector<double> thetas, double theta);

};

#endif // UTILS_H
