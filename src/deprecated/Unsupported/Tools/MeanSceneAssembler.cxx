/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: MeanSceneAssembler.cxx,v $
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
#include "tinyxml.h"
#include <sstream>
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

  TiXmlDocument doc(argv[1]);
  bool loadOkay = doc.LoadFile();
  TiXmlHandle docHandle( &doc );
  TiXmlElement *elem;
  std::istringstream inputsBuffer;

  std::string outputfile;
  unsigned int points_per_file;

  //PARAMSET(pf, points_per_file, "points_per_file", 0, ok, 1);
  points_per_file = 1;
  elem = docHandle.FirstChild( "points_per_file" ).Element();
  if (elem)
    points_per_file = atoi(elem->GetText());
  else
  {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
  }

  //PARAMSET(pf, outputfile, "output", 0, ok,"bogus");
  outputfile = "bogus";
  elem = docHandle.FirstChild( "output" ).Element();
  if (elem)
    outputfile = elem->GetText();
  else
  {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
  }
  
  // Collect a list of point file names and output file names
  int i=0;
  std::vector< std::string > metafiles;
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
      metafiles.push_back(tmp);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  unsigned int num_files = metafiles.size();
  std::cout << "Read " << num_files << " files " << std::endl;
  
  const std::string pointstring("Points");
  const std::string endofpointstring("CellType");
  const std::string cellstring("Cells");
  
  // Loop through the point files and assemble every domains_per_shape number
  // into a single file.
  std::vector<point_type> points;
  std::vector<edge_type> edges;
  for (unsigned int g = 0; g < num_files; g++)
    {
    unsigned int offset = points_per_file * g;
    // Read the next input file
    CS6350::string_io mesh_file;
    mesh_file.filename( metafiles[g] );
    mesh_file.read();

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
        p.x = ::atof(tmp[1].c_str());
        p.y = ::atof(tmp[2].c_str());
        p.z = ::atof(tmp[3].c_str());
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
    std::ofstream out( outputfile.c_str() );
    
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
  
  return 0;
}
