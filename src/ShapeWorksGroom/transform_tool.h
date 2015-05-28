/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: transform_tool.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__transform_tool_h
#define __st__transform_tool_h

#include "itkParticleSystem.h"
#include "tool.h"

namespace shapetools
{
/**
 * \class transform_tool
 *
 * A generic interface for a shape tool with a transform.
 *
 */
template <class T, unsigned int D> 
class transform_tool : tool<T,D>
{
public:
  typedef T PixelType;
  typedef itk::Image<T, D> ImageType;

  transform_tool() {}
  virtual ~transform_tool() {}

  virtual void operator()(typename ImageType::Pointer) = 0;
  typedef typename itk::ParticleSystem<D>::TransformType transform_type;
  
  transform_type get_transform() const
  { return m_transform; }

  transform_type m_transform;
  
private:
  transform_tool &operator=(const transform_tool &); // purposely unimplemented
  transform_tool(const transform_tool &); // purposely unimplemented
};


}// end namespace 

#endif
