/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: center.h,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st_center_h

#define __st_center_h



#include "itkImage.h"

#include "param.h"

#include "transform_tool.h"



namespace shapetools

{

/**

 * \class center

 *

 * Translates the center of mass of a segmentation mask to the origin of the

 * image.  Warning! This filter will crop regions of the shifted segmentation

 * that do no fit in the original image domain.  The segmentation mask is

 * defined as the pixels of value m_foreground.  All other pixels are set to

 * m_background.  If center_origin is set, then this filter will reset the

 * origin of the image to the center of the image (by default the origin is

 * usually in the the upper, left-hand corner of the image).

 *

 */

template <class T, unsigned int D> 

class center : public transform_tool<T, D>

{

public:

  typedef T pixel_type;

  typedef itk::Image<T, D> image_type;

  

  center(param::parameterFile &);

  center() {}

  virtual ~center() {}

  

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



  /** */

  bool center_origin() const

  { return m_center_origin; }

  bool &center_origin()

  { return m_center_origin; }



private: 

  pixel_type m_foreground;

  pixel_type m_background;

  bool m_center_origin;

};



 

} // end namespace 

#endif



#ifndef ST_MANUAL_INSTANTIATION

#include "center.txx"

#endif

