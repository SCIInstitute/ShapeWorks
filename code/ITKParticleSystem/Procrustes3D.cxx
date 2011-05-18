/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: Procrustes3D.cxx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.5 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include <iostream>
#include "Procrustes3D.h"
#include <vnl/algo/vnl_svd.h>

void
Procrustes3D::
AlignShapes(SimilarityTransformListType & transforms, ShapeListType & shapes)
{
  const RealType SOS_EPSILON = 1.0e-8;
  
  PointType center;
  ShapeListIteratorType shapeListIt;
  SimilarityTransformListIteratorType transformIt;
  ShapeIteratorType shapeIt;

  ShapeType mean;

  SimilarityTransform3D transform;
  transform.rotation.set_identity();
  transform.scale = 1.0;

  transforms.clear();
  transforms.reserve(shapes.size());

  // Remove translation
  for(shapeListIt = shapes.begin(); shapeListIt != shapes.end(); shapeListIt++)
  {
    ShapeType & shape = (*shapeListIt);
    center.fill(0.0);

    for(shapeIt = shape.begin(); shapeIt != shape.end(); shapeIt++)
      center += (*shapeIt);

    center /= static_cast<RealType>(shape.size());

    transform.translation = -center;

    // First time through transforms we need to push_back
    transforms.push_back(transform);

    // Apply translation to shape
    for(shapeIt = shape.begin(); shapeIt != shape.end(); shapeIt++)
      (*shapeIt) += center;
  }

  // Remove rotation and scale iteratively
  RealType sumOfSquares = ComputeSumOfSquares(shapes);
  RealType newSumOfSquares, diff = 1e10;

  while(diff > SOS_EPSILON)
  {
    shapeListIt = shapes.begin();
    transformIt = transforms.begin();
    while(shapeListIt != shapes.end())
    {
      LeaveOneOutMean(mean, shapes, shapeListIt);

      AlignTwoShapes((*transformIt), mean, (*shapeListIt));

      shapeListIt++;
      transformIt++;
    }

    // Fix scalings so geometric average = 1
    RealType scaleAve = 0.0;
    for(transformIt = transforms.begin(); transformIt != transforms.end();
        transformIt++)
      scaleAve += log((*transformIt).scale);

    scaleAve = exp(scaleAve / static_cast<RealType>(transforms.size()));

    SimilarityTransform3D scaleSim;
    scaleSim.rotation.set_identity();
    scaleSim.translation.fill(0.0);
    scaleSim.scale = 1.0 / scaleAve;

    shapeListIt = shapes.begin();
    transformIt = transforms.begin();
    while(shapeListIt != shapes.end())
    {
      TransformShape((*shapeListIt), scaleSim);
      (*transformIt).scale /= scaleAve;

      shapeListIt++;
      transformIt++;
    }

    newSumOfSquares = ComputeSumOfSquares(shapes);
    diff = sumOfSquares - newSumOfSquares;

    sumOfSquares = newSumOfSquares;
  }
}

void
Procrustes3D::
TransformShape(ShapeType & shape, SimilarityTransform3D & transform)
{
  ShapeIteratorType shapeIt;

  for(shapeIt = shape.begin(); shapeIt != shape.end(); shapeIt++)
  {
    PointType & point = (*shapeIt);

    point += transform.translation;
    point = transform.rotation * point;
    point = point * transform.scale;
  }
}

// void
// Procrustes3D::
// TransformShapeDebug(ShapeType & shape, SimilarityTransform3D & transform)
// {
//   ShapeIteratorType shapeIt;
  
//   for(shapeIt = shape.begin(); shapeIt != shape.end(); shapeIt++)
//     {
//     PointType & point = (*shapeIt);
//     vnl_vector_fixed<double,3> p;
//     p[0] = point[0]; p[1] = point[1]; p[2] = point[2];
    
//     vnl_matrix_fixed<double,4,4> R;
//     R(0,0) =  transform.rotation(0,0) * transform.scale;
//     R(1,0) =  transform.rotation(1,0) * transform.scale;
//     R(2,0) =  transform.rotation(2,0) * transform.scale;
//     R(3,0) =  0.0;
    
//     R(0,1) =  transform.rotation(0,1) * transform.scale;
//     R(1,1) =  transform.rotation(1,1) * transform.scale;
//     R(2,1) =  transform.rotation(2,1) * transform.scale;
//     R(3,1) =  0.0;
    
//     R(0,2) =  transform.rotation(0,2) * transform.scale;
//     R(1,2) =  transform.rotation(1,2) * transform.scale;
//     R(2,2) =  transform.rotation(2,2) * transform.scale;
//     R(3,2) =  0.0;
    
//     R(0,3) =  transform.translation(0) * R(0,0) + transform.translation(1) * R(0,1) + transform.translation(2) * R(0,2);
//     R(1,3) =  transform.translation(0) * R(1,0) + transform.translation(1) * R(1,1) + transform.translation(2) * R(1,2);
//     R(2,3) =  transform.translation(0) * R(2,0) + transform.translation(1) * R(2,1) + transform.translation(2) * R(2,2);
//     R(3,3) =  1.0;

//     //    point[0] = R[0][0] * p[0] + R[0][1] * p[1] + R[0][2] * p[2];
//     //  point[1] = R[1][0] * p[0] + R[1][1] * p[1] + R[1][2] * p[2];
//     //  point[2] = R[2][0] * p[0] + R[2][1] * p[1] + R[2][2] * p[2];
    
