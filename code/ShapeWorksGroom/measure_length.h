/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: measure_length.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_measure_length_h
#define __st_measure_length_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"
#include "itkParticleSystem.h"
#include <string>

namespace shapetools
{
/**
 * \class measure_length
 *
 */
template <class T, unsigned int D> 
class measure_length : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  typedef typename  itk::ParticleSystem<D>::TransformType TransformType;

  measure_length(param::parameterFile &);
  measure_length() {};// { m_principal_axis = 0;}
  virtual ~measure_length() {}
  
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

  const std::string &length_file() const
  { return m_length_file; }
  std::string &length_file()
  { return m_length_file; }
private:
  std::string m_length_file;
  pixel_type m_foreground;
  pixel_type m_background;

};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "measure_length.txx"
#endif
