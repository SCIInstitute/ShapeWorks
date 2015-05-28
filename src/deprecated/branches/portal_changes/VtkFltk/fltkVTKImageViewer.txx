/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: fltkVTKImageViewer.txx,v $
  Language:  C++
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "fltkVTKImageViewer.h"
#include "vtkCamera.h"

namespace fltk {
  
template <class ImagePixelType>
VTKImageViewer<ImagePixelType>
::VTKImageViewer()
{

  m_RenderWindow   = vtkRenderWindow::New();
  m_Renderer       = vtkRenderer::New();

  m_AdaptorFilter  = AdaptorFilterType::New();

  // ray casting can only be performed on 
  // unsigned short or unsigned char data
  m_AdaptorFilter->GetImporter()->SetDataScalarTypeToUnsignedChar();

  fltkRenderWindowInteractor->SetRenderWindow( m_RenderWindow );

  // a renderer and render window
  m_RenderWindow->AddRenderer( m_Renderer );

  m_Renderer->SetBackground( 1.0, 1.0, 1.0 ); 
  m_Renderer->GetActiveCamera()->Zoom( 1.0 ); 
  m_Renderer->GetActiveCamera()->SetPosition(0.0, 0.0, 20.0 ); 

  // Create a transfer function mapping scalar value to opacity
  vtkPiecewiseFunction *opacity = vtkPiecewiseFunction::New();
    opacity->AddSegment(80, 0.0, 255, 1.0);

  // Create a transfer function mapping scalar value to color (grey)
  vtkPiecewiseFunction *color = vtkPiecewiseFunction::New();
    color->AddSegment(0, 1.0, 255, 1.0);

  // Create a property for the volume and set the transfer functions.
  // Turn shading on and use trilinear interpolation
  vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
    volumeProperty->SetColor(color);
    volumeProperty->SetScalarOpacity(opacity);
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn();

  // Create a ray function - this is a compositing ray function
  vtkVolumeRayCastCompositeFunction *compositeFunction =
                            vtkVolumeRayCastCompositeFunction::New();

  // Create the volume mapper and set the ray function and scalar input
  vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();
    volumeMapper->SetInput( m_AdaptorFilter->GetOutput() );
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);

  // Create the volume and set the mapper and property
  vtkVolume *volume = vtkVolume::New();
    volume->SetMapper( volumeMapper );
    volume->SetProperty( volumeProperty );

  // Add this volume to the renderer 
#if USE_PROP
    m_Renderer->AddProp( volume );
#else
    m_Renderer->AddViewProp( volume );
#endif
  
  // Interact with the data at 3 frames per second
  fltkRenderWindowInteractor->SetDesiredUpdateRate(3.0);
  fltkRenderWindowInteractor->SetStillUpdateRate(0.001);

}


  
template <class ImagePixelType>
VTKImageViewer<ImagePixelType>
::~VTKImageViewer()
{
  // Delete only VTK objects
  if( m_RenderWindow )
    {
    m_RenderWindow->Delete();
    }

  if( m_Renderer )
    {
    m_Renderer->Delete();
    }

}



template <class ImagePixelType>
void
VTKImageViewer<ImagePixelType>
::SetImage(ImageType * img)
{
  m_AdaptorFilter->SetInput( img );
}




template <class ImagePixelType>
void 
VTKImageViewer<ImagePixelType>
::Show(void)
{
  fltkRenderWindowInteractor->Initialize();
  Superclass::Show(); 
}





template <class ImagePixelType>
void 
VTKImageViewer<ImagePixelType>
::Hide(void)
{
  Superclass::Hide();
}



} // end namespace itk
