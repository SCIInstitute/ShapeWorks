/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: transformbatchtool.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__transformbatchtool_txx
#define __st__transformbatchtool_txx

#include <string>
#include <vector>
#include "tool.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkConnectedComponentImageFilter.h"
#include "object_reader.h"
#include "object_writer.h"

namespace shapetools
{

template <class T, unsigned int D>
void transformbatchtool<T,D>::operator()()
{
  
  typename itk::ImageFileReader<image_type>::Pointer reader =
    itk::ImageFileReader<image_type>::New();
  typename itk::ImageFileWriter<image_type>::Pointer writer =
    itk::ImageFileWriter<image_type>::New();

  std::vector< TransformType >  transforms;
  
  typename std::vector<std::string>::const_iterator it  = this->m_input_filenames.begin();
  typename std::vector<std::string>::const_iterator oit = this->m_output_filenames.begin();
  
  for (; it != this->m_input_filenames.end(); it++, oit++)
    {
    std::cout << *it << std::endl;
    reader->SetFileName( (*it).c_str() );
    reader->Update();
    this->m_tool->operator()(reader->GetOutput());

    transforms.push_back( reinterpret_cast<transform_tool<T,D> *>(this->m_tool)->get_transform() );

    writer->SetFileName( (*oit).c_str() );
    writer->SetInput( reader->GetOutput() );
    writer->Update();
    }

  object_writer<TransformType> transwriter;
  transwriter.SetInput(transforms);
  transwriter.SetFileName(this->m_transform_file.c_str());
  transwriter.Update();

  // Verify
  std::cout << " --------------- " << std::endl;
  object_reader<TransformType> transreader;
  transreader.SetFileName(this->m_transform_file.c_str());
  transreader.Update();
  for (unsigned int i = 0; i < transreader.GetOutput().size(); i++)
    {
    std::cout << transreader.GetOutput()[i] << std::endl;
    }
  
  
}

}  // end namespace

#endif
