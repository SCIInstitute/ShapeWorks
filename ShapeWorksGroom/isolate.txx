/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: isolate.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_isolate_txx
#define __st_isolate_txx

#include "isolate.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkImageRegionIterator.h"
#include <map>

namespace shapetools
{

template <class T, unsigned int D>
isolate<T,D>::isolate(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    //PARAMSET(pf, f, "foreground", 0, ok, 0.0);
    this->m_foreground = 0.0;
    elem = docHandle.FirstChild( "foreground" ).Element();
    if (elem)
    {
      this->m_foreground = static_cast<T>(atof(elem->GetText()));
    }
    else
    {
      std::cerr << "isolate:: missing parameters" << std::endl;
    }

    //PARAMSET(pf, g, "background", 0, ok, 1.0);
    this->m_background = 1.0;
    elem = docHandle.FirstChild( "background" ).Element();
    if (elem)
    {
      this->m_background = static_cast<T>(atof(elem->GetText()));
    }
    else
    {
      std::cerr << "isolate:: missing parameters" << std::endl;
    }
  }
}


template <class T, unsigned int D> 
void isolate<T,D>::operator()(typename image_type::Pointer img)
{
  // Find the connected components in this image.
  typename itk::ConnectedComponentImageFilter<image_type, image_type>::Pointer
    ccfilter = itk::ConnectedComponentImageFilter<image_type, image_type>::New();
  ccfilter->SetInput(img);
  ccfilter->FullyConnectedOn();
  ccfilter->Update();

  // First find the size of the connected components.
  std::map<int, int> sizes;
  //  std::vector<int> sizes;
  itk::ImageRegionConstIterator<image_type> rit(ccfilter->GetOutput(),
                                                img->GetBufferedRegion());
  for (; !rit.IsAtEnd(); ++rit)
    {
    if (rit.Get() > 0)
      {
      sizes[rit.Get()] = sizes[rit.Get()] + 1;
      }
    }

  // Find largest connected component. Assumes connected component algorithm
  // enumerates sequentially. (This is true for the ITK algorithm).
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
  
  // Copy result into return image. Remove all but the largest connected component.
  itk::ImageRegionIterator<image_type> it(img, img->GetBufferedRegion());
  rit.GoToBegin();
  it.GoToBegin();
  for ( ; ! rit.IsAtEnd(); ++it, ++rit)
    {
    if (rit.Get() == bigcomponent) it.Set(m_foreground);
    else it.Set(m_background);
    }
}

} // end namespace

#endif
