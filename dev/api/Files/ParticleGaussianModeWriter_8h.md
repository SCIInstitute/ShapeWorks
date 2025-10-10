---
title: Libs/Optimize/Utils/ParticleGaussianModeWriter.h

---

# Libs/Optimize/Utils/ParticleGaussianModeWriter.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleGaussianModeWriter](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md)**  |




## Source code

```cpp
#pragma once

#include <string>
#include <vector>

#include "Libs/Optimize/Matrix/LegacyShapeMatrix.h"
#include "itkDataObject.h"

namespace shapeworks {

template <unsigned int VDimension>
class ParticleGaussianModeWriter : public itk::DataObject {
 public:
  typedef ParticleGaussianModeWriter Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef DataObject Superclass;
  itkTypeMacro(ParticleGaussianModeWriter, DataObject);

  typedef ParticleSystem ParticleSystemType;
  typedef LegacyShapeMatrix ShapeMatrixType;
  typedef typename ShapeMatrixType::DataType DataType;

  typedef typename ParticleSystemType::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;

  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  void Update() const;

  int GetNumberOfModes() const { return m_NumberOfModes; }
  void SetNumberOfModes(int b) { m_NumberOfModes = b; }

  void SetShapeMatrix(ShapeMatrixType* s) { m_ShapeMatrix = s; }
  ShapeMatrixType* GetShapeMatrix() { return m_ShapeMatrix.GetPointer(); }
  const ShapeMatrixType* GetShapeMatrix() const { return m_ShapeMatrix.GetPointer(); }

  void SetFileName(const std::string& s) { m_FileName = s; }
  void SetFileName(const char* s) { m_FileName = s; }
  const std::string& GetFileName() const { return m_FileName; }

 protected:
  ParticleGaussianModeWriter() : m_NumberOfModes(3) {}
  virtual ~ParticleGaussianModeWriter() {}
  void operator=(const ParticleGaussianModeWriter&);
  ParticleGaussianModeWriter(const ParticleGaussianModeWriter&);
  typename ShapeMatrixType::Pointer m_ShapeMatrix;

  int m_NumberOfModes;
  std::string m_FileName;
};

}  // namespace shapeworks

#include "ParticleGaussianModeWriter.txx"
```


-------------------------------

Updated on 2025-10-10 at 18:16:01 +0000
