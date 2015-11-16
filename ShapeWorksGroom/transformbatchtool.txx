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

#ifdef SW_USE_OPENMP
#include <omp.h>
#endif /* SW_USE_OPENMP */

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
void transformbatchtool<T, D>::operator() () {

  std::vector< TransformType >  transforms;

  transforms.resize( this->m_input_filenames.size() );

//#pragma omp parallel
  {

//#pragma omp for
    for ( int i = 0; i < this->m_input_filenames.size(); i++ )
    {

//      transform_tool<T, D> tool = *(reinterpret_cast<transform_tool<T, D>*>( this->m_tool ));
      typename itk::ImageFileReader<image_type>::Pointer reader =
        itk::ImageFileReader<image_type>::New();
      typename itk::ImageFileWriter<image_type>::Pointer writer =
        itk::ImageFileWriter<image_type>::New();

      std::cout << this->m_input_filenames[i] << std::endl;
      reader->SetFileName( this->m_input_filenames[i].c_str() );
      reader->Update();
      this->m_tool->operator() ( reader->GetOutput() );

      TransformType transform = reinterpret_cast<transform_tool<T, D>*>( this->m_tool )->get_transform();
      transforms[i] = transform;

      if ( this->m_write_individual_transform )
      {
        // write out each transform separately
        std::string transform_filename = this->m_input_filenames[i] + ".transform";
        std::ofstream out( transform_filename.c_str() );
        out << transform;
        out.close();
      }

      writer->SetFileName( this->m_output_filenames[i].c_str() );
      writer->SetInput( reader->GetOutput() );
      writer->SetUseCompression( true );
      writer->Update();
    }
  }

  object_writer<TransformType> transwriter;
  transwriter.SetInput( transforms );
  transwriter.SetFileName( this->m_transform_file.c_str() );
  transwriter.Update();

  // Verify
  std::cout << " --------------- " << std::endl;
  object_reader<TransformType> transreader;
  transreader.SetFileName( this->m_transform_file.c_str() );
  transreader.Update();
  for ( unsigned int i = 0; i < transreader.GetOutput().size(); i++ )
  {
    std::cout << transreader.GetOutput()[i] << std::endl;
  }
}
}  // end namespace

#endif /* ifndef __st__transformbatchtool_txx */
