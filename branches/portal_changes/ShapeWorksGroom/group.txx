/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: group.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_group_txx
#define __st_group_txx

#include "group.h"
#include "itkImageRegionIterator.h"

namespace shapetools
{

template <class T, unsigned int D>
group<T,D>::group(const char *fname)
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
      std::cerr << "group:: missing parameters" << std::endl;
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
      std::cerr << "group:: missing parameters" << std::endl;
    }
  }
}


template <class T, unsigned int D> 
void group<T,D>::operator()(typename image_type::Pointer img)
{

  // Replace all labels (not background) with foreground label.
  itk::ImageRegionIterator<image_type> it(img, img->GetBufferedRegion());
  it.GoToBegin();
  for ( ; ! it.IsAtEnd(); ++it)
    {
    if (it.Get() != m_background) it.Set(m_foreground);
    else it.Set(m_background);
    }
}

} // end namespace

#endif
