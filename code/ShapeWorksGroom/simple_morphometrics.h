/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: simple_morphometrics.h,v $
  Date:      $Date: 2009/06/09 23:09:18 $
  Version:   $Revision: 1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_simple_morphometrics_h
#define __st_simple_morphometrics_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"
#include "itkParticleSystem.h"
#include <string>

namespace shapetools
{
/**
 * \class simple_morphometrics
 *
 */
template <class T, unsigned int D> 
class simple_morphometrics : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;

  simple_morphometrics(param::parameterFile &);
  simple_morphometrics() {};// { m_principal_axis = 0;}
  virtual ~simple_morphometrics() {}
  
  virtual void operator()();
  
  /** */
  const pixel_type foreground() const
  { return m_foreground; }
  pixel_type &foreground()
  { return m_foreground; }

private:
  pixel_type m_foreground;

};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "simple_morphometrics.txx"
#endif
