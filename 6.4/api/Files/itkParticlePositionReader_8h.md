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
#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace itk {
class ITK_EXPORT ParticlePositionReader : public DataObject {
 public:
  typedef ParticlePositionReader Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;

  typedef Point<double, 3> PointType;

  itkNewMacro(Self);

  itkTypeMacro(ParticlePositionReader, DataObject);

  const std::vector<PointType>& GetOutput() const { return m_Output; }

  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  inline void Read() { this->Update(); }
  void Update() {
    int counter = 0;
    // Open the ascii file.
    std::ifstream in(m_FileName.c_str());
    if (!in) {
      itkExceptionMacro("Could not open point file for input: " << m_FileName.c_str());
    }

    // Read all of the points, one point per line.
    while (in) {
      PointType pt;
      for (unsigned int d = 0; d < 3; d++) {
        in >> pt[d];
      }

      m_Output.push_back(pt);
      counter++;
    }
    // this algorithm pushes the last point twice
    m_Output.pop_back();
    in.close();
  };

 protected:
  ParticlePositionReader() {}
  void PrintSelf(std::ostream& os, Indent indent) const {
    Superclass::PrintSelf(os, indent);

    os << indent << "ParticlePositionReader: " << std::endl;
  }
  virtual ~ParticlePositionReader(){};

 private:
  ParticlePositionReader(const Self&);  // purposely not implemented
  void operator=(const Self&);          // purposely not implemented

  std::vector<PointType> m_Output;
  std::string m_FileName;
};

}  // end namespace itk
```


-------------------------------

Updated on 2022-11-18 at 17:37:28 +0000
