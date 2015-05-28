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

#ifdef SW_USE_OPENMP
#include <omp.h>
#endif /* SW_USE_OPENMP */

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

#pragma omp parallel
  {

#pragma omp for
  for (int i=0; i < m_input_filenames.size(); i++)
    {
      typename itk::ImageFileReader<image_type>::Pointer reader =
        itk::ImageFileReader<image_type>::New();
      typename itk::ImageFileWriter<image_type>::Pointer writer =
        itk::ImageFileWriter<image_type>::New();

      std::cout << m_input_filenames[i] << std::endl;
      reader->SetFileName( m_input_filenames[i].c_str() );
      reader->Update();
      this->m_tool->operator()(reader->GetOutput());

      writer->SetFileName( m_output_filenames[i].c_str() );
      writer->SetInput( reader->GetOutput() );
      writer->SetUseCompression( true );
      writer->Update();
    }
  }
}

}  // end namespace

#endif
