/*=========================================================================
   Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
   Module:    $RCSfile: isotropic.txx,v $
   Date:      $Date: 2011/03/24 01:17:36 $
   Version:   $Revision: 1.2 $
   Author:    $Author: wmartin $

   Copyright (c) 2009 Scientific Computing and Imaging Institute.
   See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
   =========================================================================*/
#ifndef __st__isotropic_txx
#define __st__isotropic_txx

#include "itkExtractImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "bounding_box.h"

namespace shapetools
{

template <class T, unsigned int D>
isotropic<T, D>::isotropic( const char* fname )
{
  TiXmlDocument doc( fname );
  bool loadOkay = doc.LoadFile();

  if ( loadOkay )
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement* elem;
  }
}

template <class T, unsigned int D>
void isotropic<T, D>::operator() () {
  typename std::vector<std::string>::const_iterator it
    = this->input_filenames().begin();
  typename std::vector<std::string>::const_iterator oit
    = this->output_filenames().begin();


  // first find the minimim spacing in all the images
  double min_spacing = 1.0;
  it = this->input_filenames().begin();
  oit = this->output_filenames().begin();
  for (; it != this->input_filenames().end(); it++, oit++ )
  {
    typename itk::ImageFileReader<image_type>::Pointer reader =
      itk::ImageFileReader<image_type>::New();
    reader->SetFileName( ( *it ).c_str() );
    reader->UpdateLargestPossibleRegion();
    image_type::Pointer img = reader->GetOutput();
    const typename image_type::SpacingType& input_spacing = img->GetSpacing();
    for ( unsigned int i = 0; i < D; i++ )
    {
      min_spacing = std::min( min_spacing, input_spacing[i] );
    }
  }

  std::cout << "Minimum Spacing: " << min_spacing << "\n";

  it = this->input_filenames().begin();
  oit = this->output_filenames().begin();
  for (; it != this->input_filenames().end(); it++, oit++ )
  {
    typename itk::ImageFileReader<image_type>::Pointer reader =
      itk::ImageFileReader<image_type>::New();

    reader->SetFileName( ( *it ).c_str() );
    reader->UpdateLargestPossibleRegion();
    std::cout << "resample to isotropic: " << *it << std::endl;

    image_type::Pointer img = reader->GetOutput();

    const typename image_type::RegionType& inputRegion = img->GetLargestPossibleRegion();
    const typename image_type::SizeType& inputSize = inputRegion.GetSize();
    const typename image_type::SpacingType& input_spacing = img->GetSpacing();

    typename image_type::SpacingType output_spacing;
    output_spacing[0] = min_spacing;
    output_spacing[1] = min_spacing;
    output_spacing[2] = min_spacing;

    typename image_type::SizeType output_size;
    typedef typename image_type::SizeType::SizeValueType SizeValueType;
    output_size[0] = static_cast<SizeValueType>( inputSize[0] * input_spacing[0] / output_spacing[0] + .5 );
    output_size[1] = static_cast<SizeValueType>( inputSize[1] * input_spacing[1] / output_spacing[1] + .5 );
    output_size[2] = static_cast<SizeValueType>( inputSize[2] * input_spacing[2] / output_spacing[2] + .5 );

    typedef itk::NearestNeighborInterpolateImageFunction<image_type> NearestNeighborInterpolatorType;
    typename NearestNeighborInterpolatorType::Pointer nearestNeighborInterpolator = NearestNeighborInterpolatorType::New();

    typename itk::ResampleImageFilter<image_type, image_type>::Pointer resampler
      = itk::ResampleImageFilter<image_type, image_type>::New();

    typedef itk::IdentityTransform< double, 3 > TransformType;
    typename TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    resampler->SetInput( img );
    resampler->SetTransform( transform );
    resampler->SetInterpolator( nearestNeighborInterpolator );

    resampler->SetOutputOrigin( img->GetOrigin() );
    resampler->SetOutputSpacing( output_spacing );
    resampler->SetOutputDirection( img->GetDirection() );
    resampler->SetSize( output_size );

    resampler->Update();

    typename itk::ImageFileWriter<image_type>::Pointer writer =
      itk::ImageFileWriter<image_type>::New();
    writer->SetFileName( ( *oit ).c_str() );
    writer->SetInput( resampler->GetOutput() );
    writer->Update();
  }
}
} // end namespace

#endif /* ifndef __st__isotropic_txx */
