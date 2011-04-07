/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: surfaceviewerApp.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "surfaceviewerApp.h"
#include "vtkAxes.h"

surfaceviewerApp::surfaceviewerApp(const char *ref)
{
  // Initialize GUI variables
  this->defaultValue->value(-4.0);

  // Pipeline creating the reference shape isosurface.
  m_reference.filename( ref);
  m_reference.color(1.0, 0.49, 0.33);
  //  m_reference.color(0.5, 0.3, 0.5);
  m_reference.opacity(1.0);

  // An axes actor
  m_axes = vtkAxes::New();
  m_axes_mapper = vtkPolyDataMapper::New();
  m_axes_actor = vtkActor::New();
  m_tube_filter = vtkTubeFilter::New();

  m_axes->SetOrigin(0.0, 0.0, 0.0);
  m_axes->SetScaleFactor(20.0);
  m_tube_filter->SetInputConnection(m_axes->GetOutputPort());
  m_tube_filter->SetRadius(m_axes->GetScaleFactor() / 25.0);
  m_tube_filter->SetNumberOfSides(6);
  m_axes_mapper->SetInputConnection(m_tube_filter->GetOutputPort());
  m_axes_actor->SetMapper(m_axes_mapper);
  m_axes_actor->VisibilityOn();
    
  // Set up renderer and interactor.
  m_renderer= vtkRenderer::New();
  m_render_window = vtkRenderWindow::New();
  m_render_window->AddRenderer(m_renderer);
  
  m_renderer->AddActor( m_reference.output() );
  m_renderer->AddActor( m_axes_actor );


  m_renderer->SetBackground(0.5, 0.5, 0.5);
  m_render_window->SetSize(500, 500);


  // Change to trackball style interaction.
  vtkInteractorStyleSwitch *iass =
    dynamic_cast<vtkInteractorStyleSwitch*>(this->imageView->GetInteractorStyle());
  iass->SetCurrentStyleToTrackballCamera();
  
  this->imageView->SetRenderWindow(m_render_window);
  m_render_window->SetInteractor(this->imageView);

  this->imageView->Initialize();
}

surfaceviewerApp::~surfaceviewerApp()
{
  m_renderer->Delete();
  m_render_window->Delete();

}
