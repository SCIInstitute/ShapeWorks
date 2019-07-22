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
#ifndef __itkPSMTrimLabelMapImageFilter_hxx
#define __itkPSMTrimLabelMapImageFilter_hxx

#include "itkPSMTrimLabelMapImageFilter.h"
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
PSMTrimLabelMapImageFilter< TImage >
::PSMTrimLabelMapImageFilter()
{
  m_ForegroundValue = NumericTraits< PixelType >::One;
  m_BackgroundValue = NumericTraits< PixelType >::Zero;
}

template< class TImage >
void
PSMTrimLabelMapImageFilter< TImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "m_ForegroundValue = " << m_ForegroundValue << std::endl;
  os << indent << "m_BackgroundValue = " << m_BackgroundValue << std::endl;
}


template< class TImage>
void
PSMTrimLabelMapImageFilter< TImage>
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
PSMTrimLabelMapImageFilter< TImage >
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();
}


template< class TImage >
void 
PSMTrimLabelMapImageFilter<TImage>::GenerateData()
{
  // Allocate work image.  Same size as input.  Later we will crop it.
  typename ImageType::Pointer work = ImageType::New();
  work->CopyInformation(this->GetInput());
  work->SetRegions(this->GetInput()->GetRequestedRegion());
  work->Allocate();

  //  std::cout << "A work regions = " << work->GetBufferedRegion() << std::endl;
  //  std::cout << "A input regions = " << this->GetInput()->GetBufferedRegion() << std::endl;

  // Isolate largest connected component into the work image.
  // Modifies the work image.
  this->IsolateLargestComponent(work);

  // We are done with inputs.  Release them if requested.
  this->ReleaseInputs();

  // Fill holes in the working image
  this->FillHoles(work);

  // Reset the origin to the center of the image and translate the
  // center-of-mass of the segmentation to the origin.
  this->Center(work);

  // typename ImageFileWriter<ImageType>::Pointer writer = ImageFileWriter<ImageType>::New();
  // writer->SetInput(work);
  // writer->SetFileName("tmp.nrrd");
  // writer->Update();

  // Finally, crop the image.  This method allocates the output image
  // and grafts the final result to the output image.
  this->Crop(work);

  //  std::cout << "E work regions = " << work->GetBufferedRegion() << std::endl;

  // Finally, make sure all the image information is correct: origin
  // is in the CENTER of the image and the proper spacing information
  // has been maintained.
  


}

