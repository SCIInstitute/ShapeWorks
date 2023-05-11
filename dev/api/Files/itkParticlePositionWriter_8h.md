---
title: Libs/Particles/itkParticlePositionWriter.h

---

# Libs/Particles/itkParticlePositionWriter.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticlePositionWriter](../Classes/classshapeworks_1_1ParticlePositionWriter.md)**  |




## Source code

```cpp
#pragma once

#include <string>
#include <vector>

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace shapeworks {
class ParticlePositionWriter : public itk::DataObject {
 public:
  typedef ParticlePositionWriter Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  typedef itk::Point<double, 3> PointType;

  itkNewMacro(Self);

  itkTypeMacro(ParticlePositionWriter, itk::DataObject);

  void SetInput(const std::vector<PointType>& p) { m_Input = p; }

  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  inline void Write() { this->Update(); }
  void Update() {
    // Open the output file.
    std::ofstream out(m_FileName.c_str());

    if (!out) {
      itkExceptionMacro("Could not open point file for output: " << m_FileName.c_str());
    }

    //  out << num_points;

    // Write points.
    for (typename std::vector<PointType>::const_iterator it = m_Input.begin(); it != m_Input.end(); it++) {
      for (unsigned int i = 0; i < 3; i++) {
        out << (*it)[i] << " ";
      }
      out << std::endl;
    }

    out.close();
  };

 protected:
  ParticlePositionWriter() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const {
    Superclass::PrintSelf(os, indent);

    os << indent << "ParticlePositionWriter: " << std::endl;
  }
  virtual ~ParticlePositionWriter(){};

 private:
  ParticlePositionWriter(const Self&);  // purposely not implemented
  void operator=(const Self&);          // purposely not implemented

  std::vector<PointType> m_Input;
  std::string m_FileName;
};

}  // end namespace itk
```


-------------------------------

Updated on 2023-05-11 at 06:01:12 +0000
