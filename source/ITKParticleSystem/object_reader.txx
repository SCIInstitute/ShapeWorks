/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: object_reader.txx,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

    This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __object_reader_txx
#define __object_reader_txx

#include <fstream>

template <class T>
void object_reader<T>::Update()
{
  // Open the output file.
  std::ifstream in( m_FileName.c_str(), std::ios::binary );
 
  if (!in)
    {
    std::cerr << "Could not open filename " << m_FileName << std::endl;
    throw 1;
    }
  // Read the number of transforms
  int N;
  in.read(reinterpret_cast<char *>(&N), sizeof(int));
  
  int sz = sizeof(ObjectType);
  // Read the transforms
  for (unsigned int i = 0; i < (unsigned int)N; i++)
    {
    ObjectType q; // maybe not the most efficient, but safe
    in.read(reinterpret_cast<char *>(&q), sz);
    m_Output.push_back(q);
    }

   in.close();
}

#endif
