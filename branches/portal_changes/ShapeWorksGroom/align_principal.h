/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: align_principal.h,v $
  Date:      $Date: 2011/03/24 01:17:35 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_align_principal_h
#define __st_align_principal_h

#include "itkImage.h"
#include "param.h"
#include "tool.h"
#include "itkParticleSystem.h"
#include <string>

namespace shapetools
{
/**
 * \class align_principal
 *
 * Aligns the principal axes of each segmentation with a given axis.  The given
 * axis is assumed to be unit length.
 */
template <class T, unsigned int D> 
class align_principal : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  typedef typename  itk::ParticleSystem<D>::TransformType TransformType;

  align_principal(param::parameterFile &);
  align_principal() {};// { m_principal_axis = 0;}
  virtual ~align_principal() {}
  
  virtual void operator()();


 /** Get/Set the filenames to read */
  const std::vector<int> &flip_info() const
  { return m_flip_info; }
  std::vector<int> &flip_info()
  { return m_flip_info; }
  
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
  std::vector<int> m_flip_info;
};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "align_principal.txx"
#endif
