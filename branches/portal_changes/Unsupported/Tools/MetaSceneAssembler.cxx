/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: MetaSceneAssembler.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// Reads a set of meta files and assembles them into a set of metafile scenes.
// This application is intended to assemble various domains into a single
// coordinate frame.  A transformation for each domain into the common
// coordinate frame is required.

#include <iostream>
#include <vector>
#include <string>
#include "string_io.h"
#include <fstream>
#include "itkParticleSystem.h"
#include "param.h"
#include "object_reader.h"

struct point_type
{
  float x;
  float y;
  float z;
};

struct edge_type
{
  int a;
  int b;
  int c;
};

int main(int argc, char *argv[])
{
  typedef itk::ParticleSystem<3> ParticleSystemType;
  typedef ParticleSystemType::TransformType TransformType;
  
  if (argc != 2)
    {
    std::cerr << "Use: " << argv[0] << "paramfile" << std::endl;
    return 1;
    }

  param::parameterFile pf(argv[1]);

  bool ok = true;

  unsigned int domains_per_shape, points_per_file;
  std::string transfn;
  int shift = 0;
  PARAMSET(pf, shift, "shift", 0, ok, 0);
  PARAMSET(pf, points_per_file, "points_per_file", 0, ok, 1);
  PARAMSET(pf, domains_per_shape, "domains_per_shape", 0, ok, 1);
  PARAMSET(pf, transfn, "transform_file", 0, ok, "");

  std::cout << "Domains per shape = " << domains_per_shape << std::endl;
  std::cout << "Points per file = " << points_per_file << std::endl;  

  if (ok != true)
    {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
    }

  
  // Collect a list of point file names and output file names
  int i=0;
  std::vector< std::string > metafiles;
  std::vector< std::string > outputfiles;
  std::string tmp;
  while (ok == true)
    {
    // Record the point file names.
    PARAMSET(pf, tmp, "inputs", i, ok, "");
    if (i==0 && ok != true)
      {
      std::cerr << "No input files have been specified" << std::endl;
      throw 1;
      }
    if (ok == true)
      {
      std::cout << i << "\t" << tmp <<  std::endl;
      metafiles.push_back(tmp);
      } // if ok == true
    i++;
    } // while ok == true

  i--;
  unsigned int num_files = i;
  double num_samples = ((double)(num_files)) /((double)(domains_per_shape));
  std::cout << "Read " << num_files << " files " << std::endl;
  if ( i % domains_per_shape != 0)
    {
    std::cerr << "Not a multiple of domains per shape! Exiting." << std::endl;
    return 1;
    }


  ok = true;
  i = 0;
  while (ok == true)
    {
    // Record the outputfiles
    PARAMSET(pf, tmp, "outputs", i, ok, "");
    if (i==0 && ok != true)
      {
      std::cerr << "No output files have been specified" << std::endl;
      throw 1;
      }
    if (ok == true)
      {
      std::cout << i << "\t" << tmp <<  std::endl;
      outputfiles.push_back(tmp);
      } // if ok == true
    i++;
    } // while ok == true



  // Compute the average translations.
  object_reader< itk::ParticleSystem<3>::TransformType > transreader;
  std::vector< point_type > avgtrans;
  avgtrans.resize(domains_per_shape);
  transreader.SetFileName(transfn);
  transreader.Update();
  for (unsigned int i = 0; i < avgtrans.size(); i++)
    {
    avgtrans[i].x = 0.0;
    avgtrans[i].y = 0.0;
    avgtrans[i].z = 0.0;
    }
  
  for (unsigned int i = 0; i < transreader.GetOutput().size(); i++)
    {
    //    std::cout << transreader.GetOutput()[i] << std::endl;
    avgtrans[i % domains_per_shape].x += transreader.GetOutput()[i].operator()(0,3);
    avgtrans[i % domains_per_shape].y += transreader.GetOutput()[i].operator()(1,3);
    avgtrans[i % domains_per_shape].z += transreader.GetOutput()[i].operator()(2,3);
    
    //    std::cout << transreader.GetOutput()[i] << std::endl;    
    }
  
  for (unsigned int i = 0; i < avgtrans.size(); i++)
    {
    avgtrans[i].x /= num_samples;
    avgtrans[i].y /= num_samples;
    avgtrans[i].z /= num_samples;
    std::cout << i << ": " << avgtrans[i].x << " " << avgtrans[i].y << " " << avgtrans[i].z << std::endl;
    }
  
  const std::string pointstring("Points");
  const std::string endofpointstring("CellType");
  const std::string cellstring("Cells");
  
  // Loop through the point files and assemble every domains_per_shape number
  // into a single file.
  unsigned int idx = 0;
  for (unsigned int s = 0; s < outputfiles.size(); s++)
    {
    std::cout << "Shape " << s+1  << "/" << outputfiles.size() << std::endl;
    // First collect all of the points and vertices from each file.
    
    std::vector<point_type> points;
    std::vector<edge_type> edges;
    for (unsigned int g = 0; g < domains_per_shape; g++, idx++)
      {
      std::cout << "Domain " << g+1 << "/" << domains_per_shape << std::endl;
      unsigned int offset = points_per_file * g;
      
      // Read the next input file
      CS6350::string_io mesh_file;
      mesh_file.filename( metafiles[idx] );
      mesh_file.read();

      // Just doing translation here.
      float xtrans = avgtrans[g % domains_per_shape].x;
      float ytrans = avgtrans[g % domains_per_shape].y;
      float ztrans = avgtrans[g % domains_per_shape].z;

      // Parse the file and record all vertices and edges
      int mode = 0;

      for (CS6350::string_list::iterator it = mesh_file.strings().begin();
           it != mesh_file.strings().end(); it++)
        {
        if (it->size() == 0) continue; // skip any empty strings

        // Trim any trailing whitespace
        while ((*it)[it->size() -1] == ' ') { it->erase(it->end() - 1); }
        if (it->size() == 0) continue; // skip any empty strings
        
        CS6350::string_list tmp = CS6350::split_string(*it);
        
        point_type p;
        edge_type f;
        
        if (tmp[0] == pointstring)
          {
          mode = 1;
          continue;
          }
        if (tmp[0] == endofpointstring)
          {
          mode = 2;
          continue;
          }
        if (tmp[0] == cellstring)
          {
          mode = 3;
          continue;
          }

        if (mode == 1) // Add a point with the translation transform
          {
          //       Nv = ::atoi(tmp[1].c_str());
          if (shift == 1)
            {
            p.x = ::atof(tmp[1].c_str()) - xtrans;
            p.y = ::atof(tmp[2].c_str()) - ytrans;
            p.z = ::atof(tmp[3].c_str()) - ztrans;
            }
          else
            {
            p.x = ::atof(tmp[1].c_str());
            p.y = ::atof(tmp[2].c_str());
            p.z = ::atof(tmp[3].c_str());
            }
          points.push_back(p);
          }

        if (mode == 3)  // Add an edge
          {     
          //       Nf = ::atoi(tmp[1].c_str());
          f.a = ::atoi(tmp[1].c_str()) + offset;
          f.b = ::atoi(tmp[2].c_str()) + offset;
          f.c = ::atoi(tmp[3].c_str()) + offset;
          edges.push_back(f);
          }      
        } // for it
      } // for g
    
    // Open the output file.
    std::ofstream out( outputfiles[s].c_str() );
    
    if ( !out )
      {
      std::cerr << "Could not open output file" << std::endl;
      return 2;
      }
    
    // Write the meta header (why is ndims in here twice?)
    out << "ObjectType = Scene" << std::endl;
    out << "NDims = 3" << std::endl;
    out << "NObjects = 1" << std::endl;
    out << "ObjectType = Mesh" << std::endl;
    out << "NDims = 3" << std::endl;
    out << "BinaryData = False" << std::endl;
    out << "TransformMatrix = 1 0 0 0 1 0 0 0 1" << std::endl;
    out << "Offset = 0 0 0" << std::endl;
    out << "CenterOfRotation = 0 0 0" << std::endl;
    out << "ElementSpacing = 1 1 1" << std::endl;
    out << "PointType = MET_FLOAT" << std::endl;
    out << "PointDataType = MET_FLOAT" << std::endl;
    out << "CellDataType = MET_FLOAT" << std::endl;
    out << "NCellTypes = 1" << std::endl;
    out << "PointDim = ID x y ..." << std::endl;
    out << "NPoints = " << points.size() << std::endl;
    out << "Points = "<< std::endl;

    // Write points.
    for (unsigned int i = 0; i < points.size(); i++)
      {
      out << i
          << " " << points[i].x
          << " " << points[i].y
          << " " << points[i].z << " "
          << std::endl;
      
      }
    
    // Write triangles.
    out << "CellType = TRI" << std::endl;
    out << "NCells = " << edges.size() << std::endl;
    out << "Cells = " << std::endl;
    
    for (unsigned int i = 0; i < edges.size(); i++)
      {
      out << i
          << " " << edges[i].a 
          << " " << edges[i].b  
          << " " << edges[i].c  << " "
          << std::endl;
      }
    
    out.close();   
    }// for s
  
  return 0;
}
