/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: relabel.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_relabel_h
#define __st_relabel_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"
#include "itkConnectedThresholdImageFilter.h"

namespace shapetools
{
/**
 * \class relabel
 *
 *  Relabel changes the foreground value to a user-specified value.  The
 *  1.  Assume a binary (foreground/background) image
 *  2.  Loop through the image and change all non-background values to the
 *  specified foreground value.
 *  
 */
template <class T, unsigned int D> 
class relabel : public tool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  
  relabel(param::parameterFile &);
  relabel() {}
  virtual ~relabel() {}
  
 virtual void operator()(typename image_type::Pointer);


 /** Value of the foreground. */
  const pixel_type foreground() const
  { return m_foreground; }
  pixel_type &foreground()
  { return m_foreground; }

  /** Value of the background. */
  const pixel_type background() const
  { return m_background; }
  pixel_type &background()
  { return m_background; }

private: 
  pixel_type m_foreground;
  pixel_type m_background;
};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "relabel.txx"
#endif
