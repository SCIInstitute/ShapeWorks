/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: fastmarching.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_fastmarching_txx
#define __st_fastmarching_txx

#include "fastmarching.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
//#include "itkFastMarchingImageFilter.h"
#include "itkReinitializeLevelSetImageFilter.h"
#include "itkZeroCrossingImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkLevelSetNeighborhoodExtractor.h"

namespace shapetools
{

template <class T, unsigned int D>
fastmarching<T,D>::fastmarching(param::parameterFile &pf)
{
  // Set some parameters.
  bool ok;
  PARAMSET(pf, m_levelset_value, "fastmarching_isovalue", 0, ok, 0.0);
}

template <class T, unsigned int D> 
void fastmarching<T,D>::operator()(typename image_type::Pointer img)
{
   typename itk::ReinitializeLevelSetImageFilter<image_type>::Pointer filt
     = itk::ReinitializeLevelSetImageFilter<image_type>::New();
   filt->SetInput(img);
   filt->NarrowBandingOff();
   filt->SetLevelSetValue(m_levelset_value);
   filt->Update();

// Copy resampled image back to the original image (probably can do this
  // more efficiently --jc).
  itk::ImageRegionIterator<image_type> oit(img, img->GetBufferedRegion());
  itk::ImageRegionIterator<image_type> it(filt->GetOutput(),
                                          img->GetBufferedRegion());
  for ( ; ! it.IsAtEnd(); ++it, ++oit)  { oit.Set(it.Get()); }
   
}
 

   
// template <class T, unsigned int D> 
// void fastmarching<T,D>::operator()(typename image_type::Pointer img)
// {
//   // Copy the input image.
//   typename image_type::Pointer imgCp = image_type::New();
//   imgCp->SetRegions(img->GetBufferedRegion());
//   imgCp->CopyInformation(img);
//   imgCp->Allocate();

//   itk::ImageRegionIterator<image_type> iit(imgCp, imgCp->GetBufferedRegion());
//   itk::ImageRegionIterator<image_type> oit(img, img->GetBufferedRegion());
//   iit.GoToBegin();
//   oit.GoToBegin();
//   for (; ! iit.IsAtEnd(); ++iit, ++oit) { iit.Set(oit.Get()); }
  
//   typedef itk::FastMarchingImageFilter<image_type, image_type> FastMarchingFilterType;

//   // locate the level set
//   typename itk::LevelSetNeighborhoodExtractor<image_type>::Pointer locator =
//     itk::LevelSetNeighborhoodExtractor<image_type>::New();
//   locator->SetInputLevelSet( imgCp );
//   locator->SetLevelSetValue( m_levelset_value );
//   locator->Locate();


//   // Do a further refinement of the locator points.
//   // For all indicies in the active layer...


  
//  //  for (activeIt = m_Layers[0]->Begin();
// //        activeIt != m_Layers[0]->End(); ++activeIt)
// //     {
// //     // Interpolate on the (shifted) input image values at this index to
// //     // assign an active layer value in the output image.
// //     shiftedIt.SetLocation( activeIt->m_Value );

// //     length = m_ValueZero;
// //     for (i = 0; i < ImageDimension; ++i)
// //       {
// //       dx_forward = shiftedIt.GetPixel(center + m_NeighborList.GetStride(i))
// //         - shiftedIt.GetCenterPixel();
// //       dx_backward = shiftedIt.GetCenterPixel()
// //         - shiftedIt.GetPixel(center - m_NeighborList.GetStride(i));

// //       if ( vnl_math_abs(dx_forward) > vnl_math_abs(dx_backward) )
// //         {
// //         length += dx_forward * dx_forward;
// //         }
// //       else
// //         {
// //         length += dx_backward * dx_backward;
// //         }
// //       }
// //     length = vcl_sqrt((double)length) + MIN_NORM;
// //     distance = shiftedIt.GetCenterPixel() / length;

// //     output->SetPixel( activeIt->m_Value , 
// //                       vnl_math_min(vnl_math_max(-CHANGE_FACTOR, distance), CHANGE_FACTOR) );
// //     }


  
//   typename FastMarchingFilterType::Pointer fm = FastMarchingFilterType::New();
//   //  fm->SetInput(img);
//   fm->SetOutputSize( img->GetBufferedRegion().GetSize() );
//   fm->SetOutputOrigin( img->GetOrigin() );
//   fm->SetOutputSpacing( img->GetSpacing() );
//   fm->SetSpeedConstant(img->GetSpacing()[0]); // ASSUMES ISOTROPIC

//   // March outward
//   //fm->SetTrialPoints(seeds);
//    fm->SetTrialPoints( locator->GetOutsidePoints() );
//   fm->Update();

//   itk::ImageRegionIterator<image_type> git(fm->GetOutput(), img->GetBufferedRegion());
//   oit.GoToBegin();
//   git.GoToBegin();
//   iit.GoToBegin();
//   for (; ! git.IsAtEnd(); ++git, ++oit, ++iit)
//     {
//     if ( iit.Get() - m_levelset_value > 0.0)
//       { oit.Set(git.Get()); }
//     }

//   // March inward
//   fm->SetTrialPoints( locator->GetInsidePoints() );
//   fm->Update();

//   oit.GoToBegin();
//   git.GoToBegin();
//   iit.GoToBegin();
//   for (; ! git.IsAtEnd(); ++git, ++oit, ++iit)
//     {
//     if ( iit.Get() - m_levelset_value <= 0.0)
//       { oit.Set( - git.Get()); }
//     }
  

  
// }





} // end namespace

#endif
