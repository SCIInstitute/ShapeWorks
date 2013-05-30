/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ShapeWorksGroom.cxx,v $
  Date:      $Date: 2011/03/23 22:40:11 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "isolate.h"
#include "center.h"
#include "batchtool.h"
#include "align_principal.h"
#include "auto_crop.h"
#include "antialias.h"
#include "fastmarching.h"
#include "blur.h"
#include "extract_centers.h"
#include "surface_point.h"
#include "itkExceptionObject.h"
#include "split_segmentations.h"
#include "auto_pad.h"
#include "group.h"
#include "hole_fill.h"
#include "vnl/vnl_vector.h"
#include "transformbatchtool.h"
#include "relabel.h"
#include "measure_length.h"
#include "simple_morphometrics.h"
#include "scale_principal.h"
#include "metaCommand.h"
#include "isotropic.h"
//#include "icp.h"

#define ST_DIM 3 // change to 2 for a 2D build

#ifdef _WIN32
#pragma warning( disable: 4996 )
#endif


int main(int argc, char *argv[])
{ 
  if (argc < 2)
    {
    std::cerr << "Usage: " << argv[0] << " parameterfile <tool1 tool2 ...>"
              << std::endl;
    std::cerr << "Tools: relabel hole_fill isolate center extract_centers align_principal blur antialias fastmarching surface point auto_pad split_segmentations extract_label group scale_principal simple_morphometrics icp"
              << std::endl;
    return 3;
    }
  try
    {
    std::vector<std::string> inputs;
    std::vector<std::string> original_inputs;	//used to keep the segmentation
    std::vector<std::string> outputs;
    int verbose;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if (loadOkay)
      {
      TiXmlHandle docHandle( &doc );
      TiXmlElement *elem;
      std::istringstream inputsBuffer;
      std::string filename("/dev/null\0");

      // Compile the list of input files.
      elem = docHandle.FirstChild( "inputs" ).Element();
      if (elem)
        {
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> filename)
          {
          inputs.push_back(filename);
          original_inputs.push_back(filename);
          }
        inputsBuffer.clear();
        inputsBuffer.str("");
        }

      // Compile the list of output files.
      elem = docHandle.FirstChild( "outputs" ).Element();
      if (elem)
        {
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> filename)
          {
          outputs.push_back(filename);
          }
        inputsBuffer.clear();
        inputsBuffer.str("");
        }

      // Print progress info?
      //PARAMSET(pf, verbose, "verbose", 0, ok, 0);
      verbose = 0;
      elem = docHandle.FirstChild( "verbose" ).Element();
      if (elem) verbose = atoi(elem->GetText());

      // Make sure lists are the same size.
      if (inputs.size() > outputs.size())
        {
        std::cerr << "Input list size does not match output list size!" << std::endl;
        return 6;
        }

      // Set up the batch filter.
      // Apply tools.
      for (int i = 2; i < argc; i++)
        {
        //      shapetools::tool<int, ST_DIM> *t;

        // If this is the second operation, then use the output files as the
        // input files.
        if (i==3)    {  inputs = outputs;  }
        
        if (std::string(argv[i]) == "isolate")
          {
          if (verbose == 1) std::cout << "isolate" << std::endl;
          // set up tool
          shapetools::isolate<int, ST_DIM> t(argv[1]);
          
          // batch filter
          shapetools::batchtool<int, ST_DIM> filter;
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter.tool_to_use() = &t;
          filter();
          }
        else if (std::string(argv[i]) == "hole_fill")
          {
          if (verbose == 1) std::cout << "hole_fill" << std::endl;
          // set up tool
          shapetools::hole_fill<int, ST_DIM> t(argv[1]);

          // batch filter
          shapetools::batchtool<int, ST_DIM> filter;
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter.tool_to_use() = &t;
          filter();
          }
        else if (std::string(argv[i]) == "relabel")
          {
          if (verbose == 1) std::cout << "relabel" << std::endl;
          // set up tool
          shapetools::relabel<int, ST_DIM> t(argv[1]);

          // batch filter
          shapetools::batchtool<int, ST_DIM> filter;
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter.tool_to_use() = &t;
          filter();
          }
        else if (std::string(argv[i]) == "center")
        {
          if (verbose == 1) std::cout << "center" << std::endl;
          // set up tool
          shapetools::center<unsigned char, ST_DIM> t(argv[1]);
          t.center_origin() = true;

          // batch filter
          shapetools::transformbatchtool<unsigned char, ST_DIM> filter;
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;

          filter.transform_file("/dev/null\0");
          elem = docHandle.FirstChild( "transform_file" ).Element();
          if (elem) 
            filter.transform_file(elem->GetText());
          else
          {
            std::cerr << "Missing transform file parameter" << std::endl;
            return 1; 
          }


          bool write_individual_transform = false;
          elem = docHandle.FirstChild( "individual_transform" ).Element();
          if ( elem )
          {
            write_individual_transform = atoi( elem->GetText() );
          }
          filter.set_individual_transform( write_individual_transform );


          //        filter.tool_to_use() = &t;
          filter.set_tool(reinterpret_cast<shapetools::tool<unsigned char, ST_DIM> *>(&t));
          filter();
        }
        else if (std::string(argv[i]) == "isotropic")
        {
          if (verbose == 1) std::cout << "isotropic" << std::endl;
          shapetools::isotropic<unsigned char, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();
        }
        else if (std::string(argv[i]) == "antialias")
          {
          if (verbose == 1) std::cout << "antialias" << std::endl;
          // set up tool
          shapetools::antialias<float, ST_DIM> t(argv[1]);
          
          // batch filter
          shapetools::batchtool<float, ST_DIM> filter;
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter.tool_to_use() = &t;
          filter();
          }
        else if (std::string(argv[i]) == "fastmarching")
          {
          if (verbose == 1) std::cout << "fastmarching" << std::endl;
          // set up tool
          shapetools::fastmarching<float, ST_DIM> t(argv[1]);
          
          // batch filter
          shapetools::batchtool<float, ST_DIM> filter;
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter.tool_to_use() = &t;
          filter();
          }
        else if (std::string(argv[i]) == "blur")
          {
          if (verbose == 1) std::cout << "blur" << std::endl;
          // set up tool
          shapetools::blur<float, ST_DIM> t(argv[1]);
          
          // batch filter
          shapetools::batchtool<float, ST_DIM> filter;
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter.tool_to_use() = &t;
          filter();
          }
	   //  else if (std::string(argv[i]) == "icp")
  // 		{
  //         if (verbose == 1) std::cout << "icp" << std::endl;
          
  //         // batch filter
  //         shapetools::icp<float, ST_DIM> icp_t(pf);
  //         icp_t.input_filenames()  = inputs;
  //         icp_t.output_filenames() = outputs;
  //         icp_t.input_seg_filenames() = original_inputs;
          
  //         std::vector<std::string> icp_outputs;
  //         std::vector<std::string>::iterator input_it 
  //           = original_inputs.begin();
  //         icp_outputs.push_back(*input_it);
  		
  //         for(input_it = original_inputs.begin() + 1; input_it != original_inputs.end(); input_it++)
  //           {
  //           std::string output_seg_filename = *input_it;
            
  //           size_t found = output_seg_filename.rfind(".");
  //           if (found!=std::string::npos)
  //             {
  //             std::string suffix = "_icp";
  //             unsigned int input_suffix_length = 
  //               output_seg_filename.length() - found;
              
  //             suffix = suffix + output_seg_filename.substr(found);
  //             output_seg_filename.replace (found,input_suffix_length,suffix);
  //             icp_outputs.push_back(output_seg_filename);
  //             }
            
  // 		  }
          
  //         icp_t.output_seg_filenames() = icp_outputs;
  //         icp_t();
          
  //         shapetools::antialias<float, ST_DIM> antialias_t(pf);
          
  //         // batch filter
          
  //         shapetools::batchtool<float, ST_DIM> filter;
  //         filter.input_filenames()  = icp_outputs;
  //         filter.output_filenames() = outputs;
  //         filter.tool_to_use() = &antialias_t;
  //         filter();
          
  //         shapetools::fastmarching<float, ST_DIM> fastmarching_t(pf);
          
  //         // batch filter
  //         filter.input_filenames()  = outputs;
  //         filter.tool_to_use() = &fastmarching_t;
  //         filter();
          
  //         shapetools::blur<float, ST_DIM> blur_t(pf);
          
  //         // batch filter
  //         filter.input_filenames()  = outputs;
  //         filter.tool_to_use() = &blur_t;
  //         filter();
          
  //         }
        else if (std::string(argv[i]) == "align_principal")
          {
          // NOTE: THIS ASSUMES 3D
          //        vnl_vector<double> tmp(3);
          if (verbose == 1) std::cout << "align_principal" << std::endl;
          bool oka = true;

          std::vector<int> flip_info;
          int tmpi;

          elem = docHandle.FirstChild( "flip_info" ).Element();
          if (elem)
            {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpi)
              {
              flip_info.push_back(tmpi);
              }
            inputsBuffer.clear();
            inputsBuffer.str("");
            }

          // Make sure we have enough flip info
          if (flip_info.size() != inputs.size())
            {
            std::cerr << "Not enough flip info for align_principal." << std::endl;
            return 1;
            }
          
          shapetools::align_principal<float, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter.flip_info() = flip_info;
          filter();
          }
        else if (std::string(argv[i]) == "scale_principal")
          {
          // NOTE: THIS ASSUMES 3D
          if (verbose == 1) std::cout << "scale_principal" << std::endl;
          
          shapetools::scale_principal<float, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();
          }
        else if (std::string(argv[i]) == "measure_length")
          {
          // NOTE: THIS ASSUMES 3D
          //        vnl_vector<double> tmp(3);
          if (verbose == 1) std::cout << "measure_length" << std::endl;

          shapetools::measure_length<float, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();
          }
        else if (std::string(argv[i]) == "simple_morphometrics")
          {
          // NOTE: THIS ASSUMES 3D
          //        vnl_vector<double> tmp(3);
          if (verbose == 1) std::cout << "simple_morphometrics" << std::endl;

          shapetools::simple_morphometrics<float, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();
          }
        else if (std::string(argv[i]) == "split_segmentations")
          {
          if (verbose == 1) std::cout << "split_segmentations" << std::endl;

          shapetools::split_segmentations<int, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();

          }

        else if (std::string(argv[i]) == "group")
          {
          if (verbose == 1) std::cout << "group" << std::endl;
          // set up tool
          shapetools::group<int, ST_DIM> t(argv[1]);

          // batch filter
          shapetools::batchtool<int, ST_DIM> filter;
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter.tool_to_use() = &t;
          filter();

          }
        else if (std::string(argv[i]) == "surface_point")
          {
          if (verbose == 1) std::cout << "surface_point" << std::endl;
          shapetools::surface_point<float, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();
          }
        else if (std::string(argv[i]) == "auto_crop")
          {
          if (verbose == 1) std::cout << "auto_crop" << std::endl;
          shapetools::auto_crop<unsigned char, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();
          }
        else if (std::string(argv[i]) == "auto_pad")
          {
          if (verbose == 1) std::cout << "auto_pad" << std::endl;
          shapetools::auto_pad<unsigned char, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();
          }
        else if (std::string(argv[i]) == "extract_centers")
          {
          if (verbose == 1) std::cout << "extract_centers" << std::endl;
          shapetools::extract_centers<float, ST_DIM> filter(argv[1]);
          filter.input_filenames()  = inputs;
          filter.output_filenames() = outputs;
          filter();
          }
        else
          {
          std::cerr << "Unrecognized tool: " << argv[i] << std::endl;
          return 4;
          }
        }      
      }
    }
  catch (itk::ExceptionObject &e)
    {
    std::cerr << e << std::endl;
    return 3;
    }
  catch( ... )
    {
    std::cerr << "Unknown exception" << std::endl;
    return 2;
    }
  
  return 0;
}
