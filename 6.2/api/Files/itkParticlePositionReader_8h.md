---
title: Libs/Particles/itkParticlePositionReader.h

---

# Libs/Particles/itkParticlePositionReader.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticlePositionReader](../Classes/classitk_1_1ParticlePositionReader.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticlePositionReader.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticlePositionReader_h
#define __itkParticlePositionReader_h

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include <vector>
#include <string>

namespace itk
{
template <unsigned int VDimension>
class ITK_EXPORT ParticlePositionReader : public DataObject
{
public:
  typedef ParticlePositionReader Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  typedef Point<double, VDimension> PointType;
  
  itkNewMacro(Self);

  itkTypeMacro(ParticlePositionReader, DataObject);

  const std::vector<PointType> &GetOutput() const
  {
    return m_Output;
  }

  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  inline void Read()
  { this->Update(); }
  void Update();
  
protected:
  ParticlePositionReader() { }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  
    os << indent << "ParticlePositionReader: " << std::endl;
  }
  virtual ~ParticlePositionReader() {};

 private:
  ParticlePositionReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<PointType> m_Output;
  std::string m_FileName;
};

} // end namespace itk

#include "itkParticlePositionReader.cpp"

#endif
```


-------------------------------

Updated on 2022-03-31 at 09:51:19 -0600