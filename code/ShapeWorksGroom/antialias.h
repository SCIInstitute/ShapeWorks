/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: antialias.h,v $

  Date:      $Date: 2011/03/24 01:17:35 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st_antialias_h

#define __st_antialias_h



#include "itkImage.h"

#include "param.h"

#include "tool.h"



namespace shapetools

{

/**

 * \class antialias

 *

 *

 */

template <class T, unsigned int D> 

class antialias : public tool<T, D>

{

public:

  typedef T pixel_type;

  typedef itk::Image<T, D> image_type;

  

  antialias(param::parameterFile &);

  antialias() { m_iterations = 50; }

  virtual ~antialias() {}

  

  virtual void operator()(typename image_type::Pointer);



  /** */

  int iterations() const

  { return m_iterations; }

  int &iterations()

  { return m_iterations; }

  

private: 

  int m_iterations;

  

};



 

} // end namespace 

#endif



#ifndef ST_MANUAL_INSTANTIATION

#include "antialias.txx"

#endif

