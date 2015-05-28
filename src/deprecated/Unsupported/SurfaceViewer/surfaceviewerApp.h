/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: surfaceviewerApp.h,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __surfaceviewerApp_h
#define __surfaceviewerApp_h

#include <cstdio>
#include "itkImage.h"
#include "isosurface_pipeline.h"
#include "vtkRenderer.h"
#include "surfaceviewerGUI.h"
#include "vtkCommand.h"
#include "vtkTransform.h"
#include "vtkRenderWindow.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"

class surfaceviewerApp : public surfaceviewerGUI
{
 public:
  surfaceviewerApp(const char *);
  virtual ~surfaceviewerApp();

  virtual void toggleAxes()
  {
    //    if ( this->toggleAxesButton->value() == 0) m_axes_actor->VisibilityOff();
    //    else m_axes_actor->VisibilityOn();
  }
  
  virtual void Show()
  {
    mainWindow->show();
    imageView->show();
    imageView->Initialize();
  }

  virtual void ReferenceOpacity()
  {
    this->m_reference.opacity( refOpacity->value() );
    m_render_window->Render();
  }
  
  virtual void Quit()
  {
    mainWindow->hide();
  }
  virtual void SetIsovalue()
  {
    m_reference.SetIsoValue(isovalue_input->value());
    std::cout << "isovalue = " << isovalue_input->value() << std::endl;
  }
  
 protected:
  isosurface_pipeline m_reference;

  vtkRenderer *m_renderer;
  vtkRenderWindow *m_render_window;
  vtkAxes *m_axes;
  vtkPolyDataMapper *m_axes_mapper;
  vtkActor *m_axes_actor;
  vtkTubeFilter *m_tube_filter;
};


#endif
