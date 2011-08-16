/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: split_segmentations.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__split_segmentations_txx
#define __st__split_segmentations_txx

#include "pca.h"
#include "vnl/vnl_quaternion.h"
#include "vnl/vnl_cross.h"
#include "itkQuaternionRigidTransform.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include <map>

namespace shapetools
{

template <class T, unsigned int D>
split_segmentations<T,D>::split_segmentations(param::parameterFile &pf)
{
  // Set some parameters.
  double g;
  bool ok = true;
  PARAMSET(pf, g, "background", 0, ok, 1.0);

  m_background = static_cast<T>(g);

  if (ok == false)
    { throw param::Exception("split_segmentations:: missing parameters"); }

}

template <class T, unsigned int D> 
void split_segmentations<T,D>::operator()()
{
//   typename itk::ImageFileReader<image_type>::Pointer reader =
//     itk::ImageFileReader<image_type>::New();
//   typename itk::ImageFileWriter<image_type>::Pointer writer =
//     itk::ImageFileWriter<image_type>::New();
  
  typename std::vector<std::string>::const_iterator it
    = this->input_filenames().begin();
  typename std::vector<std::string>::iterator oit
    = this->output_filenames().begin();
  
  // Loop through the images and pull out the components
  for (; it != this->input_filenames().end(); it++, oit++)
    {
    typename itk::ImageFileReader<image_type>::Pointer reader =
      itk::ImageFileReader<image_type>::New();
    typename itk::ImageFileWriter<image_type>::Pointer writer =
      itk::ImageFileWriter<image_type>::New();

    reader->SetFileName( (*it).c_str() );
    reader->Update();
    //    reader->UpdateLargestPossibleRegion();


    // Determine the various values contained in the image.
    typename itk::ImageRegionIterator<image_type> vit(reader->GetOutput(),
                                                      reader->GetOutput()->GetBufferedRegion());

    // Simultaneously count and record the values in the image.
    std::map<int, int, ltfnc> values;
    for (vit.GoToBegin(); !vit.IsAtEnd(); ++vit)  { values[vit.Get()]++; }

    // Loop through the recorded values in the image and extract each in turn.
    for (typename std::map<int, int, ltfnc>::const_iterator mit = values.begin();
         mit != values.end(); mit++)
      {
      int cval = mit->first;
      //      std::cout << "cval = " << cval << std::endl;
      if (cval == m_background) continue;

      //      if (cval != 3 && cval !=4 )
      //        {        
      //        std::cout << *it << " skipping " << cval << std::endl;
      //        
      //        continue;
      //        }

      // Allocate a new image.
      typename image_type::Pointer newImage = image_type::New();
      newImage->SetRegions( reader->GetOutput()->GetBufferedRegion() );
      //      std::cout << "allocating an image with region " << newImage->GetBufferedRegion() << " = " <<
      //        reader->GetOutput()->GetBufferedRegion() << std::endl;
      newImage->CopyInformation( reader->GetOutput() );
      newImage->Allocate();

      //      std::cout << newImage << std::endl;
      
      typename itk::ImageRegionIterator<image_type> nit(newImage,
                                                        reader->GetOutput()->GetBufferedRegion());
      
      nit.GoToBegin();
      vit.GoToBegin();
      for (; !vit.IsAtEnd(); ++vit, ++nit)
        {
        if (vit.Get() == cval)
          { nit.Set(cval);}
        else nit.Set(m_background);
        }
      
      // modify output filename
      std::string tmpstr = *oit;
      char buf[10];
      //            sprintf(buf, "%d.mha\0", cval);

      sprintf(buf, "%d.nrrd", cval);
      //      sprintf(buf, "%d.nhdr\0", cval);
      tmpstr.append(buf);
      std::cout << "Writing " << tmpstr << std::endl;
      
      //      writer->Modified();
      writer->SetFileName( tmpstr.c_str() );
      writer->SetInput(newImage );
      writer->Write();
      //      writer->UpdateLargestPossibleRegion();
      } // end loop through values

    
    } // end loop through filenames

}

} // end namespace


#endif

