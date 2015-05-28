/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: plane_widget_pipeline.h,v $
  Date:      $Date: 2011/03/24 01:17:41 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __plane_widget_pipeline_h
#define __plane_widget_pipeline_h

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include <string>
//#include "vtkPlaneWidget.h"
#include "vtkPlaneSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vnl/vnl_cross.h"
#include "itkParticleSystem.h"


class plane_widget_pipeline
{
 public:
  plane_widget_pipeline();
  ~plane_widget_pipeline();

  void SetResolution(double x, double y)
  {
    m_plane_source->SetResolution(x,y);
  }
  
  vtkActor *output()
  { return m_actor; }

  void color(double r, double g, double b)
  { m_actor->GetProperty()->SetDiffuseColor(r,g,b); }

  void opacity(double o)
  { m_actor->GetProperty()->SetOpacity(o); }

  /** Set the domain whose events this object will respond to. */
  int MyDomain() const
  { return m_MyDomain; }
  void MyDomain(int d)
  { m_MyDomain = d;}

  void transform(vtkAbstractTransform *t)
  { m_transformer->SetTransform(t); }
  
  void SetTransformCallback(itk::Object *, const itk::EventObject &);
  void SetPrefixTransformCallback(itk::Object *, const itk::EventObject &);

  void SetCenter(double x,double y, double z)
  { m_plane_source->SetCenter(x,y,z); }
  
  void SetPoints(const vnl_vector_fixed<double,3> &a,
                 const vnl_vector_fixed<double,3> &b,
                 const vnl_vector_fixed<double,3> &c,
                 double extentX, double extentY)
  {
    // Compute plane
    vnl_vector_fixed<double,3> n = vnl_cross_3d(b-a,c-a);
    n.normalize();    
    double d = dot_product(-n,a);

    // Extend the boundaries of the plane
    vnl_vector_fixed<double,3> x;
    vnl_vector_fixed<double,3> y;
    vnl_vector_fixed<double,3> z;
    x[0] = -extentX;
    x[1] = -extentY;
    x[2] = (-d - n[0]*x[0] - n[1]*x[1]) / n[2];

    y[0] = extentX;
    y[1] = -extentY;
    y[2] = (-d - n[0]*y[0] - n[1]*y[1]) / n[2];
    
    z[0] =  -extentX;
    z[1] =  extentY;
    z[2] = (-d - n[0]*z[0] - n[1]*z[1]) / n[2];
    
    m_plane_source->SetOrigin(x[0],x[1],x[2]);
    m_plane_source->SetPoint1(y[0],y[1],y[2]);
    m_plane_source->SetPoint2(z[0],z[1],z[2]);
    m_plane_source->SetCenter(0.0, 0.0, d);
  }
  
protected:
  plane_widget_pipeline(const plane_widget_pipeline &o);
  const plane_widget_pipeline &operator=(const plane_widget_pipeline &o);

  unsigned int m_MyDomain;

  //  vtkPlaneWidget *m_plane_widget;
  vtkPlaneSource *m_plane_source;
  vtkPolyData *m_polydata;
  vtkPolyDataMapper *m_mapper;
  vtkActor *m_actor;
  vtkTransformPolyDataFilter *m_transformer;
  
};


#endif
