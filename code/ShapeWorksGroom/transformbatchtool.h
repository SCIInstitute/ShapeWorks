/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: transformbatchtool.h,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st__transformbatchtool_h

#define __st__transformbatchtool_h



#include <string>

#include <vector>

#include "tool.h"

#include "batchtool.h"

#include "itkParticleSystem.h"



namespace shapetools

{

/**

 * \class transformbatchtool

 *

 */

template <class T, unsigned int D> 

class transformbatchtool : public batchtool<T,D>

{

public:

  typedef T PixelType;

  typedef tool<T,D> tool_type;

  typedef itk::Image<T, D> image_type;



  typedef typename itk::ParticleSystem<D>::TransformType TransformType;



  

  transformbatchtool() {}

  virtual ~transformbatchtool() {}



  virtual void operator()();



  /** Get/Set the transform filename to write */

  const std::string &tranform_file() const

  { return m_transform_file; }

  void transform_file(const std::string &s)

  { m_transform_file = s; }

  void transform_file(const char *s)

  { m_transform_file = s ; }

  

  std::string m_transform_file;

private:

  

  transformbatchtool &operator=(const transformbatchtool &); // purposely unimplemented

  transformbatchtool(const transformbatchtool &); // purposely unimplemented

};





}// end namespace 



#endif



#ifndef ST_MANUAL_INSTANTIATION

#include "transformbatchtool.txx"

#endif

