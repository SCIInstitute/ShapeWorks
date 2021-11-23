---
title: Libs/Optimize/ParticleSystem/itkImageToVTKImageFilter.h

---

# Libs/Optimize/ParticleSystem/itkImageToVTKImageFilter.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md)** <br>Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline.  |




## Source code

```cpp
/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkImageToVTKImageFilter_h
#define __itkImageToVTKImageFilter_h

#include "itkVTKImageExport.h"
#include "vtkImageImport.h"
#include "vtkImageData.h"

namespace itk
{

template <class TInputImage >
class ImageToVTKImageFilter : public ProcessObject
{
public:
  typedef ImageToVTKImageFilter       Self;
  typedef ProcessObject             Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(ImageToVTKImageFilter, ProcessObject);

  typedef TInputImage InputImageType;
  typedef typename InputImageType::ConstPointer  InputImagePointer;

  typedef VTKImageExport< InputImageType>        ExporterFilterType;
  typedef typename ExporterFilterType::Pointer   ExporterFilterPointer;

  vtkImageData *  GetOutput() const;

  void SetInput( const InputImageType * );

  vtkImageImport * GetImporter() const;

  ExporterFilterType * GetExporter() const;

  void Update();

protected:
  ImageToVTKImageFilter();
  virtual ~ImageToVTKImageFilter();

private:
  ImageToVTKImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ExporterFilterPointer       m_Exporter;
  vtkImageImport *            m_Importer;
};

} // end namespace itk

//#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToVTKImageFilter.hxx"
//#endif

#endif
```


-------------------------------

Updated on 2021-11-23 at 22:14:03 +0000
