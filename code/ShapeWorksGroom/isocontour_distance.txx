/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: isocontour_distance.txx,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_isocontour_distance_txx
#define __st_isocontour_distance_txx

#include "isocontour_distance.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkIsocontour_DistanceImageFilter.h"
#include "itkZeroCrossingImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkLevelSetNeighborhoodExtractor.h"

namespace shapetools
{

template <class T, unsigned int D>
isocontour_distance<T,D>::isocontour_distance(param::parameterFile &pf)
{
  // Set some parameters.
  bool ok;
  PARAMSET(pf, m_levelset_value, "isocontour_distance_isovalue", 0, ok, 0.0);
}

template <class T, unsigned int D> 
void isocontour_distance<T,D>::operator()(typename image_type::Pointer img)
{
  // Copy the input image.
  typename image_type::Pointer imgCp = image_type::New();
  imgCp->SetRegions(img->GetBufferedRegion());
  imgCp->CopyInformation(img);
  imgCp->Allocate();

  itk::ImageRegionIterator<image_type> iit(imgCp, imgCp->GetBufferedRegion());
  itk::ImageRegionIterator<image_type> oit(img, img->GetBufferedRegion());
  iit.GoToBegin();
  oit.GoToBegin();
  for (; ! iit.IsAtEnd(); ++iit, ++oit) { iit.Set(oit.Get()); }
  
  typedef itk::Isocontour_DistanceImageFilter<image_type, image_type> Isocontour_DistanceFilterType;

  // locate the level set
  typename itk::LevelSetNeighborhoodExtractor<image_type>::Pointer locator =
    itk::LevelSetNeighborhoodExtractor<image_type>::New();
  locator->SetInputLevelSet( imgCp );
  locator->SetLevelSetValue( m_levelset_value );
  locator->Locate();
  
  typename Isocontour_DistanceFilterType::Pointer fm = Isocontour_DistanceFilterType::New();
  //  fm->SetInput(img);
  fm->SetOutputSize( img->GetBufferedRegion().GetSize() );
  fm->SetOutputOrigin( img->GetOrigin() );
  fm->SetOutputSpacing( img->GetSpacing() );
  fm->SetSpeedConstant(img->GetSpacing()[0]); // ASSUMES ISOTROPIC

  // March outward
  //  fm->SetTrialPoints(seeds);
  fm->SetTrialPoints( locator->GetOutsidePoints() );
  fm->Update();

  itk::ImageRegionIterator<image_type> git(fm->GetOutput(), img->GetBufferedRegion());
  oit.GoToBegin();
  git.GoToBegin();
  iit.GoToBegin();
  for (; ! git.IsAtEnd(); ++git, ++oit, ++iit)
    {
    if ( iit.Get() - m_levelset_value > 0.0)
      { oit.Set(git.Get()); }
    }

  // March inward
  fm->SetTrialPoints( locator->GetInsidePoints() );
  fm->Update();

  oit.GoToBegin();
  git.GoToBegin();
  iit.GoToBegin();
  for (; ! git.IsAtEnd(); ++git, ++oit, ++iit)
    {
    if ( iit.Get() - m_levelset_value <= 0.0)
      { oit.Set( - git.Get()); }
    }
  

  
}

} // end namespace

#endif
