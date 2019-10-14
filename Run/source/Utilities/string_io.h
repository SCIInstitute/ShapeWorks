/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: string_io.h,v $
  Date:      $Date: 2011/03/24 01:17:41 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef string_io_h
#define string_io_h

#include <iostream>
#include <vector>
#include <string>

/**
 * \class string_io
 *
 * Read/write a std::vector of strings (a list of strings) from/to disk.
 * Strings are assumed to be delimited by a newline.  Use this class by
 * specifying a filename and calling read() or write().  If reading, the file
 * contents are contained in the m_strings member variable.  If writing, the
 * contents of the m_strings member variable are written to the file.
 */
class string_io 
{
public:
  string_io() : m_filename("/dev/null") {}
  ~string_io() {}

  /** Set/get the list of strings to read/write. */
  std::vector<std::string> &strings()
  { return m_strings;}
  const std::vector<std::string> &strings() const
  { return m_strings; }
  void strings(const std::vector<std::string> &m)
  { m_strings = m; }

  /** Specify the name of the file to read/write. */
  void filename(const char *s)
  { this->filename(std::string(s)); }
  std::string filename() const
  { return m_filename; }
  void filename(const std::string &f)
  { m_filename = f;  }

  /** Writes a list of strings to a text file, one string per line. */
  void write() const;

  /** Reads a list of strings from a text file, one line per string.*/
  void read();
  
protected:
  string_io &operator=(const string_io&); // purposely unimplemented
  string_io(const string_io &); // purposely unimplemented
  std::vector<std::string> m_strings;
  std::string m_filename;
  
}; // string_io



/** Breaks a string of tokens delimited by spaces into a list of tokens
    (strings). */
std::vector<std::string> split_string(const std::string &);


/** A stream inserter for lists of strings.  This method will let us dump the
    contents of a list of strings to output streams. */ 
std::ostream &operator<<(std::ostream & os, const std::vector<std::string> &v);
 


#endif // end ifndef string_io_h
