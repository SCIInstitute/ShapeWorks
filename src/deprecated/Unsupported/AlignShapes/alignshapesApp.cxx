/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: alignshapesApp.cxx,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "alignshapesApp.h"
#include "vnl/vnl_cross.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkAffineTransform.h"
#include "itkMatrix.h"
#include "itkImageFileWriter.h"
#include "Procrustes3D.h"
#include "vtkMatrix4x4.h"
#include "vtkAxes.h"

void vtkMyCallback::Execute(vtkObject *caller, unsigned long, void*)
{
  vtkTransform *t = vtkTransform::New();
  vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
  widget->GetTransform(t);
  widget->GetProp3D()->SetUserTransform(t);

  if (m_glyphs_moving != 0) m_glyphs_moving->transform(t);
  
  char buf[512];
  std::string txt;
  if (gui->axisAngle->value() == 0)
    {
    double orient[4];
    t->GetOrientationWXYZ(orient);
    sprintf(buf, "%2.2f\t%2.2f\t%2.2f\t%2.2f", orient[0], orient[1], orient[2], orient[3]);
    txt += buf;
    txt += "\n";
    }
  else
    {
    for (unsigned int r =0; r < 4; r++)
      {
      for (unsigned int c = 0; c < 4; c++)
        {
        sprintf(buf, "%2.2f\t", t->GetMatrix()->GetElement(r,c));
        txt += buf;
        }
      txt += "\n";
      }
    }
  gui->matrixDisplay->value(txt.c_str());
  gui->writeButton->activate();  
  t->Delete();
}


void alignshapesApp::Procrustes()
{
  const int numPoints = m_glyphs_moving.points()->GetNumberOfPoints();
  const int numShapes = 2;
  
  Procrustes3D::ShapeListType shapelist;//(_num_shapes, numPoints);
  Procrustes3D::ShapeType shapevector;
  Procrustes3D::PointType point;
  double *p;
    
  // Read input shapes from file list
  for(int i = 0; i < numShapes; i++)
    {
    shapevector.clear();
    for(int j = 0; j < numPoints; j++)
      {
      if (i==0) p = m_glyphs_reference.points()->GetPoint(j);
      if (i==1) p = m_glyphs_moving.points()->GetPoint(j);

      point(0) = p[0];
      point(1) = p[1];
      point(2) = p[2];
      
      shapevector.push_back(point);
      }
    shapelist.push_back(shapevector);
    }
  
  // Run alignment
  Procrustes3D::SimilarityTransformListType transforms;
  Procrustes3D procrustes;
  procrustes.AlignShapes(transforms, shapelist);

  vtkMatrix4x4 *m1[numShapes];
  vtkTransform *t[numShapes];
  vtkMatrix4x4 *m[numShapes];
  vtkMatrix4x4 *m0[numShapes];
 
  // Construct transform matrices for each particle system.
  for (int i = 0; i < numShapes; i++)
    {
    std::cout << "i = " << i << std::endl;
    std::cout << "Scale = " ;
    std::cout << transforms[i].scale << std::endl;
    std::cout << "Rotation = " <<std::endl;
    std::cout << transforms[i].rotation << std::endl;
    std::cout << "Translation = " ;
    std::cout << transforms[i].translation << std::endl << std::endl << std::endl;

    m[i]  = vtkMatrix4x4::New();
    m0[i] = vtkMatrix4x4::New();
    m1[i] = vtkMatrix4x4::New();
    t[i]  = vtkTransform::New();
    
    m1[i]->SetElement(0, 0, transforms[i].rotation(0,0) * transforms[i].scale);
    m1[i]->SetElement(1, 0, transforms[i].rotation(1,0) * transforms[i].scale);
    m1[i]->SetElement(2, 0, transforms[i].rotation(2,0) * transforms[i].scale);
    m1[i]->SetElement(3, 0, 0.0);

    m1[i]->SetElement(0, 1, transforms[i].rotation(0,1) * transforms[i].scale);
    m1[i]->SetElement(1, 1, transforms[i].rotation(1,1) * transforms[i].scale);
    m1[i]->SetElement(2, 1, transforms[i].rotation(2,1) * transforms[i].scale);
    m1[i]->SetElement(3, 1, 0.0);
    
    m1[i]->SetElement(0, 2, transforms[i].rotation(0,2) * transforms[i].scale);
    m1[i]->SetElement(1, 2, transforms[i].rotation(1,2) * transforms[i].scale);
    m1[i]->SetElement(2, 2, transforms[i].rotation(2,2) * transforms[i].scale);
    m1[i]->SetElement(3, 2, 0.0);

    m1[i]->SetElement(0, 3, transforms[i].translation(0));
    m1[i]->SetElement(1, 3, transforms[i].translation(1));
    m1[i]->SetElement(2, 3, transforms[i].translation(2));
    m1[i]->SetElement(3, 3, 1.0);

    if (i==0) m0[i] =
      reinterpret_cast<vtkTransform *>(m_glyphs_reference.transformer()->GetTransform())->GetMatrix();
    if (i==1) m0[i] =
      reinterpret_cast<vtkTransform *>(m_glyphs_moving.transformer()->GetTransform())->GetMatrix();
    
    vtkMatrix4x4::Multiply4x4(m1[i], m0[i], m[i]);

    t[i]->SetMatrix(m[i]);
    
    //print final matrix
    for (unsigned int r = 0; r < 4; r++)
      {
      for (unsigned int c = 0; c < 4; c++)
        {std::cout << m[i]->GetElement(r,c) << " "; }
      std::cout << std::endl;
      }
    
    if (i==0 )
      {
      m_glyphs_reference.transform(t[i]);
      m_reference.output()->SetUserTransform(t[i]);
      }

    if (i==1 )
      {
      m_glyphs_moving.transform(t[i]);
      m_boxWidget->SetTransform(t[i]);
      //      m_moving.output()->SetUserTransform(t);
      m_boxWidget->GetProp3D()->SetUserTransform(t[i]);
      }

    //    t->Delete();
    }
  
  m_reference.output()->Modified();
  m_moving.output()->Modified();
  m_boxWidget->Modified();
  m_renderer->Render();  
}


