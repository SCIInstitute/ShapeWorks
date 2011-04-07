/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: sphere_widget_pipeline.h,v $

  Date:      $Date: 2011/03/24 01:17:41 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __sphere_widget_pipeline_h

#define __sphere_widget_pipeline_h



#include "vtkPolyDataMapper.h"

#include "vtkActor.h"

#include "vtkProperty.h"

#include <string>

//#include "vtkSphereWidget.h"

#include "vtkSphereSource.h"

#include "vtkTransform.h"

#include "vtkTransformPolyDataFilter.h"



#include "itkParticleSystem.h"





class sphere_widget_pipeline

{

 public:

  sphere_widget_pipeline();

  ~sphere_widget_pipeline();



  //  void SetResolution(double x, double y)

  //  {

  //    m_sphere_source->SetResolution(x,y);

  //  }

  

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



  void SetRadius(double r)

  {

    m_sphere_source->SetRadius(r);

  }

  void SetCenter(const vnl_vector_fixed<double,3> &c)

  {

    m_sphere_source->SetCenter(c[0],c[1],c[2]);

  }

  

protected:

  sphere_widget_pipeline(const sphere_widget_pipeline &o);

  const sphere_widget_pipeline &operator=(const sphere_widget_pipeline &o);



  unsigned int m_MyDomain;



  //  vtkSphereWidget *m_sphere_widget;

  vtkSphereSource *m_sphere_source;

  vtkPolyData *m_polydata;

  vtkPolyDataMapper *m_mapper;

  vtkActor *m_actor;

  vtkTransformPolyDataFilter *m_transformer;

  

};





#endif

