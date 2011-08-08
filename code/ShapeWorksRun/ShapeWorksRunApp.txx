/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ShapeWorksRunApp.txx,v $
  Date:      $Date: 2011/03/24 01:17:37 $
  Version:   $Revision: 1.6 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "itkImageFileReader.h"
#include "itkMultiThreader.h"
#include "param.h"
#include "itkMacro.h"
#include "filenameFactory.h"
#include <vector>
#include "object_reader.h"
#include "object_writer.h"
#include "itkZeroCrossingImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

template <class SAMPLERTYPE>
ShapeWorksRunApp<SAMPLERTYPE>::ShapeWorksRunApp(const char *fn)
{
  // Initialize some member variables
  m_CheckpointCounter = 0;
  m_ProcrustesCounter = 0;
  m_disable_procrustes = true;
  m_disable_checkpointing = true;
  m_optimizing = false;
  m_use_regression = false;
  
  // Read parameter file
  param::parameterFile pf(fn);
  this->SetUserParameters(pf);

  // Set up the optimization process
  m_Sampler = itk::MaximumEntropyCorrespondenceSampler<ImageType>::New();  
  m_Sampler->SetDomainsPerShape(m_domains_per_shape); // must be done first!

  // Set up the procrustes registration object.
  m_Procrustes = itk::ParticleProcrustesRegistration<3>::New();
  m_Procrustes->SetParticleSystem(m_Sampler->GetParticleSystem());
  m_Procrustes->SetDomainsPerShape(m_domains_per_shape);
  if (m_procrustes_scaling == 0)
    {
    m_Procrustes->ScalingOff();
    std::cout << "Procrustes scaling is off" << std::endl;
    }
  else
    {
    m_Procrustes->ScalingOn();
    std::cout << "Procrustes scaling is on" << std::endl;
    }

  //   // Read fixed scales if present
  //   std::vector<double> fs;
  //   double stemp;
  //   ok = true;
  //   for (unsigned int i = 0; ok == true; i++)
  //     {
  //     PARAMSET(pf, stemp, "fixed_scales", i, ok, 1.0);
  //     if (ok == true) { fs.push_back(stemp);}
  //     }
  //   if (fs.size() > 0)
  //     {
  //     m_Procrustes->SetFixedScales(fs);
  //     }
  //    ok = true;
  // SCALE ON OR OFF
  
  this->ReadInputs(pf);
  this->SetIterationCommand();
  this->InitializeSampler();
  this->ReadExplanatoryVariables(pf);
  this->FlagDomainFct(pf);
  
  // Now read the transform file if present.
  if ( m_transform_file != "" )       this->ReadTransformFile();
  if ( m_prefix_transform_file != "") this->ReadPrefixTransformFile(m_prefix_transform_file);
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::IterateCallback(itk::Object *, const itk::EventObject &)
{
  std::cout << ".";
  std::cout.flush();

  if (m_optimizing == false) return;
  
  if (m_procrustes_interval != 0 && m_disable_procrustes == false)
    {
    m_ProcrustesCounter++;

    if (m_ProcrustesCounter > (int)m_procrustes_interval)
      {
      m_ProcrustesCounter = 0;
      m_Procrustes->RunRegistration();
      }
    }
  
  static unsigned int iteration_no = 0;
  // Checkpointing
  if (m_checkpointing_interval != 0 && m_disable_checkpointing == false)
    {
    
		m_CheckpointCounter++;
   // if (m_CheckpointCounter > (int)m_checkpointing_interval)
   //   {
   //   m_CheckpointCounter = 0;
   //   this->WritePointFiles();
   //   this->WriteTransformFile();
   //   this->WriteModes();
			//if (m_use_regression == true) this->WriteParameters();
   //   }

		if (m_CheckpointCounter == (int)m_checkpointing_interval)
		{
		  iteration_no += m_checkpointing_interval;
		  m_CheckpointCounter = 0;

		  this->WritePointFiles();
		  this->WriteTransformFile();
		  this->WriteModes();
		  if (m_use_regression == true) this->WriteParameters();

		  if ( m_keep_checkpoints )
		  {
			this->WritePointFiles( iteration_no );
			this->WriteTransformFile( iteration_no );
			if (m_use_regression == true) this->WriteParameters( iteration_no );
		  }
		}
     }

  if ( m_Sampler->GetEnsembleEntropyFunction()->GetMinimumVariance()  <= m_ending_regularization )
    {
    this->optimize_stop();
    };
    
  //    this->surface_gradmag->value( m_Sampler->GetLinkingFunction()->GetAverageGradMagA());
  //    this->correspondence_gradmag->value( m_Sampler->GetLinkingFunction()->GetAverageGradMagB()
  //                                         * m_Sampler->GetLinkingFunction()->GetRelativeGradientScaling());
  //    this->surface_energy->value( m_Sampler->GetLinkingFunction()->GetAverageEnergyA());
  //    this->correspondence_energy->value( m_Sampler->GetLinkingFunction()->GetAverageEnergyB()
  //                                        * m_Sampler->GetLinkingFunction()->GetRelativeEnergyScaling());
  
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::ReadInputs(param::parameterFile &pf)
{
  // Read each input filename and add its image domain
  bool ok = true;
  std::string shape_file;
  std::string point_file;
#ifdef SW_USE_MESH
  std::string mesh_file;
#endif
  std::string attribute_file;

  int i=0;
  while (ok == true)
    {
    // First load the surface image data.
    PARAMSET(pf, shape_file, "inputs", i, ok, "");
    if (i==0 && ok != true)
      {
      std::cerr << "No input files have been specified" << std::endl;
      throw 1;
      }
    if (ok == true)
      {
      std::cout << "Read file " << shape_file << std::endl;
      // Read surface image data.
      typename itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
      reader->SetFileName(shape_file.c_str());
      reader->UpdateLargestPossibleRegion();
      m_Sampler->SetInput(i, reader->GetOutput()); // set the ith input


      // Use the first loaded image to set some numerical constants
      if (i == 0)
        {
        m_spacing = reader->GetOutput()->GetSpacing()[0];
        }

      // Tell the sampler about the corresponding list of points.
      bool ok1 = true;
      PARAMSET(pf, point_file, "point_files", i, ok1, "");
      if (ok1 == true)
        {
        //        std::cout << point_file << std::endl;
        m_Sampler->SetPointsFile(i, point_file);
        }
  

#ifdef SW_USE_MESH
      // Tell the sampler about the corresponding list of points.
      bool ok2 = true;
      PARAMSET(pf, mesh_file, "mesh_files", i, ok2, "");
      if (ok2 == true)
        {
        //        std::cout << point_file << std::endl;
        m_Sampler->SetMeshFile(i, mesh_file);
        }
#endif

      // Tell the sampler about the corresponding list of attributes
      if (m_attributes_per_domain >= 1)
        {
        double sc;
        std::vector<double> attr_scales;
        for (unsigned int kk = 0; kk < m_attributes_per_domain; kk++)
          {          
          PARAMSET(pf, sc, "attribute_scales", 0, ok1, 1.0);
          attr_scales.push_back(sc);
          }
        m_Sampler->SetAttributeScales(attr_scales);std::cout << "e "  << std::endl;

        for (unsigned int kk = 0; kk < m_attributes_per_domain; kk++)
          {
          ok1=true;
          PARAMSET(pf, attribute_file, "attribute_files",
                   i*m_attributes_per_domain + kk, ok1, "");
          if (ok1 == true)
            {
            std::cout << "Reading attribute file: " << attribute_file << std::endl;
            
            // Read attribute file
            typename itk::ImageFileReader<ImageType>::Pointer reader2
              = itk::ImageFileReader<ImageType>::New();
            reader2->SetFileName(attribute_file.c_str());
            reader2->Update();
            m_Sampler->AddAttributeImage(i, reader2->GetOutput());
            }
          else
            {
            std::cerr << "Missing an attribute file for shape file " << shape_file << std::endl;
            }
          }
        } // done read attribute block      



      // Read sphere data if present
      //      bool sph = true;
      bool cp = true;
      double cpa;
      m_spheres_per_domain = 0;
      PARAMSET(pf, m_spheres_per_domain, "spheres_per_domain", 0, cp, 0);
      for (unsigned int j = 0; j < (unsigned int)m_spheres_per_domain; j++)
        {
        vnl_vector_fixed<double,3> vec;
        double rad;
        int idx = i*m_spheres_per_domain*3;
        PARAMSET(pf, rad, "sphere_radii", i*m_spheres_per_domain + j, cp, 0.0);
        std::cout << "domain " << i << " sphere " << j
                  << " radius = " << rad << " center = (";

       
        PARAMSET(pf, cpa, "sphere_centers", idx + j*3 , cp, 0.0);
        std::cout << cpa << ", ";
        vec[0] = cpa;
        
        PARAMSET(pf, cpa, "sphere_centers", idx + j*3 + 1, cp, 0.0);
        std::cout << cpa << ", ";
        vec[1] = cpa;
        
        PARAMSET(pf, cpa, "sphere_centers", idx + j*3 + 2, cp, 0.0);
        std::cout << cpa << ") " << std::endl;
        vec[2] = cpa;        
        
        if (cp == false)
          {
          std::cerr << "WARNING: ERROR READING THE SPHERE INFORMATION" << std::endl;
          }
        m_Sampler->AddSphere(i, vec,rad);
        }

      // Read cutting plane data if present
      cp = true;
      PARAMSET(pf, cpa, "cutting_planes", i * 9, cp, 0.0);
      std::cout << "cp = " << cp << std::endl;
      if (cp == true)
        {
        vnl_vector_fixed<double,3> a,b,c;
       
        a[0] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 1, cp, 0.0);
        if (cp == true) a[1] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 2, cp, 0.0);
        if (cp == true) a[2] = cpa;
        
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 3, cp, 0.0);
        if (cp == true) b[0] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 4, cp, 0.0);
        if (cp == true) b[1] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 5, cp, 1.0);
        if (cp == true) b[2] = cpa;
        
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 6, cp, 1.0);
        if (cp == true) c[0] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 7, cp, 0.0);
        if (cp == true) c[1] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 8, cp, 0.0);
        if (cp == true) c[2] = cpa;
        
        if (cp == false)
          {
          std::cerr << "ERROR: Incomplete cutting plane data!" << std::endl;
          }
        
        std::cout << "CorrespondenceApp-> Setting Cutting Plane "
                  << i << " (" << a << ") (" << b << ") (" << c << ")"<< std::endl;
        
        m_Sampler->SetCuttingPlane(i,a,b,c);
        }
      

      
      } // if ok == true
    i++;
    } // while ok == true


} // end ReadInputs

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::AddSinglePoint()
{
  typedef itk::ParticleSystem<3> ParticleSystemType;
  typedef ParticleSystemType::PointType PointType;
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++)
    {
    typename itk::ZeroCrossingImageFilter<ImageType, ImageType>::Pointer zc =
      itk::ZeroCrossingImageFilter<ImageType, ImageType>::New();
    zc->SetInput( dynamic_cast<itk::ParticleImageDomain<float, 3> *>(
                       m_Sampler->GetParticleSystem()->GetDomain(i))->GetImage());
    zc->Update();
    itk::ImageRegionConstIteratorWithIndex<ImageType> it(zc->GetOutput(),
                                           zc->GetOutput()->GetRequestedRegion());
    bool done = false;
    // for (; !it.IsAtEnd() && done == false; ++it)
    for (it.GoToReverseBegin(); !it.IsAtReverseEnd() && done == false; --it)
      {
      if (it.Get() == 1.0)
        {
        PointType pos;
        dynamic_cast<itk::ParticleImageDomain<float, 3> *>(
                     m_Sampler->GetParticleSystem()->GetDomain(i))
                    ->GetImage()->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
        done = true;
        try
          {
          m_Sampler->GetParticleSystem()->AddPosition(pos, i);
          }
        catch(itk::ExceptionObject &)
          {
          done = false;
          }
        }
      }
    }
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::WriteTransformFile( int iter ) const
{
  std::string output_file = m_output_transform_file;
  
  if( iter >= 0 )
  {
    std::string::size_type idx = m_output_transform_file.rfind('.');
    if (idx == std::string::npos)
    {
	  return;
    }
   
    output_file = m_output_transform_file.substr( 0, idx-1 );
    std::string file_format = m_output_transform_file.substr( idx + 1, m_output_transform_file.length() );
    std::stringstream out;
    out << static_cast< unsigned int >( iter );

    output_file +=  ".it" + out.str() + "." + file_format;
  }
  
  std::vector< itk::ParticleSystem<3>::TransformType > tlist;

  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++)
    {
    tlist.push_back(m_Sampler->GetParticleSystem()->GetTransform(i));
    }

  object_writer< itk::ParticleSystem<3>::TransformType > writer;
  writer.SetFileName(output_file);
  writer.SetInput(tlist);
  writer.Update();
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::ReadTransformFile()
{
  object_reader< itk::ParticleSystem<3>::TransformType > reader;
  reader.SetFileName(m_transform_file);
  reader.Update();
  std::cout << "Read transform file " << m_transform_file << std::endl;
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++)
    {
    // std::cout << "Transform " << i << std::endl << reader.GetOutput()[i] << std::endl;
    m_Sampler->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
    }
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::ReadPrefixTransformFile(const std::string &fn)
{
  object_reader< itk::ParticleSystem<3>::TransformType > reader;
  reader.SetFileName(fn.c_str());
  reader.Update();
  std::cout << "Read prefix transform file " << fn << std::endl;
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++)
    {
    //    std::cout << "Prefix Transform " << i << std::endl << reader.GetOutput()[i] << std::endl;
    m_Sampler->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
    }
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::WritePointFiles( int iter )
{
  typedef  itk::MaximumEntropyCorrespondenceSampler<ImageType>::PointType PointType;
  const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();

  // Write points in both local and global coordinate system
  filenameFactory fn;
  fn.number_of_files(n);
  fn.prefix(m_output_points_prefix);
  fn.file_format("lpts");

  filenameFactory fnw;
  fnw.number_of_files(n);
  fnw.prefix(m_output_points_prefix);
  fnw.file_format("wpts");

  int counter;

  for (int i = 0; i < n; i++)
  {
    counter = 0;
    unsigned int u_iter = static_cast< unsigned int >( iter );
    std::string local_file = iter >= 0 ? fn.filename(i, u_iter) : fn.filename(i);
    std::string world_file = iter >= 0 ? fnw.filename(i, u_iter) : fnw.filename(i);
    
    std::ofstream out( local_file.c_str() );
    std::ofstream outw( world_file.c_str() );

    std::cout << "Writing " << fnw.filename(i) << " ";
    
    if ( !out || !outw )
      { 
      throw param::Exception("EnsembleSystem()::Error opening output file");
      }
    
    for (unsigned int j = 0; j < m_Sampler->GetParticleSystem()->GetNumberOfParticles(i); j++ )
      {
      PointType pos = m_Sampler->GetParticleSystem()->GetPosition(j, i);
      PointType wpos = m_Sampler->GetParticleSystem()->GetTransformedPosition(j, i);
      
      for (unsigned int k = 0; k < 3; k++)
        {        out << pos[k] << " ";        }
      out << std::endl;

      for (unsigned int k = 0; k < 3; k++)
        {        outw << wpos[k] << " ";        }
      outw << std::endl;

	  counter ++;
	  }  // end for points
      
    
    out.close();
    outw.close();
	std::cout << " with " << counter << "points" << std::endl;
  } // end for files
  
  if( iter >= 0 )
  {
    // Write final file signifying that the checkpoints from this iteration are ready for 
    // consumption by an external process.
    std::ostringstream final_file;
	final_file << "iter." << static_cast< unsigned int >( iter );
    std::ofstream final_out( final_file.str().c_str() );
	final_out.close();
  }
} 

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::SetUserParameters(param::parameterFile &pf)
{
  bool ok = true;
  
  // Parameters with defaults
  PARAMSET(pf, m_processing_mode, "processing_mode", 0, ok, 3);
  PARAMSET(pf, m_number_of_particles, "number_of_particles", 0, ok, 1024);
  PARAMSET(pf, m_optimization_iterations, "optimization_iterations", 0, ok, -1);
  PARAMSET(pf, m_output_points_prefix, "output_points_prefix", 0, ok, "output_points");
  PARAMSET(pf, m_output_transform_file, "output_transform_file", 0, ok, "output_transform_file");
  PARAMSET(pf, m_domains_per_shape, "domains_per_shape", 0, ok, 1);
  PARAMSET(pf, m_starting_regularization, "starting_regularization", 0, ok, 500.0);
  PARAMSET(pf, m_ending_regularization, "ending_regularization", 0, ok, 500.0);
  PARAMSET(pf, m_iterations_per_split, "iterations_per_split", 0, ok, 0);
  PARAMSET(pf, m_relative_weighting, "relative_weighting", 0, ok, 1.0);
  PARAMSET(pf, m_norm_penalty_weighting, "norm_penalty_weighting", 0, ok, 0.0);
  PARAMSET(pf, m_adaptivity_strength, "adaptivity_strength", 0, ok, 0.0);
  PARAMSET(pf, m_attributes_per_domain, "attributes_per_domain", 0, ok, 0);
  PARAMSET(pf, m_checkpointing_interval, "checkpointing_interval", 0, ok, 0);
  PARAMSET(pf, m_transform_file, "transform_file", 0, ok, "");
  PARAMSET(pf, m_prefix_transform_file, "prefix_transform_file", 0, ok, "");
  PARAMSET(pf, m_procrustes_interval, "procrustes_interval", 0, ok, 0);
  PARAMSET(pf, m_recompute_regularization_interval, "recompute_regularization_interval", 0, ok, 1);
  PARAMSET(pf, m_procrustes_scaling, "procrustes_scaling", 0, ok, 1);
  PARAMSET(pf, m_adaptivity_mode, "adaptivity_mode", 0, ok, 0);
  PARAMSET(pf, m_keep_checkpoints, "keep_checkpoints", 0, ok, 0);


  // Write out the parameters
  std::cout << "m_processing_mode = " << m_processing_mode << std::endl;
  std::cout << "m_number_of_particles = " << m_number_of_particles << std::endl;
  std::cout << "m_optimization_iterations = " << m_optimization_iterations << std::endl;
  std::cout << "m_output_points_prefix = " << m_output_points_prefix << std::endl;
 std::cout << "m_output_transform_file = " << m_output_transform_file << std::endl;
  std::cout << "m_domains_per_shape = " << m_domains_per_shape << std::endl;
  std::cout << "m_starting_regularization = " << m_starting_regularization << std::endl;
  std::cout << "m_ending_regularization = " << m_ending_regularization << std::endl;
  std::cout << "m_iterations_per_split = " << m_iterations_per_split << std::endl;
  std::cout << "m_relative_weighting = " << m_relative_weighting << std::endl;
  std::cout << "m_norm_penalty_weighting = " << m_norm_penalty_weighting << std::endl;
  std::cout << "m_adaptivity_strength = " << m_adaptivity_strength << std::endl;
  std::cout << "m_attributes_per_domain = " << m_attributes_per_domain << std::endl;
  std::cout << "m_checkpointing_interval = " << m_checkpointing_interval << std::endl;
  std::cout << "m_transform_file = " << m_transform_file << std::endl;
  std::cout << "m_prefix_transform_file = " << m_prefix_transform_file << std::endl;
  std::cout << "m_procrustes_interval = " << m_procrustes_interval << std::endl;
  std::cout << "m_recompute_regularization_interval = " << m_recompute_regularization_interval << std::endl;
  std::cout << "m_procrustes_scaling = " << m_procrustes_scaling << std::endl;
  std::cout << "m_adaptivity_mode = " << m_adaptivity_mode << std::endl;
  std::cout << "m_keep_checkpoints = " << m_keep_checkpoints << std::endl;
}



