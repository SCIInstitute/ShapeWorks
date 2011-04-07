/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: surface_point.txx,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st_surface_point_txx

#define __st_surface_point_txx



#include "surface_point.h"

#include "itkImageRegionIteratorWithIndex.h"

#include "itkZeroCrossingImageFilter.h"

#include <cmath>



namespace shapetools

{



template <class T, unsigned int D>

surface_point<T,D>::surface_point(param::parameterFile &pf)

{

  // Set some parameters.

  bool ok = true;

  PARAMSET(pf, m_isovalue, "surface_isovalue", 0, ok, 0.0);



  // Compile the list of input files.

  ok = true;

  for (unsigned int i = 0; ok == true; i++)

    {

    std::string tmp;

    PARAMSET(pf, tmp, "point_files", i, ok, "/dev/null\0");

    if (ok == true) { m_point_files.push_back(tmp); }

    }

  

}



template <class T, unsigned int D> 

void surface_point<T,D>::operator()()

{

  if (m_point_files.size() < this->input_filenames().size())

    {

    throw param::Exception("surface_point:: output list size does not match input list size");

    }

  const pixel_type epsilon = static_cast<pixel_type>(0.3);

  typename itk::ImageFileReader<image_type>::Pointer reader =

    itk::ImageFileReader<image_type>::New();

  typename std::vector<std::string>::const_iterator fit

    = this->input_filenames().begin();

  typename std::vector<std::string>::const_iterator oit

    = m_point_files.begin();

  

  for (; fit != this->input_filenames().end(); fit++, oit++)

    {

    reader->SetFileName( (*fit).c_str() );



    typename itk::ZeroCrossingImageFilter<image_type, image_type>::Pointer zc

      = itk::ZeroCrossingImageFilter<image_type, image_type>::New();

    zc->SetInput(reader->GetOutput());

    zc->SetForegroundValue(1.0);

    zc->SetBackgroundValue(0.0);

    zc->UpdateLargestPossibleRegion();

    

    itk::ImageRegionIteratorWithIndex<image_type> it(zc->GetOutput(),

                                    zc->GetOutput()->GetBufferedRegion());

    itk::Point<double, D> point;

    point.Fill(0.0);



    // Find the first surface point.

    for (it.GoToBegin(); ! it.IsAtEnd(); ++it)

      {

      if (it.Get() == 1.0 )

        {

        // Get the physical index from the image index.

        zc->GetOutput()->TransformIndexToPhysicalPoint(it.GetIndex(), point);

        //        break;

        }

      }



    // Write the point file.

    std::ofstream out( (*oit).c_str() );

    if ( !out )

      { throw param::Exception("surface_point:: could not open output file"); }



    //    out << 1 << std::endl; // number of points

    for (unsigned int i = 0; i < D; i++)

      { out << point[i] << " "; }

    out << std::endl;

    out.close();



    }

}



} // end namespace



#endif

