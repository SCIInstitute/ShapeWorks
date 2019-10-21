/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: object_writer.txx,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __object_writer_txx
#define __object_writer_txx

#include <fstream>
#include <iostream>

template <class T>
void object_writer<T>::Update()
{
  // Open the output file.
//  std::ofstream out( m_FileName.c_str(), std::ios::binary );
    std::ofstream out( m_FileName.c_str());
 
  if ( !out )
    {
    std::cerr << "Could not open point file for output: " << m_FileName << std::endl;
    throw 1;
    }

  // Write the number of objects
  int sz = m_Input.size();
  out.write(reinterpret_cast<char *>(&sz), sizeof(int));
  
  sz = sizeof(ObjectType);
  // Write the objects
  for (typename std::vector<ObjectType>::const_iterator it= m_Input.begin();
       it != m_Input.end(); it++)
    {
    ObjectType q = *it; // maybe not the most efficient, but safe
    out.write(reinterpret_cast<char *>(&q), sz);
    }

   out.close();
}

#endif