template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::InitializeSampler()
{  
  m_Sampler->GetGradientFunction()->SetFlatCutoff(0.3);
  m_Sampler->GetCurvatureGradientFunction()->SetFlatCutoff(0.3);
 m_Sampler->GetGradientFunction()->SetNeighborhoodToSigmaRatio(3.0);
  m_Sampler->GetCurvatureGradientFunction()->SetNeighborhoodToSigmaRatio(3.0);
  m_Sampler->GetQualifierGradientFunction()->SetFlatCutoff(0.3);
  m_Sampler->GetQualifierGradientFunction()->SetNeighborhoodToSigmaRatio(3.0);

 m_Sampler->GetOmegaGradientFunction()->SetFlatCutoff(0.3);
  m_Sampler->GetOmegaGradientFunction()->SetNeighborhoodToSigmaRatio(3.0);

  
  m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetGeneralEntropyGradientFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetGeneralEntropyGradientFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetGeneralEntropyGradientFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetOptimizer()->SetTimeStep(1.0);
  m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();
  //  m_Sampler->GetOptimizer()->SetModeToJacobi();
  
  m_Sampler->SetCorrespondenceOn();
  m_Sampler->SetSamplingOn();
  m_Sampler->SetAdaptivityMode(m_adaptivity_mode);
  m_Sampler->GetEnsembleEntropyFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetGeneralEntropyGradientFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetEnsembleRegressionEntropyFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  
  m_Sampler->Initialize();
  m_Sampler->GetOptimizer()->SetTolerance(0.0);
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::Initialize()
{
  m_disable_checkpointing = true;
  m_disable_procrustes = true;
  m_Sampler->GetCurvatureGradientFunction()->SetRho(0.0);
  m_Sampler->GetOmegaGradientFunction()->SetRho(0.0);
  m_Sampler->SetCorrespondenceOn();
  m_Sampler->SetCorrespondenceMode(0);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(0.050);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(0.050);
  m_Sampler->GetLinkingFunction()->SetRelativeNormGradientScaling(0.0);
  m_Sampler->GetLinkingFunction()->SetRelativeNormEnergyScaling(0.0);

  if (m_Sampler->GetParticleSystem()->GetNumberOfParticles() < m_number_of_particles)
    {  this->AddSinglePoint(); }

  while (m_Sampler->GetParticleSystem()->GetNumberOfParticles() < m_number_of_particles)
    {
    this->SplitAllParticles();
    std::cout << std::endl << "Particle count: "
              << m_Sampler->GetParticleSystem()->GetNumberOfParticles() << std::endl;
    
    m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
    m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
    m_Sampler->Modified();
    m_Sampler->Update();
    }

  this->WritePointFiles();
  this->WriteTransformFile();
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::AddAdaptivity()
{

  if (m_adaptivity_strength == 0.0) return;
  m_disable_checkpointing = true;
  m_disable_procrustes = true;
  std::cout << "Adding adaptivity." << std::endl;
  m_Sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(0.05);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(0.05);
  m_Sampler->GetLinkingFunction()->SetRelativeNormGradientScaling(0.0);
  m_Sampler->GetLinkingFunction()->SetRelativeNormEnergyScaling(0.0);

  //  if (tmpNoAdaptivityFlag == true) return;
  
  m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
  m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_Sampler->Modified();
  m_Sampler->Update();
  
  this->WritePointFiles();
  this->WriteTransformFile();
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::Optimize()
{
  m_optimizing = true;
  m_Sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeNormGradientScaling(m_norm_penalty_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeNormEnergyScaling(m_norm_penalty_weighting);

  m_disable_checkpointing = false;
  m_disable_procrustes = false;

  if (m_procrustes_interval != 0) // Initial registration
    {      m_Procrustes->RunRegistration();      }
  
  //  m_Sampler->GetOptimizer()->SetModeToGaussSeidel();
  m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();

  // Set up the minimum variance decay
  m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                   m_ending_regularization,
                                                                   m_optimization_iterations);
  m_Sampler->GetGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                          m_ending_regularization,
                                                                          m_optimization_iterations);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                             m_ending_regularization,
                                                                             m_optimization_iterations);
  
  std::cout << "Optimizing correspondences." << std::endl;
  if (m_attributes_per_domain > 0)
    {
    m_Sampler->SetCorrespondenceMode(2); // General entropy
    }
  else if (m_use_regression == true)
    {
    m_Sampler->SetCorrespondenceMode(3); // Regression
    }
  else if (m_starting_regularization == m_ending_regularization)
    {
      m_Sampler->SetCorrespondenceMode(0); // mean force
    }
  else
    {
    m_Sampler->SetCorrespondenceMode(1); // Normal
    }
                                                        
  if (m_optimization_iterations > 0)
    m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_optimization_iterations);
  else m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(0);

  
  m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_Sampler->GetOptimizer()->SetTolerance(0.0);
  m_Sampler->Modified();
  m_Sampler->Update();
  
  this->WritePointFiles();
  this->WriteTransformFile();
  this->WriteModes();
	if (m_use_regression == true) this->WriteParameters();
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp<SAMPLERTYPE>::ReadExplanatoryVariables(param::parameterFile &pf)
{
 // Read explanatory variables if present
  std::vector<double> evars;
  double etmp;
  bool ok = true;
  for (unsigned int i = 0; ok == true; i++)
    {
    PARAMSET(pf, etmp, "explanatory_variable", i, ok, 1.0);
    if (ok == true)
      {
        m_use_regression = true;
        evars.push_back(etmp);
      }
    }
  
  dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
    (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())
    ->SetExplanatory(evars);
  
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::optimize_start()
{  m_Sampler->GetOptimizer()->StartOptimization();}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::optimize_stop()
{  m_Sampler->GetOptimizer()->StopOptimization();}

template < class SAMPLERTYPE>
ShapeWorksRunApp<SAMPLERTYPE>::~ShapeWorksRunApp()
{
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::WriteParameters( int iter )
{
  std::string slopename, interceptname;
  
  if( iter >= 0 )
  {
    std::stringstream out_it;
    out_it << static_cast< unsigned int >( iter );

    slopename = std::string( m_output_points_prefix ) + ".it" + out_it.str() + std::string(".slope");
    interceptname = std::string( m_output_points_prefix ) + ".it" + out_it.str() + std::string(".intercept");
  }
  else
  {
    slopename = std::string( m_output_points_prefix ) + std::string(".slope");
    interceptname = std::string( m_output_points_prefix ) + std::string(".intercept");
  }
  
  std::cout << "writing " << slopename << std::endl;
  std::cout << "writing " << interceptname << std::endl;

  std::vector< double > slope;
  vnl_vector<double> slopevec = dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
      (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())->GetSlope();

  for (unsigned int i = 0; i < slopevec.size(); i++)
    {    slope.push_back(slopevec[i]);    }

  std::ofstream out( slopename.c_str() );
  for (unsigned int i = 0; i < slope.size(); i++)
  {
    out << slope[i] << "\n";
  }
  out.close();
   
  //object_writer< double > writer;
  //writer.SetFileName(slopename);
  //writer.SetInput(slope);
  //writer.Update();
  
  std::vector< double > intercept;
  vnl_vector<double> interceptvec = dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
    (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())->GetIntercept();
  
  for (unsigned int i = 0; i < slopevec.size(); i++)
    {    intercept.push_back(interceptvec[i]);    }

  out.open(interceptname.c_str());
  for (unsigned int i = 0; i < slope.size(); i++)
    {
    out << intercept[i] << "\n";
    }
  out.close();
  
  //object_writer< double > writer2;
  //writer2.SetFileName(interceptname);
  //writer2.SetInput(intercept);
  //writer2.Update();

}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::FlagDomainFct(param::parameterFile &pf)
{


 // ET UP ANY FIXED LANDMARK POSITIOSNS
  bool ok1 = true;
  for (unsigned int i = 0; ok1 == true; i++)
    {
    unsigned int f;
    PARAMSET(pf, f, "fixed_landmarks", i, ok1, 0);

    if (ok1 == true) m_Sampler->GetParticleSystem()->SetFixedParticleFlag(f);
    }

   // SET UP ANY FIXED DOMAINS
   ok1 = true;
   for (unsigned int i = 0; ok1 == true; i++)
     {
       unsigned int f;
       PARAMSET(pf, f, "fixed_domains", i, ok1, 0);
       if (ok1 == true) 
	 {
	   if (f >0.0){m_Sampler->GetParticleSystem()->FlagDomain(i);
     			 }
  	
	 }
     }
}

