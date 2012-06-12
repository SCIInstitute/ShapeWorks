/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ShapeWorksShopApp.h,v $
  Date:      $Date: 2011/03/24 01:17:38 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ShapeWorksShopApp_h
#define __ShapeWorksShopApp_h

#ifdef _WIN32
#pragma warning( disable: 4996 )
#endif

#include <cstdio>
#include "itkImage.h"
#include "isosurface_pipeline.h"
#include "glyph_pipeline.h"
#include "plane_widget_pipeline.h"
#include "sphere_widget_pipeline.h"
#include "vtkRenderer.h"
#include "CorrespondenceGUI.h"
#include "vtkCommand.h"
#include "vtkRenderWindow.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkGlyph3D.h"
#include "vtkAxesActor.h"
#include "vtkTubeFilter.h"
#include "vtkAxes.h"
#include "itkMaximumEntropyCorrespondenceSampler.h"
#include "itkCommand.h"
#include "vtkFlRenderWindowInteractor.h"
#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/Fl_Gl_Window.H>
#include <vtkRenderWindowInteractor.h>
#include <vector>
#include "itkParticleProcrustesRegistration.h"
#include "itkParticleSystem.h"
#include "itkParticlePointIndexPair.h"
#include <sstream>
#include <string>
#include <vtkRendererCollection.h>
#include <vtkImageGradient.h>
#include "vtkImageData.h"
#include "vnl/vnl_vector.h"
#include <itkBinaryThresholdImageFilter.h>
#include "vtkImageImport.h"
#include "itkVTKImageExport.h"
#include "vtkDataArray.h"
#include "vtkPolyDataNormals.h"
#include "vtkPointData.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageGradient.h"

template<typename T> std::string toStr(T var) {
std::ostringstream tmp; tmp << var; return tmp.str();
}

class ShapeWorksShopApp : public CorrespondenceGUI
{
 public:
  ShapeWorksShopApp(const char *);
  virtual ~ShapeWorksShopApp();

  typedef itk::Image<float, 3> ImageType;
  virtual void AddSinglePoint();
  virtual void Show()
  {
    m_MainWindow->show();
    
    m_Viewer1->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
    m_Viewer1->show();
    m_Viewer1->Initialize();

    //    m_Viewer2->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
    //    m_Viewer2->show();
    //    m_Viewer2->Initialize();
  }

  virtual void SetMinimumVarianceDecay()
  {
    m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(initial_min_variance_spinner->value(),
                                                                     final_min_variance_spinner->value(),
                                                                     min_variance_decay_iterations->value());
    m_Sampler->GetGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(initial_min_variance_spinner->value(),
                                                                            final_min_variance_spinner->value(),
                                                                            min_variance_decay_iterations->value());
    m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVarianceDecay(initial_min_variance_spinner->value(),
                                                                               final_min_variance_spinner->value(),
                                                                               min_variance_decay_iterations->value());
   

   this->min_variance_spinner->value(initial_min_variance_spinner->value());
    
  }
  virtual void WriteTransformFile() const;
  virtual void ReadTransformFile();
  //  virtual void ReadPrefixTransformFile(const std::string &s);

  virtual void WriteVTKMesh();
  
  virtual void AutoInitialize();
  
  virtual void SetSurfaceOpacity()
  {
    for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
      {      this->m_IsosurfacePipelines[i]->opacity( SurfaceOpacity->value() );      }
    //    m_RenderWindow1->Render();
    //    m_RenderWindow2->Render();
  }

  
  virtual void SplitParticleInAllDomains()
  {
    this->optimize_stop();
    for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
      {
      m_Sampler->GetParticleSystem()->SplitParticle(this->split_radius_spinner->value(),
                                                    (int)(this->selected_particle_spinner->value()),i);
      }
    this->SetParticleCounter();
    this->RecolorGlyphs();
  }
  
