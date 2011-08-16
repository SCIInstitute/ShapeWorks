/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: isosurface_pipeline.h,v $
  Date:      $Date: 2011/03/24 01:17:41 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __isosurface_pipeline_h
#define __isosurface_pipeline_h

#include "vtkContourFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkProperty.h"
#include "vtkITKUtility.h"
#include "vtkImageImport.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkVTKImageExport.h"
#include <string>
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyDataWriter.h"


class isosurface_pipeline
{
 public:
  typedef itk::Image<float, 3> ImageType;
  
  isosurface_pipeline();
  ~isosurface_pipeline();

  const std::string &filename() const
  { return m_filename; }
  void filename(const char *fn)
  {
    this->m_filename = std::string(fn);
    m_reader->SetFileName(fn);
  }

  ImageType::Pointer input()
  { return m_reader->GetOutput(); }
  
  vtkActor *output()
  { return m_actor; }

  //  void diffuse_color(double r, double g, double b)
  //  { m_actor->GetProperty()->SetDiffuseColor(r,g,b); }

  void write_vtk_mesh(const char *fn);
  
  void color(double r, double g, double b)
  { m_actor->GetProperty()->SetDiffuseColor(r,g,b); }

  void opacity(double o)
  { m_actor->GetProperty()->SetOpacity(o); }

  itk::ImageFileReader<ImageType>::Pointer reader()
  { return m_reader; }

  void refresh_input()
  {
    m_itk_exporter->Modified();
    m_contour_filter->Modified();
    m_converter->Modified();
    m_vtk_importer->Modified();
    m_actor->Modified();
    m_mapper->Modified();
  }

  /** Set the domain whose events this object will respond to. */
  int MyDomain() const
  { return m_MyDomain; }
  void MyDomain(int d)
  { m_MyDomain = d;}

  void transform(vtkAbstractTransform *t)
  { m_transformer->SetTransform(t); }
  
  void SetTransformCallback(itk::Object *, const itk::EventObject &);
  void SetPrefixTransformCallback(itk::Object *, const itk::EventObject &);

  void SetIsoValue(double d)
  {
    m_contour_filter->SetValue(0, d);
  }
  
protected:
  isosurface_pipeline(const isosurface_pipeline &o);
  const isosurface_pipeline &operator=(const isosurface_pipeline &o);

 unsigned int m_MyDomain;
  
  std::string m_filename;
  vtkContourFilter *m_contour_filter;
  vtkPolyDataMapper *m_mapper;
  vtkActor *m_actor;
  itk::ImageFileReader<ImageType>::Pointer m_reader;
  itk::VTKImageExport<ImageType>::Pointer m_itk_exporter;
  vtkImageImport *m_vtk_importer;
  vtkImageToStructuredPoints *m_converter;

  vtkTransformPolyDataFilter *m_transformer;
  
};


#endif
