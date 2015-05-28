/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: auto_pad.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__auto_pad_txx
#define __st__auto_pad_txx

#include "bounding_box.h"
#include "itkConstantPadImageFilter.h"
#include "itkChangeInformationImageFilter.h"

namespace shapetools
{

template <class T, unsigned int D>
auto_pad<T,D>::auto_pad(const char *fname)
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
      std::cerr << "auto_pad:: missing parameters" << std::endl;
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
      std::cerr << "auto_pad:: missing parameters" << std::endl;
    }

    //PARAMSET(pf, m_pad, "pad", 0, ok, 0);
    this->m_pad = 0;
    elem = docHandle.FirstChild( "pad" ).Element();
    if (elem) this->m_pad = atoi(elem->GetText());
  }
}


template <class T, unsigned int D> 
void auto_pad<T,D>::operator()()
{
  typename itk::ImageFileReader<image_type>::Pointer reader =
    itk::ImageFileReader<image_type>::New();
  
  typename std::vector<std::string>::const_iterator it
    = this->input_filenames().begin();
  typename std::vector<std::string>::const_iterator oit
    = this->output_filenames().begin();

  typename image_type::IndexType upper;
  typename image_type::IndexType lower;
    
  bool first = true;
  for (; it != this->input_filenames().end(); it++)
    {
    reader->SetFileName( (*it).c_str() );
    reader->Update();

    if (first == true) // save the first bounding box
      {
      first = false;
      lower = reader->GetOutput()->GetBufferedRegion().GetIndex();
      upper = lower + reader->GetOutput()->GetBufferedRegion().GetSize();
      }
    else
      {
      // Keep the largest bounding box.
      typename image_type::RegionType::IndexType lowerTmp
        = reader->GetOutput()->GetBufferedRegion().GetIndex();
      typename image_type::RegionType::IndexType upperTmp
        = lowerTmp + reader->GetOutput()->GetBufferedRegion().GetSize();

      for (unsigned int i = 0; i < D; i++)
        {
        if (lowerTmp[i] < lower[i])
          {
          lower[i] = lowerTmp[i];
          std::cout << *it << std::endl;
          }
        if (upperTmp[i] > upper[i])
          {
          upper[i] = upperTmp[i];
          std::cout << *it << std::endl;
          }

        }
      }
    }

  std::cout << "Lower bound = " << lower[0] << " " << lower[1]
            << " " << lower[2] << std::endl;
  std::cout << "Upper bound = " << upper[0] << " " << upper[1] << " "
            << upper[2] << std::endl;
  
  // Now we have the largest bounding box.  
//   typename image_type::RegionType::SizeType size;
//   typename image_type::RegionType::IndexType corner;
//   for (unsigned int i = 0; i < D; i++)
//     {
//     corner[i] = lower[i] - m_pad;
//     size[i]   = (upper[i] + m_pad) - lower[i];
//     }
//   typename image_type::RegionType padRegion;
//   padRegion.SetSize(size);
//   padRegion.SetIndex(corner);

  typename itk::ConstantPadImageFilter<image_type, image_type>::Pointer padder
    = itk::ConstantPadImageFilter<image_type, image_type>::New();

  padder->SetConstant(0);

  // Make sure the origin is at the center of the image.
  double orig[D];
  for (unsigned int i = 0; i < D; i++)
    {    orig[i] = -static_cast<double>(upper[i] - lower[i]) / 2.0;    }
  
  it  = this->input_filenames().begin();
  oit = this->output_filenames().begin();
  for (; it != this->input_filenames().end(); it++, oit++)
     {
     typename itk::ImageFileWriter<image_type>::Pointer writer =
       itk::ImageFileWriter<image_type>::New();
     typename itk::ChangeInformationImageFilter<image_type>::Pointer changer =
       itk::ChangeInformationImageFilter<image_type>::New();

     changer->ChangeOriginOn();
     changer->ChangeRegionOff();
     changer->ChangeSpacingOff();
     changer->UseReferenceImageOff();
     
     reader->SetFileName( (*it).c_str() );
     reader->UpdateLargestPossibleRegion();

     
     padder->SetInput(reader->GetOutput());
     changer->SetOutputOrigin(orig);
     changer->SetInput(padder->GetOutput());
     
     // Find the necessary padding
     int diff[D];
     unsigned long lowpad[D];
     unsigned long hipad[D];
     bool flag = false;
     for (unsigned int i = 0; i < D; i++)
       {
       diff[i] = (upper[i] - lower[i])
         - reader->GetOutput()->GetBufferedRegion().GetSize()[i];
       if (diff[i] < 0)
       {  std::cerr << "auto_pad:: negative pad" << std::endl;
          throw 1;
       }
       lowpad[i] = diff[i] / 2;
       hipad[i]  = diff[i] - lowpad[i];
       if (lowpad[i] != 0 || hipad[i] != 0) flag = true;
       }

     padder->SetPadUpperBound(hipad);
     padder->SetPadLowerBound(lowpad);
     padder->UpdateLargestPossibleRegion();
     changer->UpdateLargestPossibleRegion();
     
     std::cout << "input region = "
               << reader->GetOutput()->GetBufferedRegion().GetSize()
               << std::endl;
     std::cout << "lowpad: " << lowpad[0] << " " << lowpad[1] << " " << lowpad[2]
               << std::endl;
     std::cout << "hipad: " << hipad[0] << " " << hipad[1] << " " << hipad[2]
               << std::endl;


     std::cout << "Changer origin = " << changer->GetOutput()->GetOrigin() << std::endl;
     
     
     writer->SetInput(changer->GetOutput());
     writer->SetFileName( (*oit).c_str() );
     writer->Update();
     }

}

} // end namespace

#endif
