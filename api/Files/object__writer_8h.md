---
title: Libs/Optimize/ParticleSystem/object_writer.h

---

# Libs/Optimize/ParticleSystem/object_writer.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[object_writer](../Classes/classobject__writer.md)**  |




## Source code

```cpp
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

template <class T>
class object_writer
{
public:
  typedef object_writer Self;

  typedef T ObjectType;
  
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
```


-------------------------------

Updated on 2021-11-18 at 01:30:46 +0000
