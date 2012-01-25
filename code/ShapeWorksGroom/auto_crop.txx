/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: auto_crop.txx,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st__auto_crop_txx
#define __st__auto_crop_txx

#include "itkExtractImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "bounding_box.h"

namespace shapetools
{

template <class T, unsigned int D>
auto_crop<T,D>::auto_crop(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    //PARAMSET(pf, g, "background", 0, ok, 1.0);
    this->m_background = 1.0;
    elem = docHandle.FirstChild( "background" ).Element();
    if (elem)
    {
      this->m_background = static_cast<T>(atof(elem->GetText()));
    }
    else
    {
      std::cerr << "auto_crop:: missing parameters" << std::endl;
    }

    //PARAMSET(pf, m_pad, "pad", 0, ok, 0);
    this->m_pad = 0;
    elem = docHandle.FirstChild( "pad" ).Element();
    if (elem) this->m_pad = atoi(elem->GetText());
  }
}



template <class T, unsigned int D> 
void auto_crop<T,D>::operator()()
{
  typename std::vector<std::string>::const_iterator it
    = this->input_filenames().begin();
  typename std::vector<std::string>::const_iterator oit
    = this->output_filenames().begin();
  std::cout << "WARNING!  Assumes objects are centered in each image!" << std::endl;
  //  typename image_type::IndexType upper;
  //  typename image_type::IndexType lower;
  typename image_type::SizeType maxsize;

  bool first = true;
  for (; it != this->input_filenames().end(); it++)
    {
 typename itk::ImageFileReader<image_type>::Pointer reader =
    itk::ImageFileReader<image_type>::New();
    
    reader->SetFileName( (*it).c_str() );
    reader->UpdateLargestPossibleRegion();

    // Compute the bounding boxes.
    bounding_box<T,D> bb_tool;
    bb_tool.background() = m_background;
    bb_tool( reader->GetOutput() );

    std::cout << "reading " << *it << std::endl;
    std::cout << bb_tool.region() << std::endl;

    typename image_type::IndexType mycenter;
    for (unsigned i = 0; i < D; i++)
      {
      mycenter[i]  = reader->GetOutput()->GetRequestedRegion().GetIndex()[i]
        + (  reader->GetOutput()->GetRequestedRegion().GetSize()[i] / 2);
        }
    if (first == true) // save the first bounding box size
      {
      first = false;
      //      lower = bb_tool.region().GetIndex();
      //      upper = lower + bb_tool.region().GetSize();
      for (unsigned int i = 0; i < D; i++)
        {
        maxsize[i] = bb_tool.region().GetSize()[i];
        }
      }
    else
      {
      // Keep the largest bounding box.
      //      typename image_type::RegionType::IndexType lowerTmp
      //        = bb_tool.region().GetIndex();
      //      typename image_type::RegionType::IndexType upperTmp
      //        = lowerTmp + bb_tool.region().GetSize();
      typename image_type::SizeType lowbdist, upbdist, sz;
   
      for (unsigned int i = 0; i < D; i++)
        {
        // Compute the distance from lower bound to center
        lowbdist[i] = mycenter[i] - bb_tool.region().GetIndex()[i];

        // Compute the distance from upper bound to center
        upbdist[i] = (bb_tool.region().GetIndex()[i] + bb_tool.region().GetSize()[i]) - mycenter[i];
        
        // Take the larger of the two.
        if (lowbdist[i] > upbdist[i]) sz[i] = lowbdist[i];
        else sz[i] = upbdist[i];
        
        // Use that to construct maxsize
        if (maxsize[i] < sz[i]*2)
          {
          maxsize[i] = sz[i] *2;
          }
        //        if (lowerTmp[i] < lower[i]) lower[i] = lowerTmp[i];
        //        if (upperTmp[i] > upper[i]) upper[i] = upperTmp[i];
        }
      }
    }
  //  std::cout << "Largest bounding box is " << lower << " - " << upper << std::endl;
  // Now we have the largest bounding box.  Pad it and crop all of the images.

  // NOTE: NO BOUNDS CHECKING!

 //  typename image_type::RegionType::SizeType size;
  //  typename image_type::RegionType::IndexType corner;
  //  for (unsigned int i = 0; i < D; i++)
  //    {
  //    corner[i] = lower[i] - m_pad;
  //    size[i]   = (upper[i] + (m_pad *2)) - lower[i];
  //}
  //  typename image_type::RegionType cropRegion;
  //  cropRegion.SetSize(size);
  //  cropRegion.SetIndex(corner);

  for (unsigned int i = 0; i < D; i++)
    {
    maxsize[i] += m_pad;
    }
  
  std::cout << "crop size is " << maxsize << std::endl;
  
  
  it  = this->input_filenames().begin();
  oit = this->output_filenames().begin();
  for (; it != this->input_filenames().end(); it++, oit++)
    {
    typename itk::ImageFileReader<image_type>::Pointer reader =
      itk::ImageFileReader<image_type>::New();

    reader->SetFileName( (*it).c_str() );
    reader->UpdateLargestPossibleRegion();
    std::cout << "cropping " << *it << std::endl;
    //    std::cout << "with requested region " << reader->GetOutput()->GetRequestedRegion() << std::endl;

    typename itk::ExtractImageFilter<image_type, image_type>::Pointer extractor
      = itk::ExtractImageFilter<image_type, image_type>::New();
    

    // Construct region around center of image.
    typename image_type::RegionType reg;
    typename image_type::IndexType idx;

    typename image_type::IndexType thisidx = reader->GetOutput()->GetRequestedRegion().GetIndex();
    typename image_type::SizeType thissz = reader->GetOutput()->GetRequestedRegion().GetSize();
    std::cout << "thisidx = " << thisidx << std::endl;
    std::cout << "thissz = " << thissz << std::endl;
    int extra[D];
    bool pad = false;
    for (unsigned int i = 0; i < D; i++)
      {
      idx[i] = ((thisidx[i] + thissz[i] / 2)  - maxsize[i] / 2);
      if (idx[i] < 0)
        {
        //        std::cerr << "ERROR!  The image dimensions are smaller than the crop dimensions!"
        //                  << std::endl;
        pad = true;
        extra[i] = -idx[i];
        }
      }
  typename itk::ConstantPadImageFilter<image_type, image_type>::Pointer padder
        = itk::ConstantPadImageFilter<image_type, image_type>::New();

    if (pad == true)
      {
          unsigned long lb[D];
      unsigned long up[D];
      for ( unsigned int i = 0; i < D; i++)
        {
        lb[i]= extra[i]*2;
        std::cout << "lb = " << lb[i] << std::endl;
        up[i] = extra[i]*2;
        }
      padder->SetConstant(m_background);
      padder->SetPadLowerBound(lb);
      padder->SetPadUpperBound(up);
      padder->SetInput(reader->GetOutput());
      padder->Update();
      std::cout << "Padding done" << std::endl;
      thisidx = padder->GetOutput()->GetRequestedRegion().GetIndex();
      thissz = padder->GetOutput()->GetRequestedRegion().GetSize();
      
      for (unsigned int i = 0; i < D; i++)
        {
        idx[i] = ((thisidx[i] + thissz[i] / 2)  - maxsize[i] / 2);
        }
      
        }

      reg.SetSize(maxsize);
      reg.SetIndex(idx);
      
      std::cout << "siz = " << maxsize << std::endl;
      std::cout << "index = " << idx << std::endl;
      if (pad == true)   extractor->SetInput(padder->GetOutput());
      else    extractor->SetInput(reader->GetOutput());
      extractor->SetExtractionRegion(reg);
      
      extractor->Update();

      // Make sure the image information is correct.
      itk::Matrix<double,3,3> I;
      I.SetIdentity();
      extractor->GetOutput()->SetDirection(I);

      double ss[3];
      ss[0] = ss[1] = ss[2] = 1.0;
      
      float o[3];
      o[0] = - (extractor->GetOutput()->GetBufferedRegion().GetSize()[0] / 2.0);
      o[1] = - (extractor->GetOutput()->GetBufferedRegion().GetSize()[1] / 2.0);
      o[2] = - (extractor->GetOutput()->GetBufferedRegion().GetSize()[2] / 2.0);

      std::cout << "---" << o[0] << " " << o[1] << " " << o[2] << std::endl;
      
      extractor->GetOutput()->SetOrigin(o);
      extractor->GetOutput()->SetSpacing(ss);
      

      
      typename itk::ImageFileWriter<image_type>::Pointer writer =
        itk::ImageFileWriter<image_type>::New();
      writer->SetFileName( (*oit).c_str() );
      writer->SetInput( extractor->GetOutput());
      writer->Update();
    }

}

} // end namespace

#endif
