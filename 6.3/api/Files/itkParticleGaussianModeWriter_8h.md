---
title: Libs/Optimize/ParticleSystem/itkParticleGaussianModeWriter.h

---

# Libs/Optimize/ParticleSystem/itkParticleGaussianModeWriter.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleGaussianModeWriter](../Classes/classitk_1_1ParticleGaussianModeWriter.md)**  |




## Source code

```cpp
#pragma once

#include "itkParticleShapeMatrixAttribute.h"
#include "itkDataObject.h"
#include <vector>
#include <string>

namespace itk
{

template <unsigned int VDimension>
class ParticleGaussianModeWriter : public DataObject
{
public:
  typedef ParticleGaussianModeWriter Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef DataObject Superclass;
  itkTypeMacro( ParticleGaussianModeWriter, DataObject);

  typedef ParticleSystem ParticleSystemType;
  typedef ParticleShapeMatrixAttribute<double, VDimension> ShapeMatrixType;
  typedef typename ShapeMatrixType::DataType DataType;
  
  typedef typename ParticleSystemType::PointType PointType;
  typedef vnl_vector<DataType> vnl_vector_type;
  typedef vnl_matrix<DataType> vnl_matrix_type;
  
  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  void Update() const;
  
  int GetNumberOfModes() const
  { return m_NumberOfModes; }
  void SetNumberOfModes(int b)
  { m_NumberOfModes = b; }

  void SetShapeMatrix( ShapeMatrixType *s)
  {    m_ShapeMatrix = s;  }
  ShapeMatrixType *GetShapeMatrix()
  {   return  m_ShapeMatrix.GetPointer();  }
  const ShapeMatrixType *GetShapeMatrix() const
  {   return  m_ShapeMatrix.GetPointer();  }

  void SetFileName(const std::string &s)
  { m_FileName = s; }
  void SetFileName(const char *s)
  { m_FileName = s; }
  const std::string &GetFileName() const
  { return m_FileName; }
    
protected:
  ParticleGaussianModeWriter() : m_NumberOfModes(3) {  }
  virtual ~ParticleGaussianModeWriter() {}
  void operator=(const ParticleGaussianModeWriter &);
  ParticleGaussianModeWriter(const ParticleGaussianModeWriter &);
  typename ShapeMatrixType::Pointer m_ShapeMatrix;

  int m_NumberOfModes;
  std::string m_FileName;

};


} //end namespace


#include "itkParticleGaussianModeWriter.txx"
```


-------------------------------

Updated on 2022-04-15 at 07:45:38 +0000
