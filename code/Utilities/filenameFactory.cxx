/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: filenameFactory.cxx,v $

  Date:      $Date: 2011/03/24 01:17:40 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "filenameFactory.h"



void filenameFactory::number_of_files(unsigned int n)

{

  char fp[8];

  m_number_of_files = n;

  

  // Find the necessary number of digits in the filename pattern

  unsigned int dig = 0;

  unsigned int c = 1;

  while (c <= n)

    {

    c *= 10;

    dig++;

    }

  ::sprintf(fp, "%d", dig);

  

  // Construct the file pattern string.

  m_file_pattern.clear();

  m_file_pattern = "%0";

  m_file_pattern = m_file_pattern + fp + "d";

}



std::string filenameFactory::filename(unsigned int n) const

{

  char buf[32];

  ::sprintf(buf, m_file_pattern.c_str(), n);

  std::string fn = m_prefix + "." + buf + "." + m_file_format;

  return fn;

}



