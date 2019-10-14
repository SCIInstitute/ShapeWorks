/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: CS6350.h,v $
  Date:      $Date: 2011/03/24 01:17:40 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __cs6350_h
#define __cs6350_h

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace CS6350
{

/** A string list is simply a list of strings. */
typedef std::vector<std::string> string_list;

/** A string map is simply a map (lookup table) of strings. */
struct ltstr
{
  bool operator()(const std::string &s1, const std::string &s2) const
  {
    return s1 < s2;
  }
};

// //typedef std::map<std::string, string_list, ltstr> string_map;
// typedef std::map<std::string, int, ltstr> string_index_table;

}  // end namespace CS6350

#endif
