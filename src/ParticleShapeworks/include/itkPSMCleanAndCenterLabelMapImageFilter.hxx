/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMCleanAndCenterLabelMapImageFilter_hxx
#define __itkPSMCleanAndCenterLabelMapImageFilter_hxx

#include "itkPSMCleanAndCenterLabelMapImageFilter.h"
#include "itkProgressReporter.h"
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkResampleImageFilter.h"
#include "itkTranslationTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkExtractImageFilter.h"
#include "itkArray.h"
#include <map>

//#include "itkImageFileWriter.h"

namespace itk
{
template< class TImage >
PSMCleanAndCenterLabelMapImageFilter< TImage >
::PSMCleanAndCenterLabelMapImageFilter()
{
  m_ForegroundValue = NumericTraits< PixelType >::One;
  m_BackgroundValue = NumericTraits< PixelType >::Zero;
}

template< class TImage >
void
PSMCleanAndCenterLabelMapImageFilter< TImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "m_ForegroundValue = " << m_ForegroundValue << std::endl;
  os << indent << "m_BackgroundValue = " << m_BackgroundValue << std::endl;
}


template< class TImage>
void
PSMCleanAndCenterLabelMapImageFilter< TImage>
::GenerateInputRequestedRegion()
{
  // We need the whole image!
  // Get pointers to the input and output.
  typename Superclass::InputImagePointer inputPtr =
    const_cast< TImage * >( this->GetInput() );
  typename Superclass::OutputImagePointer outputPtr 
    = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  inputPtr->SetRequestedRegion( inputPtr->GetLargestPossibleRegion());
}

template< class TImage >
void
PSMCleanAndCenterLabelMapImageFilter< TImage >
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();
}


template< class TImage >
void 
PSMCleanAndCenterLabelMapImageFilter<TImage>::GenerateData()
{
  // Allocate work image.  Same size as input.  Later we will crop it.
  typename ImageType::Pointer work = ImageType::New();
  work->CopyInformation(this->GetInput());
  work->SetRegions(this->GetInput()->GetRequestedRegion());
  work->Allocate();

  // Isolate largest connected component into the work image.
  // Modifies the work image.
  this->IsolateLargestComponent(work);

  // We are done with inputs.  Release them if requested.
  this->ReleaseInputs();

  // Fill holes in the working image
  this->FillHoles(work);

  // Resample the image to isotropic volumes, in case it is not already.
  this->ResampleToIsotropic(work);
  
  // Reset the origin to the center of the image and translate the
  // center-of-mass of the segmentation to the origin.
  this->Center(work);



}

template < class TImage>
void
PSMCleanAndCenterLabelMapImageFilter<TImage>::IsolateLargestComponent(ImageType *work) const
{
  // Create a connected component filter
  typename itk::ConnectedComponentImageFilter<TImage, TImage>::Pointer ccfilter 
    = itk::ConnectedComponentImageFilter<TImage, TImage>::New();
  ccfilter->SetInput(this->GetInput());
  ccfilter->FullyConnectedOn();
  ccfilter->Update();
  
  // First find the size of the connected components.
  std::map<int, int> sizes;
  ImageRegionConstIterator<TImage> rit(ccfilter->GetOutput(),
                                       ccfilter->GetOutput()->GetBufferedRegion());
  for (; !rit.IsAtEnd(); ++rit)
    {
      if (rit.Get() > 0)
        {
          sizes[rit.Get()] = sizes[rit.Get()] + 1;
        }
    }
  
  // Find largest connected component. Assumes connected component
  // algorithm enumerates sequentially. (This is true for the ITK
  // algorithm). Copy largest connected component into work image.
  int maxsize = 0;
  int bigcomponent = 0;
  for (typename std::map<int, int>::const_iterator mapit = sizes.begin();
       mapit != sizes.end(); mapit++)
    {
      if ((*mapit).second > maxsize)
        {
          bigcomponent = (*mapit).first;
          maxsize = (*mapit).second;
        }
    }
  
  // Copy result into work image. Remove all but the largest connected component.
  ImageRegionIterator<TImage> it(work, work->GetBufferedRegion());
  rit.GoToBegin();
  it.GoToBegin();
  for ( ; ! rit.IsAtEnd(); ++it, ++rit)
    {
      if (rit.Get() == bigcomponent) it.Set(m_ForegroundValue);
      else it.Set(m_BackgroundValue);
    }  
}

