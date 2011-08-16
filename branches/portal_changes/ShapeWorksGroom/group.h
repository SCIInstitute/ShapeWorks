/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: group.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_group_h
#define __st_group_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"

namespace shapetools
{
/**
 * \class group
 *
 *  Groups all labels into a single label (forground).
 */
template <class T, unsigned int D> 
class group : public tool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  
  group(param::parameterFile &);
  group() {}
  virtual ~group() {}
  
  virtual void operator()(typename image_type::Pointer);

  /** */
  const pixel_type foreground() const
  { return m_foreground; }
  pixel_type &foreground()
  { return m_foreground; }

  /** */
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
#include "group.txx"
#endif
