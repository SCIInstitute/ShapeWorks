/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: align_principal.txx,v $
  Date:      $Date: 2011/03/24 01:17:35 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__align_principal_txx
#define __st__align_principal_txx

#include "pca.h"
#include "vnl/vnl_quaternion.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_inverse.h"
#include "itkQuaternionRigidTransform.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "object_writer.h"
#include "object_reader.h"

namespace shapetools
{

template <class T, unsigned int D>
align_principal<T,D>::align_principal(const char * fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    // Set some parameters.
     this->m_background = 1.0;
    elem = docHandle.FirstChild( "background" ).Element();
    if (elem) this->m_background = static_cast<T>(atof(elem->GetText()));

    this->m_transform_file = "/dev/null\0";
    elem = docHandle.FirstChild( "transform_file" ).Element();
    if (elem)
    {
      this->m_transform_file = elem->GetText();
    }
    else
    {
      std::cerr << "align_principal:: missing parameters" << std::endl;    
    }

    vnl_vector<double> tmp(3);
  }
}



template <class T, unsigned int D> 
void align_principal<T,D>::operator()()
{
  // Flip info code:
  // 0 no flip
  // 1 flip x
  // 2 flip y
  // 3 flip x and y

  
  // (only implemented for 3D right now)
  if (D != 3)
  {
    std::cerr <<"align_principal: this tool is only implemented for 3D" << std::endl;
    throw 1;
  }

  typename itk::ImageFileReader<image_type>::Pointer reader =
    itk::ImageFileReader<image_type>::New();
  typename itk::ImageFileWriter<image_type>::Pointer writer =
    itk::ImageFileWriter<image_type>::New();
  
  std::vector< TransformType >  transforms;
  
  typename std::vector<std::string>::const_iterator it
    = this->input_filenames().begin();
  typename std::vector<std::string>::const_iterator oit
    = this->output_filenames().begin();
  typename std::vector<int>::const_iterator fit 
    = this->flip_info().begin();


  // Loop through the images and find transforms
  for (; it != this->input_filenames().end(); it++, oit++, fit++)
    {
    reader->SetFileName( (*it).c_str() );
    reader->Update();

    // Compute the eigenvectors.
    pca<T,D> pca_tool;
    pca_tool.background() = m_background;
    pca_tool( reader->GetOutput() );

    // Find the angle of rotation between the principal eigenvector and the
    // x-axis. Construct the quaternion from the axis and angle. The params
    // in order are x, y, z, r, transX, transY, transZ.
    // Reference: http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm
    vnl_vector<double> v  = pca_tool.eigenvector(D-1);
    vnl_vector<double> v2 = pca_tool.eigenvector(D-2);

    vnl_vector_fixed<double, 3> ax1;
    ax1[0] = 1.0; ax1[1] = 0.0; ax1[2] = 0.0;
    vnl_vector_fixed<double, 3> ax2;
    ax2[0] = 0.0; ax2[1] = 1.0; ax2[2] = 0.0;
    
  if (*fit == 1)
       {
       ax1 = -ax1;
       }
     if (*fit == 2)
       {
       ax2 = -ax2;
       }
     if (*fit == 3)
       {
       ax1 = -ax1;
       ax2 = -ax2;
       }
    
    double angle = acos( dot_product(v, ax1) );
    vnl_vector<double> axis = vnl_cross_3d(v, ax1);
    axis.normalize();

    vnl_quaternion<double> q1;
    double s1  = sin(angle/2.0);
    q1.x() = axis[0] * s1;     // Vx
    q1.y() = axis[1] * s1;     // Vy
    q1.z() = axis[2] * s1;     // Vz
    q1.r() = -cos(angle/2.0); // r
    q1.normalize();  // not really needed here, already normalized

    q1.r() = -q1.r();
    v2 = q1.rotate(v2);
    q1.r() = -q1.r();
    
    double angle2 = acos( dot_product(v2, ax2) );
    vnl_vector<double> axis2 = vnl_cross_3d(v2, ax2);
    double s2  = sin(angle2/2.0);
    axis2.normalize();
    
     //     std::cout << "axis 2 = " << axis2 << std::endl;
//     std::cout << "angle 2 = " << angle2 << std::endl;
    
     vnl_quaternion<double> q2;    
     q2.x() = axis2[0] * s2;    // Vx
     q2.y() = axis2[1] * s2;    // Vy
     q2.z() = axis2[2] * s2;    // Vz
     q2.r() = -cos(angle2/2.0); // r
     q2.normalize();
    
//     // Compose the rotations
//     // Somehow the order of multiplication is backwards?!
    vnl_quaternion<double> q3 = q1 * q2;
    
    q3.normalize();
    
    itk::Array<double> params(D+4);
    params[0] = q3.x(); // Vx
    params[1] = q3.y(); // Vy
    params[2] = q3.z(); // Vz
    params[3] = q3.r(); // r
    params[4] = 0.0; // transX
    params[5] = 0.0; // transY
    params[6] = 0.0; // transZ
    
    typename itk::QuaternionRigidTransform<double>::Pointer trans
      = itk::QuaternionRigidTransform<double>::New();
    trans->SetParameters(params);
    
//     double c = cos(angle);
//     double s = sin(angle);
//     double t = 1.0-c;
//     double x = axis[0];
//     double y = axis[1];
//     double z = axis[2];    
    
//     vnl_matrix_fixed<double, 4,4> T;
//     T(0,0) = t*x*x + c;   T(0,1) = t*x*y - z*s;  T(0,2) = t*x*z + y*s;  T(0,3) = 0.0;
//     T(1,0) = t*x*y + z*s; T(1,1) = t*y*y + c;    T(1,2) = t*y*z - x*s;  T(1,3) = 0.0;
//     T(2,0) = t*x*z - y*s; T(2,1) = t*y*z + x*s;  T(2,2) = t*z*z + c;    T(2,3) = 0.0;
//     T(3,0) = 0.0;         T(3,1) = 0.0;          T(3,2) = 0.0;          T(3,3) = 1.0;
      

    vnl_matrix_fixed<double, 4,4> TT = q3.rotation_matrix_transpose_4();
//     double tmp = T(0,2);
//        T(0,2) = T(2,0);
//        T(2,0) = tmp;
// double       tmp = T(1,0);
//        T(1,0) = T(0,1);
//        T(0,1) = tmp;

    transforms.push_back(TT);
    //    transforms.push_back(q3.rotation_matrix_transpose_4().transpose());
    
    // Perform the transformation.
    typename itk::LinearInterpolateImageFunction<image_type,double>::Pointer
      interp = itk::LinearInterpolateImageFunction<image_type,double>::New();

    //    typename itk::NearestNeighborInterpolateImageFunction<image_type,double>::Pointer
    //      interp = itk::NearestNeighborInterpolateImageFunction<image_type,double>::New();
    
    typename itk::ResampleImageFilter<image_type, image_type>::Pointer resampler
      = itk::ResampleImageFilter<image_type, image_type>::New();
    resampler->SetOutputParametersFromImage(reader->GetOutput());
    resampler->SetTransform(trans);
    resampler->SetInterpolator(interp);
    resampler->SetInput(reader->GetOutput());
    resampler->Update();
    
    writer->SetFileName( (*oit).c_str() );
    writer->SetInput( resampler->GetOutput() );
    writer->Update();
    }
  
  
  object_writer< TransformType > transwriter;
  transwriter.SetFileName(m_transform_file);
  transwriter.SetInput(transforms);
  transwriter.Update();

  // Verify
  std::cout << " --------------- " << std::endl;
  object_reader<TransformType> transreader;
  transreader.SetFileName(m_transform_file.c_str());
  transreader.Update();
  for (unsigned int i = 0; i < transreader.GetOutput().size(); i++)
    {
    std::cout << transreader.GetOutput()[i] << std::endl;
    }
  
}

} // end namespace

#endif
