/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: batchtool.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__batchtool_txx
#define __st__batchtool_txx

#include <string>
#include <vector>
#include "tool.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkConnectedComponentImageFilter.h"

namespace shapetools
{

template <class T, unsigned int D>
void batchtool<T,D>::operator()()
{
  typename itk::ImageFileReader<image_type>::Pointer reader =
    itk::ImageFileReader<image_type>::New();
  typename itk::ImageFileWriter<image_type>::Pointer writer =
    itk::ImageFileWriter<image_type>::New();

  typename std::vector<std::string>::const_iterator it
    = m_input_filenames.begin();
  typename std::vector<std::string>::const_iterator oit
    = m_output_filenames.begin();
  
  for (; it != m_input_filenames.end(); it++, oit++)
    {
    std::cout << *it << std::endl;
    reader->SetFileName( (*it).c_str() );
    reader->Update();
    this->m_tool->operator()(reader->GetOutput());

    writer->SetFileName( (*oit).c_str() );
    writer->SetInput( reader->GetOutput() );
    writer->SetUseCompression( true );
    writer->Update();
    }
}

}  // end namespace

#endif
