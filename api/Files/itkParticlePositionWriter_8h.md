---
title: Libs/Particles/itkParticlePositionWriter.h

---

# Libs/Particles/itkParticlePositionWriter.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticlePositionWriter](../Classes/classitk_1_1ParticlePositionWriter.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticlePositionWriter.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticlePositionWriter_h
#define __itkParticlePositionWriter_h

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkObjectFactory.h"
#include <vector>
#include <string>

namespace itk
{
template <unsigned int VDimension>
class ITK_EXPORT ParticlePositionWriter : public DataObject
{
public:
  typedef ParticlePositionWriter Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  typedef Point<double, VDimension> PointType;
  
  itkNewMacro(Self);

  itkTypeMacro(ParticlePositionWriter, DataObject);

  void SetInput( const std::vector<PointType> &p)
  {
   m_Input = p;
  }

  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  inline void Write()
  { this->Update(); }
  void Update();
  
protected:
  ParticlePositionWriter()  { }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  
    os << indent << "ParticlePositionWriter: " << std::endl;
  }
  virtual ~ParticlePositionWriter() {};

 private:
  ParticlePositionWriter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<PointType> m_Input;
  std::string m_FileName;
};

} // end namespace itk

#include "itkParticlePositionWriter.cpp"

#endif
```


-------------------------------

Updated on 2022-01-22 at 00:21:05 +0000