  virtual void SplitParticle()
  {
    this->optimize_stop();
    m_Sampler->GetParticleSystem()->SplitParticle(this->split_radius_spinner->value(),
                                                  (int)(this->selected_particle_spinner->value()),
                                                  (int)(this->viewer1_domain_spinner->value()));
    this->SetParticleCounter();

    this->RecolorGlyphs(); 
  }

  void SetParticleCounter()
  {
    unsigned int minparticles = m_Sampler->GetParticleSystem()->GetNumberOfParticles(0);
    for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
      {
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) < minparticles)
        {        minparticles = m_Sampler->GetParticleSystem()->GetNumberOfParticles(i);        }
      }
    this->selected_particle_spinner->maximum(minparticles-1);
  }

	//virtual void GetNormals(std::vector<vtkPolyDataNormals *> &dNormals)
	//{
	//	this->optimize_stop();
	//	
	//	vtkPolyDataNormals *normals = vtkPolyDataNormals::New();
	//	normals->ConsistencyOff();
	//	normals->AutoOrientNormalsOn();
	//	normals->ComputeCellNormalsOff();
	//	normals->ComputePointNormalsOn();
	//	normals->SplittingOff();
	//	normals->ReleaseDataFlagOn();

	//	for (int d = 0; d < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); d++)
	//	{
	//		normals->SetInput(this->m_IsosurfacePipelines[d]->GetIsoSurfaceFilter()->GetOutput());
	//		normals->Update();
	//		dNormals.push_back(normals);
	//	}
	//}

	//virtual void GetNormals(std::vector<vtkImageGradient *> &dNormals)
	//{
	//	this->optimize_stop();

	//	vtkImageGaussianSmooth *smoother = vtkImageGaussianSmooth::New();
	//	smoother->ReleaseDataFlagOn();
	//	//smoother->SetStandardDeviations(1.0,1.0,1.0);

	//	vtkImageGradient *grad = vtkImageGradient::New();
	//	grad->ReleaseDataFlagOn();
	//	grad->SetDimensionality(3);
	//	
	//	for (int d = 0; d < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); d++)
//	{
	//		smoother->SetInputConnection(this->m_IsosurfacePipelines[d]->GetVTKImage());
	//		smoother->Update();
	//		grad->SetInputConnection(smoother->GetOutputPort());
	//		grad->Update();
	//		dNormals.push_back(grad);
	//	}
//}

