/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: split_segmentations.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_split_segmentations_h
#define __st_split_segmentations_h

#include "itkImage.h"
#include "tinyxml.h"
#include <sstream>
#include <string>
#include "tool.h"

namespace shapetools
{
/**
 * \class split_segmentations
 *
 */
template <class T, unsigned int D> 
class split_segmentations : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  
  split_segmentations(const char *fname);

  split_segmentations() {}
  virtual ~split_segmentations() {m_background = 0;}
  
  virtual void operator()();

  /** */
  const pixel_type background() const
  { return m_background; }  pixel_type &background()
  { return m_background; }

  struct ltfnc
  {
    bool operator()(const int a, const int b) const
    {
      return (a < b);
    }
  };

private: 
  pixel_type m_background;

};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "split_segmentations.txx"
#endif
