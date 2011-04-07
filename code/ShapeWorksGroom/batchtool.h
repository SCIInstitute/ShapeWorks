/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: batchtool.h,v $

  Date:      $Date: 2011/03/23 22:40:11 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st__batchtool_h

#define __st__batchtool_h



#include <string>

#include <vector>

#include "tool.h"



namespace shapetools

{

/**

 * \class batchtool

 *

 * Applies tools to a list of image files.  Each image file is read and

 * processed by a single instantiation of a tool.

 *

 */

template <class T, unsigned int D> 

class batchtool

{

public:

  typedef T PixelType;

  typedef tool<T,D> tool_type;

  typedef itk::Image<T, D> image_type;



  batchtool() {}

  virtual ~batchtool() {}



  virtual void operator()();



  /** Get/Set the tool to apply in batch. */

  const tool_type* tool_to_use() const

  { return m_tool; }

  tool_type *& tool_to_use()

  { return m_tool; }

  void set_tool(tool_type *t)

  { m_tool = t; }



  /** Get/Set the filenames to read */

  const std::vector<std::string> &input_filenames() const

  { return m_input_filenames; }

  std::vector<std::string> &input_filenames()

  { return m_input_filenames; }



  /** Get/Set the filenames to write */

  const std::vector<std::string> &output_filenames() const

  { return m_output_filenames; }

  std::vector<std::string> &output_filenames()

  { return m_output_filenames; }

  const std::vector<std::string> &input_seg_filenames() const
  { return m_input_seg_filenames; }
  std::vector<std::string> &input_seg_filenames()
  { return m_input_seg_filenames; }

  const std::vector<std::string> &output_seg_filenames() const
  { return m_output_seg_filenames; }
  std::vector<std::string> &output_seg_filenames()
  { return m_output_seg_filenames; }

  tool_type *m_tool;

  std::vector<std::string> m_input_filenames;

  std::vector<std::string> m_output_filenames;

  std::vector<std::string> m_input_seg_filenames;
  std::vector<std::string> m_output_seg_filenames;
  
private:

  batchtool &operator=(const batchtool &); // purposely unimplemented

  batchtool(const batchtool &); // purposely unimplemented

};





}// end namespace 



#endif



#ifndef ST_MANUAL_INSTANTIATION

#include "batchtool.txx"

#endif

