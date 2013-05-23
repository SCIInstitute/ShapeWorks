/*=========================================================================
   Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
   Module:    $RCSfile: center.txx,v $
   Date:      $Date: 2011/03/24 01:17:36 $
   Version:   $Revision: 1.3 $
   Author:    $Author: wmartin $

   Copyright (c) 2009 Scientific Computing and Imaging Institute.
   See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
   =========================================================================*/
#ifndef __st_center_txx
#define __st_center_txx

#include "center.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include <cmath>
#include "itkTranslationTransform.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "vnl/vnl_vector.h"
#include "itkExtractImageFilter.h"

namespace shapetools
{

template <class T, unsigned int D>
center<T, D>::center( const char* fname )
{
  TiXmlDocument doc( fname );
  bool loadOkay = doc.LoadFile();

  if ( loadOkay )
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement* elem;

    //m_center_origin = true;
    m_center_origin = false;

    //PARAMSET(pf, g, "background", 0, ok, 1.0);
    this->m_background = 1.0;
    elem = docHandle.FirstChild( "background" ).Element();
    if ( elem )
    {
      this->m_background = static_cast<T>( atof( elem->GetText() ) );
    }
    else
    {
      std::cerr << "center:: missing parameters" << std::endl;
    }
  }
}

template <class T, unsigned int D>
void center<T, D>::operator() ( typename image_type::Pointer img ) {

		 typename image_type::PointType origin = img->GetOrigin();
  std::cerr << "origin = " << img->GetOrigin() << "\n";

  // If center_origin is true, reset the origin of the image to the very center
  // of the image.
  if ( 1 == 0 && m_center_origin )
  {
    double orig[D];
    for ( unsigned int i = 0; i < D; i++ )
    {
      orig[i] = -( static_cast<double>( img->GetLargestPossibleRegion().GetIndex()[i] )
                   + ( static_cast<double>( img->GetLargestPossibleRegion().GetSize()[i] ) / 2.0 ) );
    }

    img->SetOrigin( orig );
  }

  // Copy the original image and find the center of mass.
  typename image_type::Pointer simg = image_type::New();
  simg->CopyInformation( img );
  simg->SetRegions( img->GetLargestPossibleRegion() );
  simg->Allocate();

  itk::ImageRegionIteratorWithIndex<image_type> oit( img, img->GetLargestPossibleRegion() );
  itk::ImageRegionIteratorWithIndex<image_type> sit( simg, img->GetLargestPossibleRegion() );
  sit.GoToBegin();
  oit.GoToBegin();

  itk::Array<double> params( D );
  params.Fill( 0.0 );
  double count = 0.0;
  itk::Point<double, D> point;
  for (; !oit.IsAtEnd(); ++oit, ++sit )
  {
    if ( oit.Get() != m_background )
    {
      sit.Set( oit.Get() );

      // Get the physical index from the image index.
      img->TransformIndexToPhysicalPoint( oit.GetIndex(), point );
      for ( unsigned int i = 0; i < D; i++ )
      {
        params[i] += point[i];
      }
      //std::cerr << "point[2] = " << point[2] << "\n";
      //return;
      count += 1.0;
    }
    else { sit.Set( m_background ); }
  }

  // Compute center of mass.
  for ( unsigned int i = 0; i < D; i++ )
  {
    params[i] = params[i] / count;
  }

  double new_origin[3];
  new_origin[0] = -( img->GetLargestPossibleRegion().GetSize()[0] / 2.0 ) * img->GetSpacing()[0];
  new_origin[1] = -( img->GetLargestPossibleRegion().GetSize()[1] / 2.0 ) * img->GetSpacing()[1];
  new_origin[2] = -( img->GetLargestPossibleRegion().GetSize()[2] / 2.0 ) * img->GetSpacing()[2];

  img->SetOrigin( new_origin );

  std::cerr << "new origin = " << img->GetOrigin() << "\n";

  //simg->SetOrigin( new_origin );

  // Zero out the original image.
  for ( oit.GoToBegin(); !oit.IsAtEnd(); ++oit )
  {
    oit.Set( m_background );
  }

  // Translate the segmentation back into the original image.
  typename itk::TranslationTransform<double, D>::Pointer trans
    = itk::TranslationTransform<double, D>::New();
  trans->SetParameters( params );

  typename itk::NearestNeighborInterpolateImageFunction<image_type, double>::Pointer
    interp = itk::NearestNeighborInterpolateImageFunction<image_type, double>::New();

  typename itk::ResampleImageFilter<image_type, image_type>::Pointer resampler
    = itk::ResampleImageFilter<image_type, image_type>::New();
  resampler->SetOutputParametersFromImage( img );
  resampler->SetTransform( trans );
  resampler->SetInterpolator( interp );
  resampler->SetInput( simg );
  resampler->Update();

  new_origin[0] = origin[0] + new_origin[0] + (params[0] - origin[0]);
  new_origin[1] = origin[1] + new_origin[1] + (params[1] - origin[1]);
  new_origin[2] = origin[2] + new_origin[2] + (params[2] - origin[2]);

  // set the origin back so that the image will line up with the original one
  //new_origin[0] = new_origin[0] + params[0];
  //new_origin[1] = new_origin[1] + params[1];
  //new_origin[2] = new_origin[2] + params[2];

  img->SetOrigin( new_origin );

  std::cerr << "new new origin = " << img->GetOrigin() << "\n";

  // Copy resampled image back to the original image (probably can do this
  // more efficiently --jc).
  oit.GoToBegin();
  itk::ImageRegionIterator<image_type> it( resampler->GetOutput(), img->GetLargestPossibleRegion() );
  for (; !it.IsAtEnd(); ++it, ++oit )
  {
    oit.Set( it.Get() );
  }

  this->m_transform.set_identity();
  for ( unsigned int i = 0; i < D; i++ )
  {
    this->m_transform[i][D] = params[i];
  }
}
} // end namespace

#endif /* ifndef __st_center_txx */