/*	
	virtual vtkImageData* GetNormals(int d=0)
	{
		this->optimize_stop();

		std::cout << "starting smoothing..." << std::endl;

		vtkImageGaussianSmooth *smoother = vtkImageGaussianSmooth::New();
	smoother->ReleaseDataFlagOn();
		//smoother->SetStandardDeviations(1.0,1.0,1.0);
		smoother->SetInputConnection(this->m_IsosurfacePipelines[d]->GetVTKImage());
		smoother->Update();


	vtkImageGradient *dNormals = vtkImageGradient::New();
		std::cout << "computing normals..." << std::endl;
		//dNormals->ReleaseDataFlagOn();
	dNormals->SetDimensionality(3);
		std::cout << "setting input..." << std::endl;
		dNormals->SetInputConnection(smoother->GetOutputPort());
		std::cout << "updating filter..." << std::endl;
		dNormals->Update();
		return dNormals->GetOutput();				
	}
*/	

  
  virtual void SplitAllParticles()
  {
    this->optimize_stop();

		m_Sampler->GetParticleSystem()->SplitAllParticles(this->split_radius_spinner->value());
    this->SetParticleCounter();
    this->RecolorGlyphs();
  }
  
  virtual void SetGlyphScale()
  {
    for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
      {      this->m_GlyphPipelines[i]->scale(glyph_scale_spinner->value() );      }
    //    m_Viewer1->Render();
    //    m_Viewer2->Render();
  }

  virtual void WriteModes()
  {
    this->SetNumericalParameter();

    if (m_Sampler->GetCorrespondenceMode() == 1)
      {   
      m_Sampler->GetEnsembleEntropyFunction()->WriteModes(pointfile_prefix_input->value(),
                                     (int)(number_of_modes_spinner->value()));
      }
    else if (m_Sampler->GetCorrespondenceMode() == 3)
      {

     m_Sampler->GetEnsembleRegressionEntropyFunction()->WriteModes(pointfile_prefix_input->value(),
                                        (int)(number_of_modes_spinner->value()));
      }
    
  }
  
  virtual void Quit()
  {
    this->optimize_stop();    
    m_MainWindow->hide();
  }

  virtual void ToggleCorrespondence()
  {
    if (m_Sampler->GetCorrespondenceOn() == false)
      {
      m_Sampler->SetCorrespondenceOn();
      if (this->m_use_normal_penalty == true) m_Sampler->SetNormalEnergyOn();
      }
    else
      {
      m_Sampler->SetCorrespondenceOff();
      }      
  }

  virtual void ToggleSampling()
  {
    if (m_Sampler->GetSamplingOn() == false)
      {
      m_Sampler->SetSamplingOn();
      }
    else
      {
      m_Sampler->SetSamplingOff();
      }      
  }

  virtual void ToggleDomainLock()
  {
    int d = (int)(viewer1_domain_spinner->value());
    if ( m_Sampler->GetParticleSystem()->GetDomainFlag(d) == false)
      {
      m_Sampler->GetParticleSystem()->FlagDomain(d);
      }
    else
      {
      m_Sampler->GetParticleSystem()->UnflagDomain(d);
      }
  }
  
 void IterateCallback(itk::Object *, const itk::EventObject &)
  {
  	//m_Procrustes->RunRegistration();
  	//std::cout << "ran procrustes..." << std::endl;
    // Checkpointing
    if (this->checkpointing_toggle->value() != 0)
    {
			if (m_CheckpointCounter > 5)
      {
        m_Procrustes->RunRegistration();

				m_CheckpointCounter = 0;
        this->WritePointFiles();
        this->WriteTransformFile();
      }
      m_CheckpointCounter++;
    }

    this->surface_gradmag->value( m_Sampler->GetLinkingFunction()->GetAverageGradMagA());
    this->correspondence_gradmag->value( m_Sampler->GetLinkingFunction()->GetAverageGradMagB()
                                         * m_Sampler->GetLinkingFunction()->GetRelativeGradientScaling());
    this->surface_energy->value( m_Sampler->GetLinkingFunction()->GetAverageEnergyA());
    this->correspondence_energy->value( m_Sampler->GetLinkingFunction()->GetAverageEnergyB()
                                        * m_Sampler->GetLinkingFunction()->GetRelativeGradientScaling());

    this->iterations_window->value(this->iterations_window->value()+1);


    if (m_Sampler->GetCorrespondenceMode() == 1)
      {    this->min_variance_spinner->value(m_Sampler->GetEnsembleEntropyFunction()->GetMinimumVariance());}
    else if (m_Sampler->GetCorrespondenceMode() ==2)
      {
      this->min_variance_spinner->value(m_Sampler->GetGeneralEntropyGradientFunction()->GetMinimumVariance());
      }
    else if (m_Sampler->GetCorrespondenceMode() ==3)
      {
      this->min_variance_spinner->value(m_Sampler->GetEnsembleRegressionEntropyFunction()->GetMinimumVariance());
      }
  }

  void AddPointCallback(itk::Object *, const itk::EventObject &)
  {
    this->samples_output->value(this->samples_output->value()+1);
  }

  void ToggleConstraints()
  {
    if (this->toggle_constraints_button->value() == 1)
      {    
      for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
        {
        m_Sampler->GetParticleSystem()->GetDomain(i)->DisableConstraints();
        }
      }
    else
      {
      for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
        {
        m_Sampler->GetParticleSystem()->GetDomain(i)->EnableConstraints();
        }
      }
  }
    
  virtual void SetNumericalParameter()
  {
    if (this->use_procrustes_scaling_button->value() == 0)
      {
      //      std::cout << "Procrustes off" << std::endl;
      m_Procrustes->ScalingOff();
      }
    else
      {
      //      std::cout << "Procrustes on" << std::endl;
      m_Procrustes->ScalingOn();      
      }
 
    m_Sampler->GetGradientFunction()->SetFlatCutoff(cutoff_spinner->value());
    m_Sampler->GetQualifierGradientFunction()->SetFlatCutoff(cutoff_spinner->value());
    m_Sampler->GetCurvatureGradientFunction()->SetFlatCutoff(cutoff_spinner->value());
    m_Sampler->GetOmegaGradientFunction()->SetFlatCutoff(cutoff_spinner->value());
    
    m_Sampler->GetGradientFunction()->SetNeighborhoodToSigmaRatio(
                                                                  ratio_spinner->value());
    m_Sampler->GetQualifierGradientFunction()->SetNeighborhoodToSigmaRatio(
                                                   ratio_spinner->value());
    m_Sampler->GetCurvatureGradientFunction()->SetNeighborhoodToSigmaRatio(
                                                                           ratio_spinner->value());
    m_Sampler->GetOmegaGradientFunction()->SetNeighborhoodToSigmaRatio(
                                                                       ratio_spinner->value());
    
    m_Sampler->GetOptimizer()->SetTimeStep(time_step_spinner->value());
    
    
    m_Sampler->GetCurvatureGradientFunction()->SetRho(rho_spinner->value());
    m_Sampler->GetOmegaGradientFunction()->SetRho(rho_spinner->value());
    //    m_Sampler->GetCurvatureGradientFunction()->SetSamplesPerCurvature(s_spinner->value());
    m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(relative_gradient_scaling_spinner->value());
    m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(relative_gradient_scaling_spinner->value());

    m_Sampler->GetLinkingFunction()->SetRelativeNormGradientScaling(norm_penalty_weight_spinner->value());
    m_Sampler->GetLinkingFunction()->SetRelativeNormEnergyScaling(norm_penalty_weight_spinner->value());

    //    m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVariance(min_variance_spinner->value());
    //    m_Sampler->GetGeneralEntropyGradientFunction()->SetMinimumVariance(min_variance_spinner->value());
    //    m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVariance(min_variance_spinner->value());

    dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
      (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())
      ->SetRegressionInterval(regression_interval_spinner->value());


    m_Sampler->GetEnsembleEntropyFunction()
      ->SetRecomputeCovarianceInterval((int)(recompute_covariance_interval_spinner->value()));
    m_Sampler->GetGeneralEntropyGradientFunction()
      ->SetRecomputeCovarianceInterval((int)(recompute_covariance_interval_spinner->value()));
    m_Sampler->GetEnsembleRegressionEntropyFunction()
      ->SetRecomputeCovarianceInterval((int)(recompute_covariance_interval_spinner->value()));

    if (this->hold_min_variance_button->value() == 1)
      {
      m_Sampler->GetEnsembleEntropyFunction()->SetHoldMinimumVariance(true);
     m_Sampler->GetGeneralEntropyGradientFunction()->SetHoldMinimumVariance(true);
      m_Sampler->GetEnsembleRegressionEntropyFunction()->SetHoldMinimumVariance(true);
      }
    else
      {
      m_Sampler->GetEnsembleEntropyFunction()->SetHoldMinimumVariance(false);
      m_Sampler->GetGeneralEntropyGradientFunction()->SetHoldMinimumVariance(false);
      m_Sampler->GetEnsembleRegressionEntropyFunction()->SetHoldMinimumVariance(false);
      }
  }

  virtual void SetOptimizationMode()
  {
    std::cout << "Optim mode = " << optimization_mode_choice->value() << std::endl;
    if (optimization_mode_choice->value() == 1)
      {
      m_Sampler->GetOptimizer()->SetModeToJacobi();
      }
    else if (optimization_mode_choice->value() == 0)
      {
      m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();
      }
    else
      m_Sampler->GetOptimizer()->SetModeToGaussSeidel();
  }
  
  virtual void SetCorrespondenceMode()
  {
    if (m_use_normal_penalty == true) m_Sampler->SetNormalEnergyOn();
    std::cout << "Correspondence mode = " << correspondence_mode_choice->value() << std::endl;
    m_Sampler->SetCorrespondenceMode(correspondence_mode_choice->value());
  }
  
  virtual void SetAdaptivityMode()
  {
    std::cout << "Adaptivity mode = " << adaptivity_mode_choice->value() << std::endl;
    m_Sampler->SetAdaptivityMode(adaptivity_mode_choice->value());
  }

  virtual void VisualizeNeighborhood()
  {
    for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
      {
      m_GlyphPipelines[i]->color_neighborhood((unsigned long int)(this->selected_particle_spinner->value()),
      this->neighborhood_size_spinner->value(), m_Sampler->GetParticleSystem());
      }

    //    m_Viewer1->Render();
    //    m_Viewer2->Render();
  }

  virtual void AutoVisualizeNeighborhood()
  {
    for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
    {
    m_GlyphPipelines[i]->color_neighborhood(
      (unsigned long int)(this->selected_particle_spinner->value()),
       m_Sampler->GetCurvatureGradientFunction()->GetSpatialSigmaCache()
      ->operator[](i)->operator[](this->selected_particle_spinner->value()) * 3.0,
       m_Sampler->GetParticleSystem());
    
    }
  }

  virtual void WriteParameters();
  
  virtual void optimize_start();
  virtual void optimize_stop();
  virtual void single_iteration();

  virtual void WritePointFiles();

  itk::MaximumEntropyCorrespondenceSampler<ImageType> *GetSampler()
  { return m_Sampler.GetPointer(); }
  itk::MemberCommand<ShapeWorksShopApp>::Pointer m_Rendercmd;
  itk::MemberCommand<ShapeWorksShopApp>::Pointer m_Iteratecmd;
  itk::MemberCommand<ShapeWorksShopApp>::Pointer m_AddPointcmd;
  itk::MemberCommand<ShapeWorksShopApp>::Pointer m_UpdateAtIteratecmd;
  virtual void ViewerSelectDomain();

  virtual void RecolorGlyphs()
  {
    for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++)
      {
      m_GlyphPipelines[i]->recolor();
      }

    //    m_Viewer1->Render();
    //    m_Viewer2->Render();
  }

  virtual void RunProcrustes()
  {
    this->optimize_stop();

    m_Procrustes->RunRegistration();
		m_Sampler->Modified();

  	this->RecolorGlyphs();
    
    //    m_Viewer1->Render();
    //    m_Viewer2->Render();
  }
  