alignshapesApp::alignshapesApp(const char *ref, const char *mov, const char *points_reference,
                               const char *points_moving, bool load_points)
{
  // Initialize GUI variables
  this->defaultValue->value(0);
  this->isovalue_input->value(1.0);
  this->linearInterpolate->value(0);
  this->outputFilename->value(mov);
  if (load_points == true)  this->pointsFilename->value(points_moving);

  // Pipeline creating the reference shape isosurface.
  m_reference.SetIsoValue(1.0);

  m_reference.filename( ref);
  m_reference.color(0.5, 0.3, 0.5);
  m_reference.opacity(0.5);

  // Pipeline creating the reference shape isosurface.
  m_moving.SetIsoValue(1.0);
  
  m_moving.filename( mov);
  m_moving.color(1.0, 0.49, 0.33);
  m_moving.opacity(0.5);

  // Optionally create pipelines for glyphs
  if (load_points == true)
    {
    m_glyphs_moving.load_points(points_moving);
    m_glyphs_moving.color(0.7, 0.49, 0.33);
    m_glyphs_moving.scale(1.0);
   
    m_glyphs_reference.load_points(points_reference);
    m_glyphs_reference.color(0.3, 0.3, 0.5);
    m_glyphs_reference.scale(1.0);
    }

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
  m_axes_actor->VisibilityOff();
    
  // Set up renderer and interactor.
  m_renderer= vtkRenderer::New();
  m_render_window = vtkRenderWindow::New();
  m_render_window->AddRenderer(m_renderer);
  
  m_renderer->AddActor( m_reference.output() );
  m_renderer->AddActor( m_moving.output() );
  m_renderer->AddActor( m_axes_actor );

  if (load_points == true)
    {
    m_renderer->AddActor( m_glyphs_moving.output() );
    m_renderer->AddActor( m_glyphs_reference.output() );
    }
  m_renderer->SetBackground(0.0, 0.0, 0.0);
  m_render_window->SetSize(500, 500);

  m_boxWidget = vtkBoxWidget::New();
  m_boxWidget->SetPlaceFactor(1.25);
  m_boxWidget->SetProp3D(m_moving.output());
  m_boxWidget->ScalingEnabledOff();
  //  m_boxWidget->TranslationEnabledOff();
  m_boxWidget->PlaceWidget();

  m_box_callback = vtkMyCallback::New();
  m_box_callback->gui = this;

  if (load_points == true) m_box_callback->m_glyphs_moving = &m_glyphs_moving;
  else m_box_callback->m_glyphs_moving = 0;
  
  m_boxWidget->AddObserver(vtkCommand::InteractionEvent, m_box_callback);


  // Change to trackball style interaction.
  vtkInteractorStyleSwitch *iass =
    dynamic_cast<vtkInteractorStyleSwitch*>(this->imageView->GetInteractorStyle());
  iass->SetCurrentStyleToTrackballCamera();
  
  m_boxWidget->SetInteractor(this->imageView);
  
  this->imageView->SetRenderWindow(m_render_window);
  m_render_window->SetInteractor(this->imageView);

  this->imageView->Initialize();
}

