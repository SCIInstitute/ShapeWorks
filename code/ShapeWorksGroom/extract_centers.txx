/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: extract_centers.txx,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st__extract_centers_txx

#define __st__extract_centers_txx



#include "itkExtractImageFilter.h"

#include "bounding_box.h"



namespace shapetools

{



template <class T, unsigned int D>

extract_centers<T,D>::extract_centers(param::parameterFile &pf)

{

  // Set some parameters.

  double f,g;

  bool ok = true;

  PARAMSET(pf, f, "foreground", 0, ok, 0.0);

  PARAMSET(pf, g, "background", 0, ok, 1.0);



  m_foreground = static_cast<T>(f);

  m_background = static_cast<T>(g);



  int s;

  for (unsigned int i = 0; i < D; i++)

    {

    PARAMSET(pf, s, "center_size", i, ok, 0);

    m_center_size[i] = s;

    }

  

  if (ok == false)

    { throw param::Exception("extract_centers:: missing parameters"); }

}



template <class T, unsigned int D> 

void extract_centers<T,D>::operator()()

{

  typename itk::ImageFileReader<image_type>::Pointer reader =

    itk::ImageFileReader<image_type>::New();

  typename itk::ImageFileWriter<image_type>::Pointer writer =

    itk::ImageFileWriter<image_type>::New();

  

  typename std::vector<std::string>::const_iterator it

    = this->input_filenames().begin();

  typename std::vector<std::string>::const_iterator oit

    = this->output_filenames().begin();



  

  for (; it != this->input_filenames().end(); it++, oit++)

    {

    reader->SetFileName( (*it).c_str() );

    reader->UpdateLargestPossibleRegion();



    // Determine the corner of the extraction region (user has already specified

    // the size).

    typename image_type::RegionType::IndexType corner;

    typename image_type::RegionType::IndexType lower

      = reader->GetOutput()->GetBufferedRegion().GetIndex();

    typename image_type::RegionType::SizeType size

      = reader->GetOutput()->GetBufferedRegion().GetSize();

    

    for (unsigned i = 0; i < D; i++)

      { corner[i] = lower[i] + (size[i] / 2) - (m_center_size[i] / 2); }



    typename image_type::RegionType cropRegion;

    cropRegion.SetSize(m_center_size);

    cropRegion.SetIndex(corner);



    typename itk::ExtractImageFilter<image_type, image_type>::Pointer extractor

      = itk::ExtractImageFilter<image_type, image_type>::New();

    extractor->SetInput(reader->GetOutput());

    extractor->SetExtractionRegion(cropRegion);

    extractor->UpdateLargestPossibleRegion();



    // Make sure the origin is at the center of the image.

    double orig[D];

    for (unsigned int i = 0; i < D; i++)

      {  orig[i] = - static_cast<double>(m_center_size[i] /2);  }

    extractor->GetOutput()->SetOrigin( orig );

    

    writer->SetFileName( (*oit).c_str() );

    writer->SetInput( extractor->GetOutput());

    writer->Update();

    }

}



} // end namespace



#endif

