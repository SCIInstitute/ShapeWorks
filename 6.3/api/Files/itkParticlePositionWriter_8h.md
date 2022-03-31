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
#pragma once

#include <string>
#include <vector>

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace itk {
class ITK_EXPORT ParticlePositionWriter : public DataObject {
 public:
  typedef ParticlePositionWriter Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;

  typedef Point<double, 3> PointType;

  itkNewMacro(Self);

  itkTypeMacro(ParticlePositionWriter, DataObject);

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
  void PrintSelf(std::ostream& os, Indent indent) const {
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

Updated on 2022-03-31 at 09:10:17 -0600