void alignshapesApp::Write()
{
  const double PI2 = 6.283185307;
  typedef isosurface_pipeline::ImageType image_type;
  vtkTransform *t = vtkTransform::New();
  m_boxWidget->GetTransform(t);

  itk::AffineTransform<double>::Pointer trans = itk::AffineTransform<double>::New();
  itk::AffineTransform<double>::OutputVectorType axis;
  itk::AffineTransform<double>::OutputVectorType off;
  double angle;

  // Get the axis angle representation of the rotation.
  double orient[4];
  t->GetOrientationWXYZ(orient);
  angle   = -(orient[0] / 360.0) * PI2;
  axis[0] = orient[1];
  axis[1] = orient[2];
  axis[2] = orient[3];
  trans->Rotate3D(axis, angle, false);
  
  // Get the translation component
  double pos[3];
  t->GetPosition(pos);
  off[0] = -pos[0];
  off[1] = -pos[1];
  off[2] = -pos[2];
  trans->Translate(off, true);
  
  // Perform the transformation.
  itk::LinearInterpolateImageFunction<image_type,double>::Pointer
    interp1 = itk::LinearInterpolateImageFunction<image_type,double>::New();

  itk::NearestNeighborInterpolateImageFunction<image_type,double>::Pointer
    interp2 = itk::NearestNeighborInterpolateImageFunction<image_type,double>::New();
  
  itk::ResampleImageFilter<image_type, image_type>::Pointer resampler
    = itk::ResampleImageFilter<image_type, image_type>::New();
  resampler->SetOutputParametersFromImage( m_moving.input() );
  resampler->SetTransform(trans);
  if (this->linearInterpolate->value() == 1)  resampler->SetInterpolator(interp1);
  else resampler->SetInterpolator(interp2);
  resampler->SetInput( m_moving.input() );
  resampler->SetDefaultPixelValue( static_cast<double>( defaultValue->value() ));
  resampler->Update();

  itk::ImageFileWriter<image_type>::Pointer writer
    = itk::ImageFileWriter<image_type>::New();
  writer->SetFileName( m_moving.filename().c_str() );
  writer->SetFileName( this->outputFilename->value() );
  writer->SetInput( resampler->GetOutput() );
  writer->Update();

  // Now write the points
  m_glyphs_moving.write_points(this->pointsFilename->value());
  this->writeButton->deactivate();
}

alignshapesApp::~alignshapesApp()
{
  m_renderer->Delete();
  m_render_window->Delete();
  m_boxWidget->Delete();
  m_box_callback->Delete();
}
