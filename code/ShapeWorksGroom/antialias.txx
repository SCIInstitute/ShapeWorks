/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: antialias.txx,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st_antialias_txx

#define __st_antialias_txx



#include "antialias.h"

#include "itkImageRegionIterator.h"

#include "itkAntiAliasBinaryImageFilter.h"



namespace shapetools

{



template <class T, unsigned int D>

antialias<T,D>::antialias(param::parameterFile &pf)

{

  // Set some parameters.

  bool ok;

  PARAMSET(pf, m_iterations, "antialias_iterations", 0, ok, 50);

}



template <class T, unsigned int D> 

void antialias<T,D>::operator()(typename image_type::Pointer img)

{

  typename itk::AntiAliasBinaryImageFilter<image_type, image_type>::Pointer anti

    = itk::AntiAliasBinaryImageFilter<image_type, image_type>::New();

  anti->SetInput(img);

  anti->SetNumberOfIterations(m_iterations);

  anti->SetMaximumRMSError(0.0);

  anti->Update();



  // Copy resampled image back to the original image (probably can do this

  // more efficiently --jc).

  itk::ImageRegionIterator<image_type> oit(img, img->GetBufferedRegion());

  itk::ImageRegionIterator<image_type> it(anti->GetOutput(),

                                          img->GetBufferedRegion());

  for ( ; ! it.IsAtEnd(); ++it, ++oit)  { oit.Set(it.Get()); }

}



} // end namespace



#endif

