/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: Procrustes3D.h,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

     updated by Shireen Elhabian (Dec. 2013)
=========================================================================*/
#ifndef __Procrustes3D_h
#define __Procrustes3D_h

#include <vector>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>


#define VDimension 3

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
    typedef vnl_vector_fixed<double, VDimension> PointType;
    typedef std::vector<PointType> ShapeType;
    typedef ShapeType::iterator ShapeIteratorType;

    typedef std::vector<ShapeType> ShapeListType;
    typedef ShapeListType::iterator ShapeListIteratorType;

    typedef std::vector<SimilarityTransform3D> SimilarityTransformListType;
    typedef SimilarityTransformListType::iterator
    SimilarityTransformListIteratorType;

    typedef vnl_matrix_fixed<double, VDimension +1, VDimension +1> TransformMatrixType;
    typedef std::vector<TransformMatrixType> TransformMatrixListType;
    typedef TransformMatrixListType::iterator TransformMatrixIteratorType;

    Procrustes3D() : m_Scaling(true), m_RotationTranslation(true) { }
    Procrustes3D(bool do_scaling, bool do_rotation_translation) : m_Scaling(do_scaling), m_RotationTranslation(do_rotation_translation) { }

    bool GetScaling() const
    { return m_Scaling; }
    void ScalingOn()
    { m_Scaling = true; }
    void ScalingOff()
    { m_Scaling = false; }

    bool GetRotationTranslation() const
    { return m_RotationTranslation; }
    void RotationTranslationOn()
    { m_RotationTranslation = true; }
    void RotationTranslationOff()
    { m_RotationTranslation = false; }

    // Align a list of shapes using Generalized Procrustes Analysis
    void AlignShapes(SimilarityTransformListType & transforms,
                     ShapeListType & shapes);

    // Helper function to transform a shape by a similarity transform
    static void TransformShape(ShapeType & shape,
                               SimilarityTransform3D & transform);

    // Helper function to transform a list of shapes by a list of transforms
    static void TransformShapes(ShapeListType & shapes,
                                SimilarityTransformListType & transforms);

    static RealType ComputeSumOfSquares(ShapeListType & shapes);

    // Transform from Configuration space to Procrustes space.  Translation
    // followed by rotation and scaling.
    void ConstructTransformMatrices(SimilarityTransformListType & transforms,TransformMatrixListType & transformMatrices);
    void ConstructTransformMatrix(SimilarityTransform3D & transform,TransformMatrixType & transformMatrix);

    void ComputeMeanShape(ShapeType & mean, ShapeListType & shapeList);
    void ComputeCenterOfMass(ShapeType & shape, PointType& center);
    void CenterShape(ShapeType & shape);

    // this is the center which needed for translation of the shapes to coincide on the image origin
    // so that the whole object is in the image and won't go outside
    void ComputeCommonCenter(SimilarityTransformListType & transforms,PointType& center);

    // Align source shape to target using Ordinary Procrustes Analysis (translation, scaling and rotation)
    // the target shape is assumed to be centered at the origin
    void AlignSourceToTarget(SimilarityTransform3D & transform,
                        ShapeType & target, ShapeType & source);

    /* The median shape is
         defined as the shape with the minimum sum of Euclidean L1 norms to all
         other shapes in that group.  */
    int ComputeMedianShape(ShapeListType & shapeList);


private:

    // Align two shapes (rotation & scale) using Ordinary Procrustes Analysis
    void AlignTwoShapes(SimilarityTransform3D & transform,
                        ShapeType & shape1, ShapeType & shape2);

    //  const RealType SOS_EPSILON = 1.0e-8;

    bool m_Scaling; // a flag to factor out scaling
    bool m_RotationTranslation; // a flag for rotation + translation + (scale depending on m_Scaling), if false, the transformation will only be scaling
};



#endif