template < class TImage>
void
PSMCleanAndCenterLabelMapImageFilter<TImage>::FillHoles(ImageType *work) const
{

  // TODO: Need a check to see if the UL corner is background value.
  IndexType idx;
  idx.Fill(0);

  if (work->GetPixel(idx) == m_ForegroundValue)
    {
    itkWarningMacro("Could not fill holes. Upper-left hand corner is not in the background. See documentation for this filter for more information.")
    }
  
  // Find a background value seed at the boundary. Assumption is that
  // the corner index is background.
  typename ConnectedThresholdImageFilter<ImageType, ImageType>::IndexType seed;
  seed.Fill(NumericTraits<PixelType>::Zero);
  
  // Flood fill the background with the foreground value.
  typename itk::ConnectedThresholdImageFilter<ImageType, ImageType >::Pointer
    ccfilter = itk::ConnectedThresholdImageFilter<ImageType, ImageType>::New();
  ccfilter->SetInput(work);
  ccfilter->SetLower(m_BackgroundValue);
  ccfilter->SetUpper(m_BackgroundValue);
  ccfilter->SetSeed(seed);
  ccfilter->SetReplaceValue(m_ForegroundValue);
  ccfilter->Update();

  // Logical "OR" the original image with the inverse (foreground/background flipped)
  // of the flood-filled copy.
  itk::ImageRegionConstIterator<ImageType> rit(ccfilter->GetOutput(),
                                               work->GetBufferedRegion());
  itk::ImageRegionIterator<ImageType> it(work,work->GetBufferedRegion());
  for (; !rit.IsAtEnd(); ++rit, ++it)
    {
    if (rit.Get() == m_BackgroundValue)
      {
      it.Set(m_ForegroundValue);
      }
    }
}

template < class TImage>
void
PSMCleanAndCenterLabelMapImageFilter<TImage>::Center(ImageType *work) 
{
  // Reset the physical coordinates at the center of the image to (0,0,0)
  PointType lower;
  PointType upper;
  PointType ul_physical_coords;
  work->TransformIndexToPhysicalPoint(work->GetBufferedRegion().GetIndex(),
                                      lower);
  work->TransformIndexToPhysicalPoint(work->GetBufferedRegion().GetIndex() 
                                      + work->GetBufferedRegion().GetSize(), upper);
  for (unsigned int i = 0; i < Dimension; i++)
    {
    ul_physical_coords[i] = -(lower[i] + upper[i]) / 2.0;
    }
  
  // IndexType idx;
  // work->TransformPhysicalPointToIndex(ul_physical_coords,idx);
  
  // It is confusing, but the following command sets the physical coordinates
  // of pixel number 0,0,0... (i.e. the upper-left corner of the volume).
  work->SetOrigin(ul_physical_coords);
  
 // Find the center of mass.
  ImageRegionIteratorWithIndex<ImageType> oit(work, work->GetBufferedRegion());
  oit.GoToBegin();
  
  Array<double> params(Dimension);
  params.Fill(0.0);
  double count = 0.0;
  Point<double, Dimension> point;
  for (; ! oit.IsAtEnd(); ++oit)
    {
    if (oit.Get() != m_BackgroundValue)
      {
      // Get the physical index from the image index.
      work->TransformIndexToPhysicalPoint(oit.GetIndex(), point);
      for (unsigned int i = 0; i < Dimension; i++) { params[i] += point[i]; }
      count += 1.0;
      }
    }
  
  // Compute center of mass.
  for (unsigned int i = 0; i < Dimension; i++)
    {
    params[i] = params[i] / count;
    }
  
  // Translate the segmentation into the output
  typename TranslationTransform<double,Dimension>::Pointer trans
    = TranslationTransform<double,Dimension>::New();
  trans->SetParameters(params);
  
  typename NearestNeighborInterpolateImageFunction<ImageType,double>::Pointer
    interp = NearestNeighborInterpolateImageFunction<ImageType,double>::New();
  
  typename ResampleImageFilter<ImageType, ImageType>::Pointer resampler
    = ResampleImageFilter<ImageType, ImageType>::New();
  resampler->SetOutputParametersFromImage(work);
  resampler->SetTransform(trans);
  resampler->SetInterpolator(interp);
  resampler->SetInput(work);

 // the size.
  this->GetOutput()->SetRegions(this->GetInput()->GetLargestPossibleRegion());
  this->GetOutput()->Allocate();

  // pipe to the output image
  resampler->GraftOutput(this->GetOutput());
  resampler->Update();

  // also necessary for pipe to output
  // see itkImageSource for description
  this->GraftOutput(resampler->GetOutput());

  // Make sure the image information is correct.
  //  itk::Matrix<double,3,3> I;
  //  I.SetIdentity();
  // this->GetOutput()->SetDirection(I);
}

} // end namespace itk

#endif
