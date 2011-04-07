/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: blur.txx,v $

  Date:      $Date: 2011/03/24 19:21:15 $

  Version:   $Revision: 1.3 $

  Author:    $Author: rantao $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st_blur_txx

#define __st_blur_txx



#include "blur.h"

#include "itkImageRegionIterator.h"

#include "itkDiscreteGaussianImageFilter.h"



namespace shapetools

{



template <class T, unsigned int D>

blur<T,D>::blur(param::parameterFile &pf)

{

  // Set some parameters.

  bool ok;

  PARAMSET(pf, m_sigma, "blur_sigma", 0, ok, 1.0);

}



template <class T, unsigned int D> 

void blur<T,D>::operator()(typename image_type::Pointer img)

{

  typename itk::DiscreteGaussianImageFilter<image_type, image_type>::Pointer blur

    = itk::DiscreteGaussianImageFilter<image_type, image_type>::New();

  blur->SetInput(img);
  blur->SetVariance(m_sigma * m_sigma);
  blur->SetUseImageSpacingOff();
  blur->Update();



  // Copy resampled image back to the original image (probably can do this

  // more efficiently --jc).

  itk::ImageRegionIterator<image_type> oit(img, img->GetBufferedRegion());

  itk::ImageRegionIterator<image_type> it(blur->GetOutput(),

                                          img->GetBufferedRegion());

  for ( ; ! it.IsAtEnd(); ++it, ++oit)  { oit.Set(it.Get()); }

}



} // end namespace



#endif

