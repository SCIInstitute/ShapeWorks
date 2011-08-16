/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: bounding_box.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_bounding_box_h
#define __st_bounding_box_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"
#include "itkImageRegion.h"

namespace shapetools
{
/**
 * \class bounding_box
 *
 * Computes the smallest bounding box around the foreground (in pixel coordinates).
 */
template <class T, unsigned int D> 
class bounding_box : public tool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  typedef typename image_type::RegionType region_type;
  
  bounding_box(param::parameterFile &);
  bounding_box() {}
  virtual ~bounding_box() {}
  
  virtual void operator()(typename image_type::Pointer);

  /** */
  const pixel_type background() const
  { return m_background; }
  pixel_type &background()
  { return m_background; }

  /** */
  const region_type& region() const
  { return m_region; }
  region_type &region()
  { return m_region; }

private: 
  pixel_type m_background;

  region_type m_region;
};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "bounding_box.txx"
#endif
