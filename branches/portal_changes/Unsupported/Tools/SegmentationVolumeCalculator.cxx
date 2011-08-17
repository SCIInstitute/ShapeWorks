/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: SegmentationVolumeCalculator.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// Reads a set of binary segmentation files and computes the volume.
// Outputs a list of volumes and a lists of scales which normalize the volume.

#include <iostream>
#include <string>
#include <fstream>
#include "itkImageFileReader.h"
#include "itkImage.h"
#include "tinyxml.h"
#include <sstream>
#include "itkImageRegionConstIterator.h"


int main(int argc, char *argv[])
{
  typedef itk::Image<int, 3> ImageType;
  
  if (argc != 2)
    {
    std::cerr << "Use: " << argv[0] << "paramfile" << std::endl;
    return 1;
    }

  TiXmlDocument doc(argv[1]);
  bool loadOkay = doc.LoadFile();
  TiXmlHandle docHandle( &doc );
  TiXmlElement *elem;
  std::istringstream inputsBuffer;

  std::string outputfile;
  int background = 0;

  //PARAMSET(pf, background, "background", 0, ok, 0);
  background = 0;
  elem = docHandle.FirstChild( "background" ).Element();
  if (elem) background = atoi(elem->GetText());

  //PARAMSET(pf, outputfile, "output", 0, ok, "volumes.txt");
  outputfile = "volumes.txt";
  elem = docHandle.FirstChild( "output" ).Element();
  if (elem) outputfile = elem->GetText();

  // Collect a list of input file names
  std::vector< std::string > inputfiles;
  std::string tmp;

  elem = docHandle.FirstChild( "inputs" ).Element();
  if (!elem)
  {
    std::cerr << "No input files have been specified" << std::endl;
    throw 1;
  }
  else
  {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> tmp)
    {
      inputfiles.push_back(tmp);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  std::vector<unsigned int> volumes;
  std::vector<double> scales;
  // COMPUTE VOLUMES
  double mean = 0.0;
  for (unsigned int i = 0; i < inputfiles.size(); i++)
    {
    itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
    reader->SetFileName(inputfiles[i].c_str());

    std::cout << inputfiles[i].c_str();

    try
      {    reader->Update();  }
    catch(itk::ExceptionObject &e)
      {
      std::cerr << e << std::endl;
      return 1;
      }

    itk::ImageRegionConstIterator<ImageType> it(reader->GetOutput(),
                                                reader->GetOutput()->GetRequestedRegion());
    unsigned int count = 0;
    for (it.GoToBegin(); ! it.IsAtEnd(); ++it)
      {
      if (it.Get() != background)
        {
        count++;
        }      
      }
    std::cout << " "<<  count << std::endl;
    mean += (double)count;
    volumes.push_back(count);
    }
  mean /= (double)(inputfiles.size());

  // COMPUTE SCALES
  for (unsigned int i = 0; i < volumes.size(); i++)
    {
    scales.push_back( mean / (double)(volumes[i]));
    }


  /// OUTPUT

  // Open the output file.
    std::ofstream out( outputfile.c_str() );
    
    if ( !out )
      {
      std::cerr << "Could not open output file" << std::endl;
      return 2;
      }
    
    // Write the meta header (why is ndims in here twice?)


    // Write mean volume
    out << "// Volume is in voxel units " << std::endl;
    out << "(mean_volume " << mean << ")" << std::endl;
    
    // Write volumes
    out << "(volumes" << std::endl;
    for (unsigned int i = 0; i < volumes.size(); i++)
      {
      out << volumes[i] << std::endl;
      }
    out << ")" << std::endl;

    // Write scales
    out << "// Scale is percent of mean volume" << std::endl;
    out << "(scales" << std::endl;
    for (unsigned int i = 0; i < scales.size(); i++)
      {
      out << scales[i] << " // " << inputfiles[i] << std::endl;
      }
    out << ")" << std::endl;


    // Write 1/scales
    out << "// Inverse scale is 1 / percent of mean volume" << std::endl;
    out << "(inv_scales" << std::endl;
    for (unsigned int i = 0; i < scales.size(); i++)
      {
      out << 1.0 / scales[i] << " " << inputfiles[i] << std::endl;
      }
    out << ")" << std::endl;

    
    out.close();   
  
  return 0;
}

