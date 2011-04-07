/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: hole_fill.h,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st_hole_fill_h

#define __st_hole_fill_h



#include "itkImage.h"

#include "param.h"

#include "tool.h"

#include "itkConnectedThresholdImageFilter.h"



namespace shapetools

{

/**

 * \class hole_fill

 *

 *  Hole_Fill removes holes in segmentations using the following algorithm.

 *  1.  Assume a binary (foreground/background) image

 *  2.  Copy the image.

 *  3.  Flood fill the background of the copy image outside of the segmentation

 *  (user specified seed point) with the foreground value.

 *  3.  Invert the copy (switch foreground/background values)

 *  4.  "or" the copy with the original image

 *  

 */

template <class T, unsigned int D> 

class hole_fill : public tool<T, D>

{

public:

  typedef T pixel_type;

  typedef itk::Image<T, D> image_type;

  typedef itk::ConnectedThresholdImageFilter<image_type, image_type> flood_fill_filter_type;



  

  hole_fill(param::parameterFile &);

  hole_fill() {}

  virtual ~hole_fill() {}

  

  virtual void operator()(typename image_type::Pointer);



  /** Value of the foreground. */

  const pixel_type foreground() const

  { return m_foreground; }

  pixel_type &foreground()

  { return m_foreground; }



  /** Value of the background. */

  const pixel_type background() const

  { return m_background; }

  pixel_type &background()

  { return m_background; }



  /** Seed point that is assumed to be somewhere in the background region.*/

  const typename flood_fill_filter_type::IndexType &seed() const

  { return m_seed; }

  typename flood_fill_filter_type::IndexType &seed()

  { return m_seed; }

  

private: 

  pixel_type m_foreground;

  pixel_type m_background;

  typename flood_fill_filter_type::IndexType m_seed;

};



 

} // end namespace 

#endif



#ifndef ST_MANUAL_INSTANTIATION

#include "hole_fill.txx"

#endif

