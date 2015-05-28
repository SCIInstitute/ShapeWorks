/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: isotropic.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_isotropic_h
#define __st_isotropic_h

#include "itkImage.h"
#include "tinyxml.h"
#include <sstream>
#include <string>
#include "tool.h"

namespace shapetools
{
/**
 * \class isotropic
 *
 */
template <class T, unsigned int D> 
class isotropic : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  
  isotropic(const char *fname);

  isotropic() {}
  virtual ~isotropic() {}
  
  virtual void operator()();

private: 
 
};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "isotropic.txx"
#endif
