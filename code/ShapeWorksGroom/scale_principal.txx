/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: scale_principal.txx,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.3 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st__scale_principal_txx

#define __st__scale_principal_txx



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

#include "itkImageRegionIteratorWithIndex.h"



namespace shapetools

{



template <class T, unsigned int D>

scale_principal<T,D>::scale_principal(param::parameterFile &pf)

{

  // Set some parameters.

  double g;

  bool ok = true;

  PARAMSET(pf, g, "background", 0, ok, 1.0);

  m_background = static_cast<T>(g);



  PARAMSET(pf, m_transform_file, "scale_transform_file", 0, ok, "/dev/null\0");

  PARAMSET(pf, m_length_file, "scale_length_file", 0, ok, "/dev/null\0");

  

  if (ok == false)

    { throw param::Exception("scale_principal:: missing parameters"); }

  

}



template <class T, unsigned int D> 

void scale_principal<T,D>::operator()()

{

  // (only implemented for 3D right now)

  if (D != 3)

    throw param::Exception("scale_principal: this tool is only implemented for 3D");



  typename itk::ImageFileReader<image_type>::Pointer reader =

    itk::ImageFileReader<image_type>::New();

  typename itk::ImageFileWriter<image_type>::Pointer writer =

    itk::ImageFileWriter<image_type>::New();

  

  std::vector< TransformType >  transforms;

  

  typename std::vector<std::string>::const_iterator it

    = this->input_filenames().begin();

  typename std::vector<std::string>::const_iterator oit

    = this->output_filenames().begin();



  double count = 0;

  double sum =0.0;

  std::vector< double> lengths;

  std::ofstream out(m_length_file.c_str());

  out << "Raw Lengths" << std::endl;  

  // Loop through the images and find transforms

  for (; it != this->input_filenames().end(); it++, oit++)

    {

    reader->SetFileName( (*it).c_str() );

    reader->Update();



    // Compute the eigenvectors.

    pca<T,D> pca_tool;

    pca_tool.background() = m_background;

    pca_tool( reader->GetOutput() );



    // STEP 1

    

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



    double angle = acos( dot_product(v, ax1) );

    

    vnl_vector<double> axis = vnl_cross_3d(v, ax1);

    axis.normalize();

    std::cout << "v = " << v << std::endl;

    //std::cout << "axis = " << axis << std::endl;

    std::cout << "angle = " << angle << std::endl;

    

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



    double dot2 = dot_product(v2,ax2);

    double angle2;

    if (fabs(1.0 - fabs(dot2)) < 1.0e-8)

      {

      dot2 = 0.0;

      angle2 = 1.5708;

      }

    else angle2 = acos( dot2 );

    std::cout << "v2 = " << v2 << std::endl;

    std::cout << "angle2 = " << angle2 << std::endl;

    std::cout << std::endl;

    vnl_vector<double> axis2 = vnl_cross_3d(v2, ax2);

    double s2  = sin(angle2/2.0);

    axis2.normalize();



    vnl_quaternion<double> q2;    

    q2.x() = axis2[0] * s2;    // Vx

    q2.y() = axis2[1] * s2;    // Vy

    q2.z() = axis2[2] * s2;    // Vz

    q2.r() = -cos(angle2/2.0); // r

    q2.normalize();

     

//     // Compose the rotations

//     // Somehow the order of multiplication is backwards?!

//    vnl_quaternion<double> q3 = q1 * q2;

    vnl_quaternion<double> q3 = q1;

    

    

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

    vnl_matrix_fixed<double, 4,4> TT = q3.rotation_matrix_transpose_4();

    //    std::cout << "TT = " << TT << std::endl;



    

    transforms.push_back(TT);



    // STEP 2

    // DONE FINDING ROTATION

    // Now compute length on the principal axis

    

    typename itk::ImageRegionIteratorWithIndex<image_type>

      p(reader->GetOutput(), reader->GetOutput()->GetRequestedRegion());

    

    vnl_vector_fixed<double, 3> point;

    vnl_vector_fixed<double, 3> point2;

    double min = 0.0;

    double max = 0.0;

    double pos = 0.0;



    typename image_type::PointType pt;



    vnl_vector_fixed<double, 3> spacing;

    vnl_vector_fixed<double, 3> offset;

    vnl_vector_fixed<double, 3> origin;

    for (unsigned int i = 0; i < 3; i++)

      {

      spacing[i] = reader->GetOutput()->GetSpacing()[i];

      offset[i] = (double) (reader->GetOutput()->GetRequestedRegion().GetIndex()[i]);

      origin[i] = (double) (reader->GetOutput()->GetOrigin()[i]);

      }

    

    for (; ! p.IsAtEnd(); ++p)

      {

      if (p.Get() != m_background )

        {

        for (unsigned int i = 0; i < 3; i++)

          {

          point[i] = origin[i] + (double)p.GetIndex()[i] * spacing[i];

          }



        // project to principal axis

        pos = dot_product(v,point);



        if (pos < min) min = pos;

        if (pos > max) max = pos;        

        }

      }

    

    double l = fabs(max-min);

    std::cout << l << std::endl;

    out << l << std::endl;

    sum += l;

    lengths.push_back(l);

    count += 1.0;

    

    } // done processing all files



  // Now compute the scalings.  Ultimate transform is a rotation to X followed

  // by scaling to average length, followed by the inverse rotation to X



  //  std::cout << "count = " << count << std::endl;

  

  double averagelen = sum / count;



  //  std::cout << "Average len = " << averagelen << std::endl;

  

  // Write out the lengths

  //  sum = 0.0;

  for (unsigned int i = 0; i < (int)count; i++)

    {

    double ln = lengths[i] / averagelen;

    double lnd = lengths[i] - averagelen;

    //    sum += lnd * lnd;

    out << ln << std::endl;

    //    std::cout << ln << std::endl;

    }

  

  out << "Normalized Lengths avg/len" << std::endl;

  //  std::cout << "Normalized Lengths avg/len" << std::endl;



  

  for (unsigned int i = 0; i < (int)count; i++)

    {

    double ln = averagelen / lengths[i];

    out << ln << std::endl;

    //std::cout << ln << std::endl;



    vnl_matrix_fixed<double, 4,4> ts;

    ts.set_identity();

    ts(0,0) = ln;



    //    std::cout << "ts = " << ts << std::endl;

    //    std::cout << "transforms[i] = " << transforms[i] << std::endl;

    

    transforms[i] = transforms[i].transpose() * ts * transforms[i];

    



    //    std::cout << "final = " << transforms[i] << std::endl;

    }

  //  std::cout << "done setting transforms" << std::endl;

    //  sum /= count;

    //  double stdev = sqrt(sum);

  //    //  out << "Standard deviation = " << stdev << std::endl;

    //  std::cout  << "Standard deviation = " << stdev << std::endl;

  out.close();

  

  object_writer< TransformType > transwriter;

  transwriter.SetFileName(m_transform_file);

  transwriter.SetInput(transforms);

  transwriter.Update();



  

 // Verify

  //  std::cout << " --------------- " << std::endl;

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

