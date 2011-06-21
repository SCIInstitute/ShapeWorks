/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: alignshapesApp.h,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __alignshapesApp_h
#define __alignshapesApp_h

#include <cstdio>
#include "itkImage.h"
#include "isosurface_pipeline.h"
#include "glyph_pipeline.h"
#include "vtkRenderer.h"
#include "alignshapesGUI.h"
#include "vtkCommand.h"
#include "vtkBoxWidget.h"
#include "vtkTransform.h"
#include "vtkRenderWindow.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkGlyph3D.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"

class vtkMyCallback : public vtkCommand
{
 public:
  alignshapesGUI *gui;
  static vtkMyCallback *New()    { return new vtkMyCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*);
  glyph_pipeline *m_glyphs_moving;
};

class alignshapesApp : public alignshapesGUI
{
 public:
  alignshapesApp(const char *, const char *, const char *, const char *, bool);
  virtual ~alignshapesApp();

  virtual void toggleAxes()
  {
    if ( this->toggleAxesButton->value() == 0) m_axes_actor->VisibilityOff();
    else m_axes_actor->VisibilityOn();
  }
  
  void Hide() {}

  virtual void Show()
  {
    mainWindow->show();
    imageView->show();
    imageView->Initialize();
  }

  virtual void Write();
  virtual void ReferenceOpacity()
  {
    this->m_reference.opacity( refOpacity->value() );
    m_render_window->Render();
  }
  virtual void MovingOpacity()
  {
    this->m_moving.opacity( movOpacity->value() );
    m_render_window->Render();
  }
  virtual void BoxOn()
  {
    if (this->boxOn->value() == 1)
      {
      m_boxWidget->On();
      }
   else m_boxWidget->Off();
  }
  virtual void moving_scale()
  {
    m_glyphs_moving.scale(m_moving_scale->value());
  }
  
  virtual void reference_scale()
  {
    m_glyphs_reference.scale(m_reference_scale->value());
  }
  
  virtual void Quit()
  {
    mainWindow->hide();
  }
 virtual void SetIsovalue()
  {
    m_reference.SetIsoValue(isovalue_input->value());
    m_moving.SetIsoValue(isovalue_input->value());
    std::cout << "isovalue = " << isovalue_input->value() << std::endl;
  }
  virtual void Procrustes();
 protected:
  glyph_pipeline m_glyphs_reference;
  glyph_pipeline m_glyphs_moving;
  isosurface_pipeline m_reference;
  isosurface_pipeline m_moving;
  vtkRenderer *m_renderer;
  vtkRenderWindow *m_render_window;
  vtkBoxWidget *m_boxWidget;
  vtkMyCallback *m_box_callback;
  vtkAxes *m_axes;
  vtkPolyDataMapper *m_axes_mapper;
  vtkActor *m_axes_actor;
  vtkTubeFilter *m_tube_filter;
  
};


#endif