//     // std::cout << "POINT " << point << std::endl;
//     // std::cout << "transform.translation = " << transform.translation << std::endl << std::endl;

//     point += transform.translation;
//     point = transform.rotation * point;
//     point = point * transform.scale;
//     }
// }


void
Procrustes3D::
TransformShapes(ShapeListType & shapes,
                SimilarityTransformListType & transforms)
{
  ShapeListIteratorType shapeListIt;
  SimilarityTransformListIteratorType transformListIt;

  shapeListIt = shapes.begin();
  transformListIt = transforms.begin();
  while(shapeListIt != shapes.end() && transformListIt != transforms.end())
  {
    // TransformShape((*shapeListIt), (*transformListIt));
  TransformShape((*shapeListIt), (*transformListIt));

    shapeListIt++;
    transformListIt++;
  }
}

Procrustes3D::RealType
Procrustes3D::
ComputeSumOfSquares(ShapeListType & shapes)
{
  ShapeListIteratorType shapeIt1, shapeIt2;
  ShapeIteratorType pointIt1, pointIt2;
  
  RealType sum = 0.0;

  for(shapeIt1 = shapes.begin(); shapeIt1 != shapes.end(); shapeIt1++)
    {
    for(shapeIt2 = shapes.begin(); shapeIt2 != shapes.end(); shapeIt2++)
      {
      ShapeType & shape1 = (*shapeIt1);
      ShapeType & shape2 = (*shapeIt2);
      
      pointIt1 = shape1.begin();
      pointIt2 = shape2.begin();
      while(pointIt1 != shape1.end() && pointIt2 != shape2.end())
        {
        sum += ((*pointIt1) - (*pointIt2)).squared_magnitude();
        pointIt1++;
        pointIt2++;
        }
      }
    }
  return sum / static_cast<RealType>(shapes.size() * shapes[0].size());
}

void
Procrustes3D::
AlignTwoShapes(SimilarityTransform3D & transform, ShapeType & shape1,
               ShapeType & shape2)
{
  ShapeIteratorType shapeIt1, shapeIt2;
  vnl_matrix_fixed<RealType, 3, 3> shapeMat;
  RealType scale2;

  SimilarityTransform3D newTransform;
  newTransform.scale = 1.0;
  newTransform.translation.fill(0.0);

  // Build matrix X1 * X2^T, and get scale2 = tr(X2 * X2^T)
  shapeIt1 = shape1.begin();
  shapeIt2 = shape2.begin();
  shapeMat.fill(0.0);
  scale2 = 0.0;
  while(shapeIt1 != shape1.end() && shapeIt2 != shape2.end())
  {
    PointType & point1 = (*shapeIt1);
    PointType & point2 = (*shapeIt2);

    shapeMat(0, 0) += point1[0] * point2[0];
    shapeMat(0, 1) += point1[0] * point2[1];
    shapeMat(0, 2) += point1[0] * point2[2];
    shapeMat(1, 0) += point1[1] * point2[0];
    shapeMat(1, 1) += point1[1] * point2[1];
    shapeMat(1, 2) += point1[1] * point2[2];
    shapeMat(2, 0) += point1[2] * point2[0];
    shapeMat(2, 1) += point1[2] * point2[1];
    shapeMat(2, 2) += point1[2] * point2[2];

    scale2 += point2.squared_magnitude();

    shapeIt1++;
    shapeIt2++;
  }

  // Rotation from SVD
  vnl_svd<RealType> svd(shapeMat.transpose());
  newTransform.rotation = svd.V() * svd.U().transpose();
  transform.rotation = newTransform.rotation * transform.rotation;

  TransformShape(shape2, newTransform);

  // Compute scale

  // Need the value tr(X1 * X2^T) (after rotation of X2)
  shapeIt1 = shape1.begin();
  shapeIt2 = shape2.begin();
  RealType scale1 = 0.0;
  while(shapeIt1 != shape1.end() && shapeIt2 != shape2.end())
  {
    scale1 += dot_product((*shapeIt1), (*shapeIt2));

    shapeIt1++;
    shapeIt2++;
  }

  newTransform.scale = (scale1 / scale2);
  transform.scale *= newTransform.scale;

  newTransform.rotation.set_identity();
  TransformShape(shape2, newTransform);
}

void
Procrustes3D::
LeaveOneOutMean(ShapeType & mean, ShapeListType & shapeList,
                ShapeListIteratorType & leaveOutIt)
{
  ShapeListIteratorType shapeListIt;
  ShapeIteratorType shapeIt, meanIt;

  int i, numPoints = shapeList[0].size();

  mean.clear();
  mean.reserve(numPoints);
  for(i = 0; i < numPoints; i++)
    mean.push_back(vnl_vector_fixed<RealType, 3>(0.0, 0.0, 0.0));

  for(shapeListIt = shapeList.begin(); shapeListIt != shapeList.end();
      shapeListIt++)
  {
    if(shapeListIt != leaveOutIt)
    {
      ShapeType & shape = (*shapeListIt);
      shapeIt = shape.begin();
      meanIt = mean.begin();
      while(shapeIt != shape.end())
      {
        (*meanIt) += (*shapeIt);

        shapeIt++;
        meanIt++;
      }
    }
  }

  for(meanIt = mean.begin(); meanIt != mean.end(); meanIt++)
    (*meanIt) /= static_cast<RealType>(shapeList.size() - 1);
}
