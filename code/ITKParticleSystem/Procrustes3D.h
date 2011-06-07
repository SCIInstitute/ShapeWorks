/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: Procrustes3D.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// -*- Mode: C++ -*-
#ifndef __Procrustes3D_h
#define __Procrustes3D_h

#include <vector>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

struct SimilarityTransform3D
{
  vnl_matrix_fixed<double, 3, 3> rotation;
  double scale;
  vnl_vector_fixed<double, 3> translation;
};

class Procrustes3D
{
public:
  typedef double RealType;
  typedef vnl_vector_fixed<double, 3> PointType;
  typedef std::vector<PointType> ShapeType;
  typedef ShapeType::iterator ShapeIteratorType;

  typedef std::vector<ShapeType> ShapeListType;
  typedef ShapeListType::iterator ShapeListIteratorType;
  typedef std::vector<SimilarityTransform3D> SimilarityTransformListType;
  typedef SimilarityTransformListType::iterator
    SimilarityTransformListIteratorType;

  Procrustes3D() {}

  // Align a list of shapes using Generalized Procrustes Analysis
  void AlignShapes(SimilarityTransformListType & transforms,
                   ShapeListType & shapes);

  // Helper function to transform a shape by a similarity transform
  static void TransformShape(ShapeType & shape,
                             SimilarityTransform3D & transform);
  static void TransformShapeDebug(ShapeType & shape,
                                  SimilarityTransform3D & transform);

  // Helper function to transform a list of shapes by a list of transforms
  static void TransformShapes(ShapeListType & shapes,
                              SimilarityTransformListType & transforms);

  static RealType ComputeSumOfSquares(ShapeListType & shapes);

private:
  // Align two shapes (rotation & scale) using Ordinary Procrustes Analysis
  void AlignTwoShapes(SimilarityTransform3D & transform,
                      ShapeType & shape1, ShapeType & shape2);

  // Compute mean of all shapes except the one at leaveOutIt
  void LeaveOneOutMean(ShapeType & mean, ShapeListType & shapeList,
                       ShapeListIteratorType & leaveOutIt);

  //  const RealType SOS_EPSILON = 1.0e-8;
};

#endif
