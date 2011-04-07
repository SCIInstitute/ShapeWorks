/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: blur.h,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st_blur_h

#define __st_blur_h



#include "itkImage.h"

#include "param.h"

#include "tool.h"



namespace shapetools

{

/**

 * \class blur

 *

 *

 */

template <class T, unsigned int D> 

class blur : public tool<T, D>

{

public:

  typedef T pixel_type;

  typedef itk::Image<T, D> image_type;

  

  blur(param::parameterFile &);

  blur() { m_sigma = 1.0; }

  virtual ~blur() {}

  

  virtual void operator()(typename image_type::Pointer);



  /** */

  double sigma() const

  { return m_sigma; }

  double &sigma()

  { return m_sigma; }

  

private: 

  double m_sigma;

  

};



 

} // end namespace 

#endif



#ifndef ST_MANUAL_INSTANTIATION

#include "blur.txx"

#endif

