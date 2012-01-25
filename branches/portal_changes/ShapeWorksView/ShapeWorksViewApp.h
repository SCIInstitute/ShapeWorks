/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ShapeWorksViewApp.h,v $
  Date:      $Date: 2011/03/24 01:17:39 $
  Version:   $Revision: 1.9 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ShapeWorksViewApp_h
#define __ShapeWorksViewApp_h

#include <cstdio>
#include <vector>
#include <string>

#include <fstream>
#include <iostream>

#include "FL/Fl_File_Chooser.H"

#include "AnalyzeCorrespondenceGUI.h"
#include "tinyxml.h"
#include <sstream>
#include <string>
#include <iostream>
#include "itkParticleShapeLinearRegressionMatrixAttribute.h"
#include "itkParticlePositionReader.h"
#include "object_reader.h"
#include "colorScheme.h"
#include "itkParticleShapeStatistics.h"
#include "itkParticlePositionWriter.h"

#include "vtkTransform.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkInteractorStyleSwitch.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_matrix.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPolyDataNormals.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkArrowSource.h"
#include "vtkLookupTable.h"
#include "vtkUnsignedLongArray.h"
#include "vtkPointData.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkContourFilter.h"
#include "vtkReverseSense.h"
#include "vtkFloatArray.h"
#include "vtkColorTransferFunction.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkImageGradient.h"
#include "vtkImageData.h"
#include "vtkDecimatePro.h"

#ifdef SW_USE_POWERCRUST
  #include "vtkPowerCrustSurfaceReconstruction.h"
  #include "vtkPolyDataToImageStencil.h"
  #include "vtkPolyDataWriter.h"
  #include "vtkDataSetWriter.h"
  #include "vtkImageStencil.h"
  #include "vtkDataArray.h"
  #include "vtkMath.h"
  #include "vtkPointLocator.h"
  #include "vtkDelaunay3D.h"
  #include "vtkProbeFilter.h"
  #include "vtkThinPlateSplineTransform.h"
  #include "vtkGeneralTransform.h"
  #include "vtkTransformPolyDataFilter.h"
#else
  #include "vtkSurfaceReconstructionFilter.h"
#endif



class ShapeWorksViewApp : public AnalyzeCorrespondenceGUI
{
public:
  ShapeWorksViewApp(const char *);
  virtual ~ShapeWorksViewApp()
  {
    m_renderer->Delete();
    m_render_window->Delete();    
  }
  
  virtual void Show()
  {
    mainWindow->show();
    imageView->show();
    imageView->Initialize();
  }
  
  virtual void Quit() { mainWindow->hide(); }

  virtual void ComputeSimpleRegressionShape();
  virtual void ComputeSimpleRegressionParameters();
  virtual void ComputeRegressionShape();
  virtual void ComputeModeShape();
  virtual void ComputeGroupMeanDifferenceShape();

  virtual void WritePoints();
  virtual void LoadPCAShape();
  virtual void LoadPointFile();
  virtual void WritePCALoadings();
  virtual void LoadScalars();
  virtual void DisplayMeanDifference();
  virtual void ShowSpheres();

  virtual void InterpolateNormalsOn()
  { m_InterpolateNormals = true; }
  virtual void InterpolateNormalsOff()
  { m_InterpolateNormals = false; }
  virtual void SetSurfaceSmoothing()
  {    m_surfSmoother->SetNumberOfIterations(this->smooth_iter->value());  }
  
  virtual void SetGlyphScale();
  virtual void ComputeSurface();
  virtual void RemoveSurface();
  virtual void RemoveGlyphs()
  {
    m_renderer->RemoveActor(m_glyphActor);
    if (m_showingArrowGlyphs==true)
      {
      m_renderer->RemoveActor(m_arrowGlyphActor);
      }
    m_render_window->Render();
  }
  virtual void ShowGlyphs()
  {
    m_renderer->AddActor(m_glyphActor);
    if (m_showingArrowGlyphs==true)
      { m_renderer->AddActor(m_arrowGlyphActor); }
    m_render_window->Render();
  }
  virtual void ChangeColorScheme();
  virtual void HideGroups()
  {
    this->OptionsReconstructionGroup->hide();
    this->OptionsColorSchemeGroup->hide();
    this->OptionsGlyphGroup->hide();
    this->ViewPCAModesGroup->hide();
    this->ViewRegressionGroup->hide();
    this->ViewMeanGroup->hide();
    this->ViewSampleGroup->hide();
  }
  virtual void DisplayMean(unsigned int i)
  {
    if (i==0) this->DisplayShape(m_Stats.Mean());
    //if (i==1) this->DisplayShape(m_Stats.Group1Mean());
    //if (i==2) this->DisplayShape(m_Stats.Group2Mean());

	if (i==1) this->DisplayGroup1Mean();
	if (i==2) this->DisplayGroup2Mean();
  }

