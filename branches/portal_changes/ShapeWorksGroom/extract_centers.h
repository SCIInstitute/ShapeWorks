/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: extract_centers.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_extract_centers_h
#define __st_extract_centers_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"

namespace shapetools
{
/**
 * \class extract_centers
 *
 * Crops to a specified size around the center of the image.
 */
template <class T, unsigned int D> 
class extract_centers : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  
  extract_centers(param::parameterFile &);
  extract_centers() {}
  virtual ~extract_centers() {}
  
  virtual void operator()();

  /** */
  const pixel_type foreground() const
  { return m_foreground; }
  pixel_type &foreground()
  { return m_foreground; }

  /** */
  const pixel_type background() const
  { return m_background; }  pixel_type &background()
  { return m_background; }

private: 
  pixel_type m_foreground;
  pixel_type m_background;

  typename image_type::RegionType::SizeType m_center_size;
};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "extract_centers.txx"
#endif
