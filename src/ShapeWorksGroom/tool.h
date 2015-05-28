/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: tool.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__tool_h
#define __st__tool_h

#include "itkImage.h"

namespace shapetools
{
/**
 * \class tool
 *
 * A generic interface for a shape tool.
 *
 */
template <class T, unsigned int D> 
class tool
{
public:
  typedef T PixelType;
  typedef itk::Image<T, D> ImageType;

  tool() {}
  virtual ~tool() {}

  virtual void operator()(typename ImageType::Pointer) = 0;

private:
  tool &operator=(const tool &); // purposely unimplemented
  tool(const tool &); // purposely unimplemented
};


}// end namespace 

#endif