  // Split the above into three functions for callback
  virtual void DisplayStatsMean()
  {
	this->DisplayShape(m_Stats.Mean());

	this->m_displayIndicator = DisplayStatsMean_E;
  }

  virtual void DisplayGroup1Mean()
  {
	this->DisplayShape( m_Stats.Group1Mean() );
	this->m_displayIndicator = DisplayGroup1Mean_E;
  }

  virtual void DisplayGroup2Mean()
  {
	this->DisplayShape( m_Stats.Group2Mean() );
	this->m_displayIndicator = DisplayGroup2Mean_E;
  }
  // Add End

  virtual void DisplaySamples()
  {
    this->DisplayShape(m_Stats.ShapeMatrix().get_column(this->sample_selector->value()));
    this->group_display->value(m_Stats.GroupID(this->sample_selector->value()));

	this->m_displayIndicator = DisplaySamples_E;
	
  }

  virtual void DisplayGroupMedian(int i)
  {
    this->sample_selector->value(m_Stats.ComputeMedianShape(i));

	if ( i == 1)
	{
		this->DisplayGroup1Median( );
	} 
	else if ( i == 2)
	{
		this->DisplayGroup2Median( );
	}
	else
	{
    this->DisplaySamples();
		this->m_displayIndicator = DisplayGroupMedian_E;
  }

	
	//this->DisplaySamples();
  }

  // Split the above into two functions for callback

  virtual void DisplayGroup1Median( )
  {
	  this->sample_selector->value( m_Stats.ComputeMedianShape(1) );
	  this->DisplaySamples();	
	  this->m_displayIndicator = DisplayGroup1Median_E;
	  
  }

  virtual void DisplayGroup2Median( )
  {
	  this->sample_selector->value( m_Stats.ComputeMedianShape(2) );
	  this->DisplaySamples();
	  this->m_displayIndicator = DisplayGroup2Median_E;
	  
  }

  // Add end 

  virtual void LoadPCAShapeCheckUpdate(); 

  virtual void ShowCorrespondence()
  {
    this->generate_color_list(1024);
    this->RecolorGlyphs(0,0,0);
  }

  virtual void PointFileDiff();

  // as fltk DOES NOT support member function pointer, have to static first, then static cast.
  static void CheckUpdates( void* );	
  void InitCheck() 
  {  
	  Fl::add_timeout( 5.0, &ShapeWorksViewApp::CheckUpdates, this );
	 // Fl::repeat_timeout( 1.0, &ShapeWorksViewApp::CheckUpdates, this );
  }
  

  //static void callback(void*) {
	 // std::cout << "TICK\n";
	 // Fl::repeat_timeout(1.0, &ShapeWorksViewApp::callback);
  //}

  //void InitCheck() { Fl::add_timeout(1.0, &ShapeWorksViewApp::callback, this); }

  //static void PointFileDiffCallBack( void* obj )
  //{
	 // ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	 // me->PointFileDiff();
  //}

  //static void LoadVectorFieldCallBack( void* obj )
  //{
	 // ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	 // me->LoadVectorField();
  //}

  //static void LoadVectorFieldCallBack( void* obj )
  //{
  // ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
  // me->LoadVectorField();
  //}

  static void LoadPCAShapeCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
      me->LoadPCAShapeCheckUpdate();
  }

  static void DisplayMeanDifferenceCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->DisplayMeanDifference();
  }

  static void DisplayStatsMeanCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->DisplayStatsMean();
  }

  static void DisplayGroup1MeanCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->DisplayGroup1Mean();
  }

  static void DisplayGroup2MeanCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->DisplayGroup2Mean();
  }

  static void DisplaySamplesCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->DisplaySamples();
  }

  static void ComputeRegressionShapeCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->ComputeRegressionShape();
  }

  static void ComputeModeShapeCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->ComputeModeShape();
  }

  static void ComputeGroupMeanDifferenceShapeCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->ComputeGroupMeanDifferenceShape();
  }

  static void DisplayGroup1MedianCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->DisplayGroup1Median();
  }

  static void DisplayGroup2MedianCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->DisplayGroup2Median();
  }

  static void DisplayGroupMedianCallBack( void* obj )
  {
	  ShapeWorksViewApp *me = static_cast<ShapeWorksViewApp *>( obj );
	  me->DisplayGroupMedian(-32);
  }

