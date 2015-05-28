/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: MeshToMetaConverter.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// A simple utility to convert a points file into a meta data object file.

#include <iostream>
#include <vector>
#include <string>
#include "string_io.h"
#include <fstream>


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
  if (argc != 3)
    {
    std::cerr << "Use: " << argv[0] << "input output" << std::endl;
    return 1;
    }

  const std::string vertex("Vertex");
  const std::string face("Face");

  std::vector<point_type> points;
  std::vector<edge_type> edges;
  
 // Read the input files
  CS6350::string_io mesh_file;
  mesh_file.filename(argv[1]);
  mesh_file.read();

  // Trim whitespace and blank lines and Figure out how many points are in the
  // mesh.
  int Nv = 0;
  int Nf = 0;
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
    
    if (tmp[0]  == vertex)
      {     
      Nv = ::atoi(tmp[1].c_str());
      p.x = static_cast<float>(::atof(tmp[2].c_str()));
      p.y = static_cast<float>(::atof(tmp[3].c_str()));
      p.z = static_cast<float>(::atof(tmp[4].c_str()));

      points.push_back(p);
      
      }

    if (tmp[0]  == face)
      {     
      Nf = ::atoi(tmp[1].c_str());
      f.a = ::atoi(tmp[2].c_str());
      f.b = ::atoi(tmp[3].c_str());
      f.c = ::atoi(tmp[4].c_str());

      edges.push_back(f);
      }
    
    }

  std::cout << "Number of points = " << Nv << std::endl;
  std::cout << "Number of faces = " << Nf << std::endl;

  // Open the output file.
  std::ofstream out( argv[2] );
 
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
  out << "NPoints = " << Nv << std::endl;
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
  out << "NCells = " <<  Nf << std::endl;
  out << "Cells = " << std::endl;

  for (unsigned int i = 0; i < edges.size(); i++)
    {
    out << i
        << " " << edges[i].a - 1
        << " " << edges[i].b -1 
        << " " << edges[i].c -1 << " "
        << std::endl;
    }

   
  out.close();


  

  return 0;
}
