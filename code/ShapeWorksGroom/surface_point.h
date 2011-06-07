/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: surface_point.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_surface_point_h
#define __st_surface_point_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"

namespace shapetools
{
/**
 * \class surface_point
 *
 *  This is a simple class for writing a single point on the surface to a file.
 *  It searches the volume for an isovalue and records the first index
 *  (physical coordinates) that is within an epsilon of this isovalue.  This is
 *  useful for initializing particle systems through splitting.
 */
template <class T, unsigned int D> 
class surface_point : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  
  surface_point(param::parameterFile &);
  surface_point() {m_isovalue = 0.0;}
  virtual ~surface_point() {}
  
  virtual void operator()();

  /** */
  const pixel_type isovalue() const
  { return m_isovalue; }
  pixel_type &isovalue()
  { return m_isovalue; }

private: 
  pixel_type m_isovalue;
  std::vector<std::string> m_point_files;
};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "surface_point.txx"
#endif
