/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: scale_principal.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_scale_principal_h
#define __st_scale_principal_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"
#include "itkParticleSystem.h"
#include <string>

namespace shapetools
{
/**
 * \class scale_principal
 *
 * Scales the principal axes of each segmentation with a given axis.  The given
 * axis is assumed to be unit length.
 */
template <class T, unsigned int D> 
class scale_principal : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  typedef typename  itk::ParticleSystem<D>::TransformType TransformType;

  scale_principal(param::parameterFile &);
  scale_principal() {};// { m_principal_axis = 0;}
  virtual ~scale_principal() {}
  
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

  std::string m_transform_file;
  std::string m_length_file;
};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "scale_principal.txx"
#endif
