/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: bounding_box.txx,v $

  Date:      $Date: 2011/03/24 01:17:36 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __st_bounding_box_txx

#define __st_bounding_box_txx



#include "bounding_box.h"

#include "itkImageRegionIterator.h"



namespace shapetools

{



template <class T, unsigned int D>

bounding_box<T,D>::bounding_box(param::parameterFile &pf)

{

  // Set some parameters.

  bool ok = true;

  PARAMSET(pf, m_background, "background", 0, ok, static_cast<T>(1));

  

  if (ok == false)

    {  throw param::Exception("bounding_box:: missing parameters"); }

}



template <class T, unsigned int D> 

void bounding_box<T,D>::operator()(typename image_type::Pointer img)

{

  // Initialize bounding box

  itk::ImageRegionIteratorWithIndex<image_type> it(img, img->GetBufferedRegion());



  typename region_type::SizeType  imgSize = img->GetBufferedRegion().GetSize();

  typename region_type::IndexType imgCorner = img->GetBufferedRegion().GetIndex();

  

  // Search for bounding box.

  typename image_type::IndexType idx;

  typename image_type::IndexType lower = imgCorner + imgSize;

  typename image_type::IndexType upper = imgCorner;



  for (it.GoToBegin(); ! it.IsAtEnd(); ++it)

    {

    if (it.Get() != m_background)

      {

      idx = it.GetIndex();

      

      for (unsigned int i = 0; i < D; i++)

        {

        if (lower[i] > idx[i]) lower[i] = idx[i];

        if (upper[i] < idx[i]) upper[i] = idx[i];

        }

      }

    }



  typename region_type::SizeType size;

  for (unsigned int i = 0; i <D; i++) size[i] = upper[i] - lower[i];

  m_region.SetSize(size);

  m_region.SetIndex(lower);

}



} // end namespace



#endif