protected:
 /** Generate a random list of colors.  New glyphs will take on the next color
      in the list. */
  struct rgb
  {
    float r;
    float g;
    float b;
  };
  void generate_color_list(int);
  const std::vector<rgb> &get_color_list() const
  { return m_color_list; }
  void set_color_list(const std::vector<rgb> &c)
  { m_color_list = c; }

  virtual void ReadExplanatoryVariables(const char *fname);

  virtual void ReadSimpleRegressionParameters(const char *fname);

  void DisplayShape( const vnl_vector<double> &);
  void ZeroAllVTK()
  { m_surf = 0; m_surfMap = 0; m_surfActor = 0; m_surfReverse = 0; m_surfContour = 0;  }
  void DisplayVectorField(const std::vector<itk::ParticlePositionReader<3>::PointType > &);
  virtual void LoadVectorField();
  void TrilinearInterpolate(vtkImageData *,double,double, double,
                            vnl_vector_fixed<double,3> &) const;

  
  void InitializeRenderer();
  void InitializeGlyphs();
  void InitializeSurface();
  void RecolorGlyphs(float, float, float);

  void UpdateDifferenceLUT(float, float);

  void ResetPointScalars();
  
  std::string lock_file_; // This file is used to check if updates available.

  // Iteration # is used to indicate that new updates arrive.
  unsigned int iteration_;

  // Display mode indicator
  size_t m_displayIndicator;

  // For loading the PCA shapes, when stats
  unsigned int m_numModes; 
  vnl_vector<double> m_wext;

  //// Enum of callbacks
  enum m_callbackList {

	  LoadPCAShape_E,
	  DisplayMeanDifference_E, 

	  DisplayStatsMean_E,
	  DisplayGroup1Mean_E,	
	  DisplayGroup2Mean_E,

	  DisplaySamples_E,

	  ComputeRegressionShape_E,
	  ComputeModeShape_E,
	  ComputeGroupMeanDifferenceShape_E, // only one group will call this function?

	  DisplayGroup1Median_E, 
	  DisplayGroup2Median_E,

	  DisplayGroupMedian_E,

	  PointFileDiff_E = 98,
	  LoadPointFile_E = 99,
	  LoadVectorField_E = 100
  };


  // Displayer modes decide which function should call
  // This is a single thread application, hence lock and unlock are unnecessary.
  // std::vector< void (*)( void ) > m_callbackPtrVec;	 
  std::vector< void (*)( void* ) > m_callbackPtrVec;
  
  std::string m_CurrentDirectory;

  bool m_showingArrowGlyphs;
  
  // Variables for simple regression
  double m_SimpleRegressionA;
  double m_SimpleRegressionB;
  std::vector<double> m_SimpleRegressionSlopes;
  std::vector<double> m_SimpleRegressionIntercepts;  
  
  std::vector<rgb> m_color_list;
  bool m_Initialized;
  unsigned int m_NumberOfSamples;
  ParticleShapeStatistics<3> m_Stats;
  itk::ParticleShapeLinearRegressionMatrixAttribute<double, 3>::Pointer m_Regression;
  colorSchemes     m_ColorSchemes;
  bool m_InterpolateNormals;

  // VTK Rendering classes
  vtkRenderer     *m_renderer;
  vtkRenderWindow *m_render_window;
  vtkLookupTable  *m_lut;
  vtkColorTransferFunction *m_PValueTFunc;
  vtkColorTransferFunction *m_differenceLUT;
  
  
  // VTK classes for sphere correspondence point glyphs
  vtkGlyph3D        *m_glyphs;
  vtkPolyDataMapper *m_glyphMapper;
  vtkActor          *m_glyphActor;
  vtkPolyData       *m_glyphPointset;
  vtkPoints         *m_glyphPoints;  
  vtkSphereSource   *m_sphereSource;
  vtkUnsignedLongArray *m_scalars;
  vtkTransform      *m_transform180;


  // VTK classes for arrow glyphs
  vtkArrowSource    *m_arrowSource;
  vtkGlyph3D        *m_arrowGlyphs;
  vtkPolyDataMapper *m_arrowGlyphMapper ;
  vtkActor          *m_arrowGlyphActor ;
  vtkTransformPolyDataFilter *m_arrowFlipFilter;  
  
  
  // VTK Surface reconstruction classes
#ifdef SW_USE_POWERCRUST
  vtkPowerCrustSurfaceReconstruction *m_surf;
#else
  vtkSurfaceReconstructionFilter *m_surf;
#endif

  vtkPolyDataMapper              *m_surfMap;
  vtkActor                       *m_surfActor;
  vtkContourFilter               *m_surfContour;
  vtkReverseSense                *m_surfReverse;
  vtkSmoothPolyDataFilter        *m_surfSmoother;
  vtkPolyDataNormals             *m_surfNormals;
  vtkDecimatePro                 *m_surfDecimate;
  
};

#endif
