---
title: Libs/Optimize/ParticleSystem/object_reader.h

---

# Libs/Optimize/ParticleSystem/object_reader.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[object_reader](../Classes/classobject__reader.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: object_reader.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __object_reader_h
#define __object_reader_h

#include <vector>
#include <string>


template <class T>
class object_reader 
{
public:
  typedef object_reader Self;
  typedef T ObjectType;

  const std::vector<ObjectType> &GetOutput() const
  {
    return m_Output;
  }
  std::vector<ObjectType> &GetOutput()
  {
    return m_Output;
  }

 void SetFileName(const char *fn)
  { m_FileName = fn; }
  void SetFileName(const std::string &fn)
  { m_FileName = fn; }
  const std::string& GetFileName() const
  { return m_FileName; }
  
  inline void Read()
  { this->Update(); }
  void Update();
  
  object_reader() { }
  virtual ~object_reader() {};

 private:
  object_reader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<ObjectType> m_Output;
  std::string m_FileName;
};

# include "object_reader.txx"

#endif
```


-------------------------------

Updated on 2022-01-12 at 19:02:31 +0000