template < class TImage>
void
PSMTrimLabelMapImageFilter<TImage>::IsolateLargestComponent(ImageType *work) const
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
PSMTrimLabelMapImageFilter<TImage>::FillHoles(ImageType *work) const
{
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

  // "or" the original image with the inverse (foreground/background flipped)
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
PSMTrimLabelMapImageFilter<TImage>::Center(ImageType *work) const
{
  // // // Reset the origin of the image to the very center of the image.
  // // //  double orig[Dimension];
   PointType lower;
   PointType upper;
   PointType center;
   work->TransformIndexToPhysicalPoint(work->GetBufferedRegion().GetIndex(),
                                       lower);
   work->TransformIndexToPhysicalPoint(work->GetBufferedRegion().GetIndex() 
                                       + work->GetBufferedRegion().GetSize(), upper);
   for (unsigned int i = 0; i < Dimension; i++)
     {
       center[i] = (lower[i] + upper[i]) / 2.0;
     }

   IndexType idx;
   work->TransformPhysicalPointToIndex(center,idx);

   work->SetPixel(idx,4.0);
  // work->SetOrigin(center);
  
  // Copy the original image and find the center of mass.
  typename ImageType::Pointer simg = ImageType::New();
  simg->CopyInformation(work);
  simg->SetRegions(work->GetBufferedRegion());
  simg->Allocate();
  
  ImageRegionIteratorWithIndex<ImageType> oit(work, work->GetBufferedRegion());
  ImageRegionIteratorWithIndex<ImageType> sit(simg, work->GetBufferedRegion());
  sit.GoToBegin();
  oit.GoToBegin();

  Array<double> params(Dimension);
  params.Fill(0.0);
  double count = 0.0;
  Point<double, Dimension> point;
  for (; ! oit.IsAtEnd(); ++oit, ++sit)
    {
    if (oit.Get() != m_BackgroundValue)
      {
      sit.Set(oit.Get());
      
      // Get the physical index from the image index.
      work->TransformIndexToPhysicalPoint(oit.GetIndex(), point);
      for (unsigned int i = 0; i < Dimension; i++) { params[i] += point[i]; }
      count += 1.0;
      }
    else { sit.Set(m_BackgroundValue); }
    }
  
  // Compute center of mass.
  for (unsigned int i = 0; i < Dimension; i++)
    {
    params[i] = params[i] / count;
    // std::cout << "params " << i << " = " << params[i] <<std::endl;
    }

  // PointType pt;
  // for (unsigned i = 0; i < Dimension; i++) { pt[i] = params[i];}
  // work->TransformPhysicalPointToIndex(pt,idx);
  // work->SetPixel(idx,8.0);

  //  std::cout << "Center of mass = " << pt << std::endl;
  // std::cout << "idx = " << idx << std::endl;
  // return;
  // Zero out the original image.
  for (oit.GoToBegin(); ! oit.IsAtEnd(); ++oit)  { oit.Set(m_BackgroundValue); }

  // Translate the segmentation back into the original image.
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
  resampler->SetInput(simg);

  resampler->Update();

  // Copy resampled image back to the original image 
  oit.GoToBegin();
  ImageRegionIterator<ImageType> 
    it(resampler->GetOutput(), work->GetBufferedRegion());
  for ( ; ! it.IsAtEnd(); ++it, ++oit)  
    { 
      oit.Set(it.Get()); 
    }
}

template < class TImage>
void
PSMTrimLabelMapImageFilter<TImage>::Crop(ImageType *work)
{
  // Find the bounding box containing the foreground
  ImageRegionIteratorWithIndex<ImageType> 
    it(work, work->GetBufferedRegion());

  SizeType  workSize   = work->GetBufferedRegion().GetSize();
  IndexType workCorner = work->GetBufferedRegion().GetIndex();
  
  // Search for bounding box.
  IndexType idx;
  IndexType lower = workCorner + workSize;
  IndexType upper = workCorner;

  for (it.GoToBegin(); ! it.IsAtEnd(); ++it)
    {
    if (it.Get() != m_BackgroundValue)
      {
      idx = it.GetIndex();
      
      for (unsigned int i = 0; i < Dimension; i++)
        {
        if (lower[i] > idx[i]) lower[i] = idx[i];
        if (upper[i] < idx[i]) upper[i] = idx[i];
        }
      }
    }

  SizeType size;
  for (unsigned int i = 0; i < Dimension; i++) 
    {
      size[i] = upper[i] - lower[i];
    }

  // Create bounding box region
  m_BoundingBox.SetSize(size);
  m_BoundingBox.SetIndex(lower);
  
  // Create cropping filter
  typename itk::ExtractImageFilter<ImageType, ImageType>::Pointer extractor
    = itk::ExtractImageFilter<ImageType, ImageType>::New();
  extractor->SetExtractionRegion(m_BoundingBox);
  extractor->SetInput(work);

  // We are finally able to allocate an output image now that we know
  // the size.
  this->GetOutput()->SetRegions(m_BoundingBox);
  this->GetOutput()->Allocate();

  // pipe to the output image
  extractor->GraftOutput(this->GetOutput());
  extractor->Update();
  // also necessary for pipe to output
  // see itkImageSource for description
  this->GraftOutput(extractor->GetOutput());

  // Make sure the image information is correct.
  // itk::Matrix<double,3,3> I;
  // I.SetIdentity();
  // extractor->GetOutput()->SetDirection(I);
  // double ss[3];
  // ss[0] = ss[1] = ss[2] = 1.0;

  // Origin at the center of the image
  // float o[3];
  // o[0] = - (extractor->GetOutput()->GetBufferedRegion().GetSize()[0] / 2.0);
  // o[1] = - (extractor->GetOutput()->GetBufferedRegion().GetSize()[1] / 2.0);
  // o[2] = - (extractor->GetOutput()->GetBufferedRegion().GetSize()[2] / 2.0);

  // std::cout << "---" << o[0] << " " << o[1] << " " << o[2] << std::endl;
 
  // extractor->GetOutput()->SetOrigin(o);
  // extractor->GetOutput()->SetSpacing(ss);
}
  
  
  
  
  
  
  
// Allocate output
  //      this->GetOutput(i)->SetRegions(this->GetInput(i)->GetRequestedRegion());
  //      std::cout << "Allocating " << i << std::endl;
  //   this->GetOutput(i)->Allocate();
  
      //      std::cout << "Done Allocating " << i << std::endl;
      
      // Find the connected components in this image. Setup the
      // mini-pipeline to process the ith input to this filter.
 
      
      // Setup the mini-pipeline to calculate the correct regions and
      // write to the appropriate bulk data block.
      //      ccfilter->GraftOutput(this->GetOutput(i));

      // Graft the mini-pipeline output back onto this filter's
      // output. This is needed to get the appropriate regions passed
      // back.
      //      this->GraftOutput(ccfilter->GetOutput(i));
      
      // First find the size of the connected components.
      // std::map<int, int> sizes;

      // ImageRegionConstIterator<TImage> rit(ccfilter->GetOutput(),
      //                                           work->GetBufferedRegion());
      // for (; !rit.IsAtEnd(); ++rit)
      //   {
      //     if (rit.Get() > 0)
      //       {
      //         sizes[rit.Get()] = sizes[rit.Get()] + 1;
      //       }
      //   }
      
      // // Find largest connected component. Assumes connected component algorithm
      // // enumerates sequentially. (This is true for the ITK algorithm).
      // int maxsize = 0;
      // int bigcomponent = 0;
      // for (typename std::map<int, int>::const_iterator mapit = sizes.begin();
      //      mapit != sizes.end(); mapit++)
      //   {
      //     if ((*mapit).second > maxsize)
      //       {
      //         bigcomponent = (*mapit).first;
      //         maxsize = (*mapit).second;
      //       }
      //   }
      
      // // Copy result into return image. Remove all but the largest connected component.
      // ImageRegionIterator<TImage> it(work, img->GetBufferedRegion());
      // rit.GoToBegin();
      // it.GoToBegin();
      // for ( ; ! rit.IsAtEnd(); ++it, ++rit)
      //   {
      //     if (rit.Get() == bigcomponent) it.Set(m_foreground);
      //     else it.Set(m_BackgroundValue);
      //   }


// template< class TImage >
// void PSMTrimLabelMapImageFilter< TImage >::GenerateInputRequestedRegion()
// {
//   // call the superclass' implementation of this method
//   Superclass::GenerateInputRequestedRegion();

//   // get pointers to the input and output
//   typename ImageType::Pointer inputPtr  = this->GetInputImage();
//   typename ImageType::Pointer outputPtr = this->GetOutputImage();

//   if ( !inputPtr || !outputPtr )
//     {
//     return;
//     }

//   //
//   // FOR NOW WE'LL JUST SET THE INPUT REGION TO THE OUTPUT REGION
//   // AND OVERRIDE THIS LATER
//   //
//   inputPtr->SetRequestedRegion( outputPtr->GetRequestedRegion() );
// }

} // end namespace itk

#endif
