/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: relabel.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_relabel_txx
#define __st_relabel_txx

#include "relabel.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace shapetools
{

template <class T, unsigned int D>
relabel<T,D>::relabel(param::parameterFile &pf)
{
 // Set some parameters.
  double f,g;
  bool ok = true;
  PARAMSET(pf, f, "foreground", 0, ok, 0.0);
  PARAMSET(pf, g, "background", 0, ok, 1.0);

  m_foreground = static_cast<T>(f);
  m_background = static_cast<T>(g);

  if (ok == false)
    {  throw param::Exception("relabel:: missing parameters"); }
}

template <class T, unsigned int D> 
void relabel<T,D>::operator()(typename image_type::Pointer img)
{
  // Loop through the image and change all foreground values to the new
  // foreground value.

  itk::ImageRegionIterator<image_type> it(img,img->GetBufferedRegion());
  for (; !it.IsAtEnd(); ++it)
    {
    if (it.Get() != m_background)
      {
      it.Set(m_foreground);
      }
    }
}

} // end namespace

#endif
