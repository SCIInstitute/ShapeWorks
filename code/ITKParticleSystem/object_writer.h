/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: object_writer.h,v $

  Date:      $Date: 2011/03/24 01:17:34 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __object_writer_h

#define __object_writer_h



#include <vector>

#include <string>



/** \class object_writer

 */

template <class T>

class object_writer

{

public:

  /** Standard class typedefs */

  typedef object_writer Self;



  typedef T ObjectType;

  

  /** Set the input vector.  The input is a reference to a std::vector of ObjectType. */

  void SetInput( const std::vector<ObjectType> &p)

  {

   m_Input = p;

  }



  void SetFileName(const char *fn)

  { m_FileName = fn; }

  void SetFileName(const std::string &fn)

  { m_FileName = fn; }

  const std::string& GetFileName() const

  { return m_FileName; }



  /** Write the file. */

  inline void Write()

  { this->Update(); }

  void Update();

  

  object_writer()  { }

  virtual ~object_writer() {};



 private:

  object_writer(const Self&); //purposely not implemented

  void operator=(const Self&); //purposely not implemented



  std::vector<ObjectType> m_Input;

  std::string m_FileName;

};



# include "object_writer.txx"



#endif

