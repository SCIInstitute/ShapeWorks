---
title: Libs/Utils/Utils.h

---

# Libs/Utils/Utils.h



## Namespaces

| Name           |
| -------------- |
| **[utils](../Namespaces/namespaceutils.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[Utils](../Classes/classUtils.md)**  |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[twopi_inv](../Files/Utils_8h.md#define-twopi-inv)**  |
|  | **[twopi](../Files/Utils_8h.md#define-twopi)**  |
|  | **[RANDU](../Files/Utils_8h.md#define-randu)**  |
|  | **[RANDN2](../Files/Utils_8h.md#define-randn2)**(mu, sigma)  |
|  | **[RANDN](../Files/Utils_8h.md#define-randn)**  |




## Macros Documentation

### define twopi_inv

```cpp
#define twopi_inv 0.5/M_PI
```


### define twopi

```cpp
#define twopi 2.0*M_PI
```


### define RANDU

```cpp
#define RANDU ((double) rand()/RAND_MAX)
```


### define RANDN2

```cpp
#define RANDN2(
    mu,
    sigma
)
(mu + (rand()%2 ? -1.0 : 1.0)*sigma*pow(-log(0.99999*RANDU), 0.5))
```


### define RANDN

```cpp
#define RANDN RANDN2(0, 1.0)
```


## Source code

```cpp
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

#ifdef _WIN32
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#endif

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
#include <vnl/vnl_matrix.h>

namespace utils //TODO: -> namespace shapeworks (need to change everywhere it's used
{
#define twopi_inv 0.5/M_PI
#define twopi  2.0*M_PI

#define RANDU ((double) rand()/RAND_MAX)
#define RANDN2(mu, sigma) (mu + (rand()%2 ? -1.0 : 1.0)*sigma*pow(-log(0.99999*RANDU), 0.5))
#define RANDN RANDN2(0, 1.0)

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

    //--------------- linear algebra -------------------------------------------

    // matrix multiplication without an allocation for the output
    template<typename T>
    static void multiply_into(vnl_matrix<T> &out, const vnl_matrix<T> &lhs, const vnl_matrix<T> &rhs);

  //--------------- average normal directions --------------------------------
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
```


-------------------------------

Updated on 2022-02-02 at 18:18:10 +0000