protected:
  itk::MaximumEntropyCorrespondenceSampler<ImageType>::Pointer m_Sampler;
  itk::ParticleProcrustesRegistration<3>::Pointer m_Procrustes;

  double m_norm_penalty_weighting;
  bool m_use_normal_penalty;
  
  itk::MultiThreader::Pointer m_Threader;
  int m_ProcessingThread;

  bool m_ShowCuttingPlanes;
  bool m_ShowSpheres;
  int m_SpheresPerDomain;
  
  int m_Viewer1Domain;
  //  int m_Viewer2Domain;
  
  std::vector<glyph_pipeline *> m_GlyphPipelines;
  std::vector<isosurface_pipeline *> m_IsosurfacePipelines;
  std::vector<plane_widget_pipeline *> m_PlaneWidgetPipelines;
  std::vector< std::vector<sphere_widget_pipeline *> > m_SphereWidgetPipelines;
  vtkRenderer *m_Renderer1;
  //  vtkRenderer *m_Renderer2;
  vtkRenderWindow *m_RenderWindow1;
  //  vtkRenderWindow *m_RenderWindow2;
  int m_CheckpointCounter;
  vtkPolyDataMapper *m_axes_mapper;
  vtkActor *m_axes_actor;
  vtkTubeFilter *m_tube_filter;
  vtkAxes *m_axes;

  
  static ITK_THREAD_RETURN_TYPE optimize_callback( void *arg );
  static ITK_THREAD_RETURN_TYPE auto_init_callback( void *arg );  
};


#endif
