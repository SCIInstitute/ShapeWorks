/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: simple_morphometrics.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__simple_morphometrics_txx
#define __st__simple_morphometrics_txx

#include "object_reader.h"

namespace shapetools
{

template <class T, unsigned int D>
simple_morphometrics<T,D>::simple_morphometrics(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
 
    //PARAMSET(pf, g, "foreground", 0, ok, 1.0);
    this->m_foreground = 1.0;
    elem = docHandle.FirstChild( "foreground" ).Element();
    if (elem)
    {
      this->m_foreground = static_cast<T>(atof(elem->GetText()));
    }
    else
    {
      std::cerr << "auto_pad:: missing parameters" << std::endl;
    }  
  }
}


template <class T, unsigned int D> 
void simple_morphometrics<T,D>::operator()()
{
  if (D != 3)
  {
    std::cerr << "simple_morphometrics: this tool is only implemented for 3D" << std::endl;
    throw 1;
  }

  typename itk::ImageFileReader<image_type>::Pointer reader =
    itk::ImageFileReader<image_type>::New();

  typename std::vector<std::string>::const_iterator it
    = this->input_filenames().begin();

  // Print the header for the .csv
  std::cout << "filename,xwidth,ywidth,zwidth,volume,spacingX,spacingY,spacingZ,voxelvolume" << std::endl;

  // Loop through the images
  for (; it != this->input_filenames().end(); it++)
    {    
    reader->SetFileName( (*it).c_str() );
    reader->Update();
    std::cout << *it << ",";

    //    typename image_type::Pointer testimg = image_type::New();
    //    testimg->SetRegions(reader->GetOutput()->GetRequestedRegion());
    //    testimg->CopyInformation(reader->GetOutput());
    //    testimg->Allocate();
    
    vnl_vector_fixed<double, 3> spacing;
    vnl_vector_fixed<double, 3> offset;
    vnl_vector_fixed<int   , 3> size;
    //    vnl_vector_fixed<double, 3> origin;    
    for (unsigned int i = 0; i < 3; i++)
      {
      spacing[i] = reader->GetOutput()->GetSpacing()[i];
      offset[i] = (double) (reader->GetOutput()->GetRequestedRegion().GetIndex()[i]);
      size[i] = reader->GetOutput()->GetRequestedRegion().GetSize()[i];
      //      origin[i] = (double) (reader->GetOutput()->GetOrigin()[i]);
      }

//     std::cout << "spacing = " << spacing << std::endl;
//     std::cout << "offset = " << offset << std::endl;
//     std::cout << "origin = " << origin << std::endl;
    
    // Check x direction (also volume)
    const float epsilon = 0.7;
    double vol = 0.0;
    double maxcount = 0.0;
    typename image_type::IndexType idx;
    for (idx[2] = 0; idx[2] < size[2]; idx[2] = idx[2]+1)
      for (idx[1] = 0; idx[1] < size[1]; idx[1] = idx[1]+1)
        {
        double count = 0.0;
        for (idx[0] = 0; idx[0] < size[0]; idx[0] = idx[0]+1)
          {
          if (reader->GetOutput()->GetPixel(idx) > m_foreground - epsilon
              && reader->GetOutput()->GetPixel(idx) < m_foreground + epsilon)
            {
            vol   = vol   + 1.0;
            count = count + 1.0;
            }
          }
        if (count > maxcount) maxcount = count;
        }


    
    std::cout << maxcount << ",";

    // check y
    maxcount = 0.0;    
    for (idx[2] = 0; idx[2] < size[2]; idx[2] = idx[2]+1)
      for (idx[0] = 0; idx[0] < size[0]; idx[0] = idx[0]+1)
        {
        double count = 0.0;
        for (idx[1] = 0; idx[1] < size[1]; idx[1] = idx[1]+1)
          {
          if (reader->GetOutput()->GetPixel(idx) > m_foreground - epsilon
              && reader->GetOutput()->GetPixel(idx) < m_foreground + epsilon)
            {
            count = count + 1.0;
            }
          }
        if (count > maxcount) maxcount = count;
        }
    std::cout << maxcount << ",";

    // check z
    maxcount = 0.0;
    for (idx[0] = 0; idx[0] < size[0]; idx[0] = idx[0]+1)
      for (idx[1] = 0; idx[1] < size[1]; idx[1] = idx[1]+1)
        {
        double count = 0.0;
        for (idx[2] = 0; idx[2] < size[2]; idx[2] = idx[2]+1)
          {
          if (reader->GetOutput()->GetPixel(idx) > m_foreground - epsilon
              && reader->GetOutput()->GetPixel(idx) < m_foreground + epsilon)
            {
            count = count + 1.0;
            }
          }
        if (count > maxcount) maxcount = count;
        }
    std::cout << maxcount << "," << vol << "," << spacing[0]
              << "," << spacing[1] << "," << spacing[2]
              << "," << spacing[0] * spacing[1] * spacing[2] << std::endl;
    
    } // done loop through all images

}

} // end namespace

#endif
