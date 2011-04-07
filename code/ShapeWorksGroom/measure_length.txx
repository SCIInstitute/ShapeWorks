/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: measure_length.txx,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st__measure_length_txx

#define __st__measure_length_txx



#include "pca.h"

#include "vnl/vnl_quaternion.h"

#include "vnl/vnl_cross.h"

#include "itkQuaternionRigidTransform.h"

#include "itkResampleImageFilter.h"

#include "itkLinearInterpolateImageFunction.h"

#include "itkNearestNeighborInterpolateImageFunction.h"

#include "object_writer.h"

#include "object_reader.h"

#include "itkImageRegionIteratorWithIndex.h"

#include "itkImageFileWriter.h"

#include <fstream>



namespace shapetools

{



template <class T, unsigned int D>

measure_length<T,D>::measure_length(param::parameterFile &pf)

{

  // Set some parameters.

  double g;

  bool ok = true;

  PARAMSET(pf, g, "background", 0, ok, 1.0);

  PARAMSET(pf, m_length_file, "length_file", 0, ok, "");

  m_background = static_cast<T>(g);



  if (ok == false)

    { throw param::Exception("measure_length:: missing parameters"); }

}



template <class T, unsigned int D> 

void measure_length<T,D>::operator()()

{

  if (D != 3)

    throw param::Exception("measure_length: this tool is only implemented for 3D");



  double sum = 0.0;

  double count = 0.0;



  std::vector<double> lengths;

  std::ofstream out(m_length_file.c_str());

  

  typename itk::ImageFileReader<image_type>::Pointer reader =

    itk::ImageFileReader<image_type>::New();



  typename std::vector<std::string>::const_iterator it

    = this->input_filenames().begin();



  // Loop through the images

  std::cout << "Raw Lengths" << std::endl;

  out << "Raw Lengths" << std::endl;

  for (; it != this->input_filenames().end(); it++)

    {

    reader->SetFileName( (*it).c_str() );

    reader->Update();



    typename image_type::Pointer testimg = image_type::New();

    testimg->SetRegions(reader->GetOutput()->GetRequestedRegion());

    testimg->CopyInformation(reader->GetOutput());

    testimg->Allocate();

    

    vnl_vector_fixed<double, 3> spacing;

    vnl_vector_fixed<double, 3> offset;

    vnl_vector_fixed<double, 3> origin;

    for (unsigned int i = 0; i < 3; i++)

      {

      spacing[i] = reader->GetOutput()->GetSpacing()[i];

      offset[i] = (double) (reader->GetOutput()->GetRequestedRegion().GetIndex()[i]);

      origin[i] = (double) (reader->GetOutput()->GetOrigin()[i]);

      }

//     std::cout << "spacing = " << spacing << std::endl;

//     std::cout << "offset = " << offset << std::endl;

//     std::cout << "origin = " << origin << std::endl;

    

    // Compute the eigenvectors.

    pca<T,D> pca_tool;

    pca_tool.background() = m_background;

    pca_tool( reader->GetOutput() );

    vnl_vector<double> v  = pca_tool.eigenvector(D-1);



    // Project all foreground points to the 1st principal axis.  Measure the

    // dist between the lowest/highest points.  Rotate to align w/ x axis for

    // easier pos/neg and distance calc.

//     vnl_vector_fixed<double,3> xaxis;

//     xaxis[0] = 1.0;

//     xaxis[1] = 0.0;

//     xaxis[2] = 0.0;



//     double angle = acos( dot_product(v, xaxis) );

//     vnl_vector<double> axis = vnl_cross_3d(v, xaxis);

//     double s  = sin(angle/2.0);

//     axis.normalize();

    

//     vnl_quaternion<double> q;

//     q.x() = axis[0] * s;     // Vx

//     q.y() = axis[1] * s;     // Vy

//     q.z() = axis[2] * s;     // Vz

//     q.r() = -cos(angle/2.0); // r

//     q.normalize();



    // loop through image

    typename itk::ImageRegionIteratorWithIndex<image_type>

      p(reader->GetOutput(), reader->GetOutput()->GetRequestedRegion());



    vnl_vector_fixed<double, 3> point;

    vnl_vector_fixed<double, 3> point2;

    double min = 0.0;

    double max = 0.0;

    double pos = 0.0;

    //    typename image_type::IndexType idx;

    typename image_type::PointType pt;



 

    for (; ! p.IsAtEnd(); ++p)

      {

      if (p.Get() != m_background )

        {

        for (unsigned int i = 0; i < 3; i++)

          {

          point[i] = origin[i] + (double)p.GetIndex()[i] * spacing[i];

          }

        //        std::cout << p.GetIndex() << " = " << point << std::endl;;

        // rotate

        //        q.rotate(point);



        // project to principal axis

        pos = dot_product(v,point);



        if (pos < min) min = pos;

        if (pos > max) max = pos;

        

        //        point2 = pos * v;

        //        for (unsigned int i = 0; i < 3; i++)

        //          { pt[i] = point2[i]; }

        

        //        std::cout << point2 << std::endl;

        

        //        testimg->TransformPhysicalPointToIndex(pt, idx);

        

        //        testimg->SetPixel( idx, 1.0);

        }

      



      }

      

//     typename itk::ImageFileWriter<image_type>::Pointer writer

//       = itk::ImageFileWriter<image_type>::New();

//     writer->SetInput(testimg);

//     writer->SetFileName("testoutput.mha");

//     writer->Update();

    

    //write to file

    double l = fabs(max-min);

    std::cout << l << std::endl;

    out << l << std::endl;

    sum += l;

    lengths.push_back(l);

    count += 1.0;

    }



  out << std::endl;

  double averagelen = sum / count;

  std::cout << "average length = " << averagelen << std::endl;

  out << "average length = " << averagelen << std::endl;

  std::cout << "Normalized Lengths" << std::endl;

  out << "Normalized Lengths len/avg" << std::endl;

  std::cout << "Normalized Lengths len/avg" << std::endl;



  sum = 0.0;

  for (unsigned int i = 0; i < (int)count; i++)

    {

    double ln = lengths[i] / averagelen;

    double lnd = lengths[i] - averagelen;

    sum += lnd * lnd;

    out << ln << std::endl;

    std::cout << ln << std::endl;

    }



  out << "Normalized Lengths avg/len" << std::endl;

  std::cout << "Normalized Lengths avg/len" << std::endl;



  for (unsigned int i = 0; i < (int)count; i++)

    {

    double ln = averagelen / lengths[i];

    out << ln << std::endl;

    std::cout << ln << std::endl;

    }

  

  sum /= count;

  double stdev = sqrt(sum);

  out << "Standard deviation = " << stdev << std::endl;

  std::cout  << "Standard deviation = " << stdev << std::endl;

  out.close();



    

}



} // end namespace



#endif

