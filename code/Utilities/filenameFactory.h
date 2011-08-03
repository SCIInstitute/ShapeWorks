/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: filenameFactory.h,v $
  Date:      $Date: 2011/03/24 01:17:41 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef _oec_filenameFactory_h
#define _oec_filenameFactory_h

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>

/**
 * */
class filenameFactory
{
public:
  filenameFactory() {}
  ~filenameFactory() {}

  void number_of_files(unsigned int n);
  unsigned int number_of_files() const
  { return m_number_of_files; }

  //  void file_pattern(const std::string &s)
  //  { m_file_pattern = s; }
  const std::string &file_pattern() const
  { return m_file_pattern; }

  void file_format(const std::string &s)
  { m_file_format = s; }
  const std::string &file_format() const
  { return m_file_format; }

  void prefix(const std::string &s)
  { m_prefix = s; }
  const std::string &prefix() const
  { return m_prefix; }

  std::string filename(unsigned int) const;

  // record the iteration info
  std::string filename(unsigned int, unsigned int) const; 
  
private:
  unsigned int m_number_of_files;
  std::string m_file_pattern;
  std::string m_prefix;
  std::string m_file_format;
};

#endif // _oec_filenameFactory_h
