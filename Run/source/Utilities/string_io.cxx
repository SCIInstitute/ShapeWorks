/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: string_io.cxx,v $
  Date:      $Date: 2011/03/24 01:17:41 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "string_io.h"
#include <fstream>
namespace CS6350
{

string_list split_string(const std::string &s)
{
  string_list list;
  unsigned int start = 0;

  // Step through the string and find tokens delimited by space(s).
  for (unsigned int i = 0; i < s.size(); i++)
    {
    if (s[i] == ' ')
      {
      if ( i != start ) list.push_back(s.substr(start, i - start));
      start = i+1;
      }
    }
  list.push_back(s.substr(start, s.size() - start));
  
  return list;
}
 
void string_io::write() const
{
  std::ofstream fout(m_filename.c_str(), std::ios::out);

  if (! fout.is_open() )
    {
    throw std::runtime_error("string_io: could not open file for writing");
    }

  string_list::const_iterator it;
  for (it = m_strings.begin(); it != m_strings.end(); it++)
    {
    fout << *it;
    fout << std::endl;
    }
  
  fout.close();
}

void string_io::read()
{
  const int n = 4096;
  char str[n];
  std::ifstream fin(m_filename.c_str(), std::ios::in);

  // Attempt to open the file.
  if (! fin.is_open() )
    {
    throw std::runtime_error("string_io: could not open file for reading");
    }

  m_strings.clear(); // empty the list
  
  // Read each line to a new string.  Push this new string onto the list.
  while (! fin.eof())
    {
    fin.getline(str, n);
    m_strings.push_back(std::string(str));
    }
  fin.close();
}

} // end namespace CS6350

std::ostream &operator<<(std::ostream & os, const CS6350::string_list &v)
{
  for (unsigned int i = 0; i < v.size(); i++)
    {
    os << v[i] << std::endl;
    }
  return os;
}
