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
#include "tinyxml.h"
#include <sstream>
#include <string>
#include <iostream>
#include "itkMacro.h"
#include "filenameFactory.h"
#include <vector>
#include "object_reader.h"
#include "object_writer.h"
#include "itkZeroCrossingImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#ifdef _WIN32
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

template <class SAMPLERTYPE>
ShapeWorksRunApp<SAMPLERTYPE>::ShapeWorksRunApp(const char *fn)
{
  // Initialize some member variables
  m_CheckpointCounter = 0;
  m_ProcrustesCounter = 0;
  m_disable_procrustes = true;
  m_disable_checkpointing = true;
  m_optimizing = false;
  m_use_normal_penalty = false;
  m_use_initial_normal_penalty = false;
  m_use_regression = false;
  m_use_mixed_effects = false;
  
  // Read parameter file
  this->SetUserParameters(fn);

  // Set up the optimization process
  m_Sampler = itk::MaximumEntropyCorrespondenceSampler<ImageType>::New();  
  m_Sampler->SetDomainsPerShape(m_domains_per_shape); // must be done first!
  m_Sampler->SetTimeptsPerIndividual(m_timepts_per_subject);

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
  
  //this->ReadInputs(pf);
  this->ReadInputs(fn);
  this->SetIterationCommand();
  this->InitializeSampler();
  this->ReadExplanatoryVariables(fn);
  this->FlagDomainFct(fn);
  
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

    if (m_ProcrustesCounter >= (int)m_procrustes_interval)
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
			  std::stringstream ss;
			  ss << iteration_no + m_optimization_iterations_completed;
			  std::string dir_name = "iter" + ss.str();
			  std::string tmp_dir_name = std::string(".") + dir_name;

#ifdef _WIN32
			  mkdir( tmp_dir_name.c_str() );
#else
			  mkdir( tmp_dir_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
#endif
			this->WritePointFiles( iteration_no );
			this->WriteTransformFile( iteration_no );
			/*if (m_use_regression == true) */this->WriteParameters( iteration_no );

			rename( tmp_dir_name.c_str(), dir_name.c_str() );
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
ShapeWorksRunApp<SAMPLERTYPE>::ReadInputs(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    std::istringstream inputsBuffer;
    std::string filename;
    int numShapes = 0;

    // load input shapes
    std::vector<std::string> shapeFiles;
    elem = docHandle.FirstChild( "inputs" ).Element();
    if (!elem)
    {
      std::cerr << "No input files have been specified" << std::endl;
      throw 1;
    }
    else
    {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> filename)
      {
        shapeFiles.push_back(filename);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      numShapes = shapeFiles.size();
	  
	  // try to fix the parameter file's input data names to match the parameter file's path
	  std::string pname(fname);
	  std::string path = pname.substr(0,pname.find_last_of("/") + 1);
	  std::ifstream test(shapeFiles[0].c_str());
	  if (!test.is_open()) {
		  for (int i = 0; i < shapeFiles.size(); i++) {
			  shapeFiles[i] = path + shapeFiles[i];
		  }
	  } else test.close();

      for (int shapeCount = 0; shapeCount < numShapes; shapeCount++)
      {
        typename itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
        reader->SetFileName(shapeFiles[shapeCount].c_str());
        reader->UpdateLargestPossibleRegion();
        m_Sampler->SetInput(shapeCount, reader->GetOutput()); // set the ith input

        // Use the first loaded image to set some numerical constants
        if (shapeCount == 0)
        {
          m_spacing = reader->GetOutput()->GetSpacing()[0];
        }
      }

      shapeFiles.clear();
    }

    // load point files
    std::vector<std::string> pointFiles;
    elem = docHandle.FirstChild( "point_files" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> filename)
      {
        pointFiles.push_back(filename);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      // read point files only if they are all present
      if (pointFiles.size() < numShapes)
      {
        std::cerr << "not enough point files, none will be loaded" << std::endl;
      }
      else
      {
        for (int shapeCount = 0; shapeCount < numShapes; shapeCount++)
        {
          m_Sampler->SetPointsFile(shapeCount, pointFiles[shapeCount]);
        }
      }

      pointFiles.clear();
    }   


#ifdef SW_USE_MESH
    // load mesh files
    std::vector<std::string> meshFiles;
    elem = docHandle.FirstChild( "mesh_files" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> filename)
      {
        meshFiles.push_back(filename);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      // read mesh files only if they are all present
      if (meshFiles.size() < numShapes)
      {
        std::cerr << "not enough mesh files, none will be loaded" << std::endl;
      }
      else
      {
        for (int shapeCount = 0; shapeCount < numShapes; shapeCount++)
        {
          m_Sampler->SetMeshFile(shapeCount, meshFiles[shapeCount]);
        }
      }

      meshFiles.clear();
    }


#endif

    // read geometric constraints, if present
    // cutting planes
    elem = docHandle.FirstChild( "cutting_planes" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());

      std::vector<double> cpVals;
      double pt;

      while (inputsBuffer >> pt)
      {
        cpVals.push_back(pt);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      if (cpVals.size() < 9*numShapes)
      {
        std::cerr << "ERROR: Incomplete cutting plane data! No cutting planes will be loaded!!" << std::endl;
      }
      else
      {
        vnl_vector_fixed<double,3> a,b,c;
        int ctr = 0;

        for (int shapeCount = 0; shapeCount < numShapes; shapeCount++)
        {
          a[0] = cpVals[ctr++];
          a[1] = cpVals[ctr++];
          a[2] = cpVals[ctr++];

          b[0] = cpVals[ctr++];
          b[1] = cpVals[ctr++];
          b[2] = cpVals[ctr++];

          c[0] = cpVals[ctr++];
          c[1] = cpVals[ctr++];
          c[2] = cpVals[ctr++];

          std::cout << "CorrespondenceApp-> Setting Cutting Plane "
                    << shapeCount << " (" << a << ") (" << b << ") (" << c << ")"<< std::endl;
          
          m_Sampler->SetCuttingPlane(shapeCount,a,b,c);
        }
      }
    }

    // sphere radii and centers
    this->m_spheres_per_domain = 0;
    elem = docHandle.FirstChild( "spheres_per_domain" ).Element();
    if (elem) this->m_spheres_per_domain = atoi(elem->GetText());

    int numSpheres = numShapes * this->m_spheres_per_domain;
    std::vector<double> radList;
    double r;

    elem = docHandle.FirstChild( "sphere_radii" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());

      while (inputsBuffer >> r)
      {
        radList.push_back(r);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      if (radList.size() < numSpheres)
      {
        std::cerr << "ERROR: Incomplete sphere radius data! No spheres will be loaded!!" << std::endl;
      }
      else
      {
        elem = docHandle.FirstChild( "sphere_centers" ).Element();
        if (elem)
        {
          inputsBuffer.str(elem->GetText());

          std::vector<double> spVals;
          double pt;

          while (inputsBuffer >> pt)
          {
            spVals.push_back(pt);
          }
          inputsBuffer.clear();
          inputsBuffer.str("");

          if (spVals.size() < 3*numSpheres)
          {
            std::cerr << "ERROR: Incomplete sphere center data! No spheres will be loaded!!" << std::endl;
          }
          else
          {
            vnl_vector_fixed<double,3> center;
            double rad;
            int c_ctr = 0;
            int r_ctr = 0;

            for (int shapeCount = 0; shapeCount < numShapes; shapeCount++)
            {
              for (int sphereCount = 0; sphereCount < m_spheres_per_domain; sphereCount++)
              {
                center[0] = spVals[c_ctr++];
                center[1] = spVals[c_ctr++];
                center[2] = spVals[c_ctr++];

                rad = radList[r_ctr++];

                m_Sampler->AddSphere(shapeCount,center,rad);
              }
            }
          }
        }
      }
    }

    // attributes
    if (this->m_attributes_per_domain >= 1)
    {
      // attribute scales
      double sc;
      std::vector<double> attr_scales;

      elem = docHandle.FirstChild( "attribute_scales" ).Element();
      if (elem)
      {
        inputsBuffer.str(elem->GetText());

        while (inputsBuffer >> sc)
        {
          attr_scales.push_back(sc);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
      }

      // attribute files
      std::vector<std::string> attrFiles;
      elem = docHandle.FirstChild( "attribute_files" ).Element();
      if (elem)
      {
        inputsBuffer.str(elem->GetText());

        while (inputsBuffer >> filename)
        {
          attrFiles.push_back(filename);
        }

        inputsBuffer.clear();
        inputsBuffer.str("");

        if ( (attr_scales.size() < m_attributes_per_domain) || (attrFiles.size() < numShapes*m_attributes_per_domain) )
        {
          std::cerr << "ERROR: Incomplete attribute scales or filenames ! No attributes will be loaded!!" << std::endl;
        }
        else
        {
          m_Sampler->SetAttributeScales(attr_scales);

          int ctr = 0;

          for (int shapeCount = 0; shapeCount < numShapes; shapeCount++)
          {
            for (int attrCount = 0; attrCount < m_attributes_per_domain; attrCount++)
            {
              typename itk::ImageFileReader<ImageType>::Pointer reader2 = itk::ImageFileReader<ImageType>::New();
              reader2->SetFileName(attrFiles[ctr++].c_str());
              reader2->Update();
              m_Sampler->AddAttributeImage(shapeCount, reader2->GetOutput());
            }
          }
        }
      }
    }

  } // end: document check
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

    bool done = false;

    // first attempt to find the surface moving from the center out in the y direction
    ImageType::Pointer img = dynamic_cast<itk::ParticleImageDomain<float, 3> *>(
      m_Sampler->GetParticleSystem()->GetDomain(i))->GetImage();
    ImageType::IndexType center;
    center[0] = img->GetLargestPossibleRegion().GetSize()[0] / 2;
    center[1] = img->GetLargestPossibleRegion().GetSize()[1] / 2;
    center[2] = img->GetLargestPossibleRegion().GetSize()[2] / 2;

    while ( !done && center[1] > 0 )
    {
      if ( img->GetPixel(center) < 1.0 && img->GetPixel( center ) > -1.0 )
      {
        PointType pos;
        img->TransformIndexToPhysicalPoint( center, pos );
        m_Sampler->GetParticleSystem()->AddPosition( pos, i );
        done = true;
      }
      center[1]--;
    }

    // couldn't find it, try the old method
    itk::ZeroCrossingImageFilter<ImageType, ImageType>::Pointer zc =
      itk::ZeroCrossingImageFilter<ImageType, ImageType>::New();
    zc->SetInput( img );
    zc->Update();
    itk::ImageRegionConstIteratorWithIndex<ImageType> it(zc->GetOutput(),
      zc->GetOutput()->GetRequestedRegion());
    //for (; !it.IsAtEnd() && done == false; ++it)
    for (it.GoToReverseBegin(); !it.IsAtReverseEnd() && done == false; --it)
    {
      if (it.Get() == 1.0)
      {
        PointType pos;
        img->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
        done = true;
        try
        {
          m_Sampler->GetParticleSystem()->AddPosition( pos, i );
        }
        catch ( itk::ExceptionObject & )
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
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
	output_file = "./.iter" + ss.str() + "/" + output_file;
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

  std::stringstream ss;
  ss << iter+m_optimization_iterations_completed;

  //// to suffix with # of points
  //std::stringstream points_ss;
  //points_ss << m_Sampler->GetParticleSystem()->GetNumberOfParticles(0);

  int counter;

  std::cout << "\n";
  for (int i = 0; i < n; i++)
  {
    counter = 0;
    unsigned int u_iter = static_cast< unsigned int >( iter );
    std::string local_file = iter >= 0 ? "./.iter" + ss.str() + "/" + fn.filename(i) : fn.filename(i);
    std::string world_file = iter >= 0 ? "./.iter" + ss.str() + "/" + fnw.filename(i) : fnw.filename(i);
    //// to suffix with # of points
    //std::string local_file = iter >= 0 ? "./.iter" + ss.str() + "/" + fn.filename(i) : fn.filename(i) + "." + points_ss.str();
    //std::string world_file = iter >= 0 ? "./.iter" + ss.str() + "/" + fnw.filename(i) : fnw.filename(i) + "." + points_ss.str();
    
    std::ofstream out( local_file.c_str() );
    std::ofstream outw( world_file.c_str() );

    std::cout << "Writing " << world_file << " ";
    
    if ( !out || !outw )
      {
        std::cerr << "EnsembleSystem()::Error opening output file" << std::endl;
        throw 1;
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
    std::cout << " with " << counter << " points" << std::endl;
  } // end for files
  }

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::SetUserParameters(const char *fname)
{

  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    // read values from parameter file: 1. set default value, 2. try to read XML tag, 3. if present, set new value
    this->m_processing_mode = 3;
    elem = docHandle.FirstChild( "processing_mode" ).Element();
    if (elem) this->m_processing_mode = atoi(elem->GetText());

    this->m_number_of_particles = 1024;
    elem = docHandle.FirstChild( "number_of_particles" ).Element();
    if (elem) this->m_number_of_particles = atoi(elem->GetText());

    // Parameters with defaults
    this->m_optimization_iterations = -1;
    elem = docHandle.FirstChild( "optimization_iterations" ).Element();
    if (elem) this->m_optimization_iterations = atoi(elem->GetText());
      
	this->m_optimization_iterations_completed = 0;
	elem = docHandle.FirstChild( "optimization_iterations_completed" ).Element();
	if (elem) this->m_optimization_iterations_completed = atoi(elem->GetText());

    this->m_output_points_prefix = "output_points";
    elem = docHandle.FirstChild( "output_points_prefix" ).Element();
    if (elem) this->m_output_points_prefix = elem->GetText();

    this->m_output_transform_file = "output_transform_file";
    elem = docHandle.FirstChild( "output_transform_file" ).Element();
    if (elem) this->m_output_transform_file = elem->GetText();

    this->m_domains_per_shape = 1;
    elem = docHandle.FirstChild( "domains_per_shape" ).Element();
    if (elem) this->m_domains_per_shape = atoi(elem->GetText());

    this->m_timepts_per_subject = 1;
    elem = docHandle.FirstChild( "timepts_per_subject" ).Element();
    if (elem) this->m_timepts_per_subject = atoi(elem->GetText());

    this->m_starting_regularization = 500.0;
    elem = docHandle.FirstChild( "starting_regularization" ).Element();
    if (elem) this->m_starting_regularization = atof(elem->GetText());

    this->m_ending_regularization = 500.0;
    elem = docHandle.FirstChild( "ending_regularization" ).Element();
    if (elem) this->m_ending_regularization = atof(elem->GetText());

    this->m_iterations_per_split = 0;
    elem = docHandle.FirstChild( "iterations_per_split" ).Element();
    if (elem) this->m_iterations_per_split = atoi(elem->GetText());

    this->m_relative_weighting = 1.0;
    elem = docHandle.FirstChild( "relative_weighting" ).Element();
    if (elem) this->m_relative_weighting = atof(elem->GetText());

    this->m_norm_penalty_weighting = 0.0;
    elem = docHandle.FirstChild( "norm_penalty_weighting" ).Element();
    if (elem)
    {
      this->m_norm_penalty_weighting = atof(elem->GetText());
      if (this->m_norm_penalty_weighting > 0.0)
      {
        this->m_use_normal_penalty = true;
      }
      else
      {
        this->m_use_normal_penalty = false;
      }
    }

    //this->m_initial_relative_weighting = 0.05;
    this->m_initial_relative_weighting = 20; // shireen - sampling is now the weighted part we increase it during init
    elem = docHandle.FirstChild( "initial_relative_weighting" ).Element();
    if (elem) this->m_initial_relative_weighting = atof(elem->GetText());

    this->m_initial_norm_penalty_weighting = 0.0;
    elem = docHandle.FirstChild( "initial_norm_penalty_weighting" ).Element();
    if (elem)
    {
      this->m_initial_norm_penalty_weighting = atof(elem->GetText());
      if (this->m_initial_norm_penalty_weighting > 0.0)
      {
        this->m_use_initial_normal_penalty = true;
      }
      else
      {
        this->m_use_initial_normal_penalty = false;
      }
    }

    this->m_adaptivity_strength = 0.0;
    elem = docHandle.FirstChild( "adaptivity_strength" ).Element();
    if (elem) this->m_adaptivity_strength = atof(elem->GetText());

    this->m_attributes_per_domain = 0;
    elem = docHandle.FirstChild( "attributes_per_domain" ).Element();
    if (elem) this->m_attributes_per_domain = atoi(elem->GetText());

    this->m_checkpointing_interval = 0;
    elem = docHandle.FirstChild( "checkpointing_interval" ).Element();
    if (elem) this->m_checkpointing_interval = atoi(elem->GetText());

    this->m_transform_file = "";
    elem = docHandle.FirstChild( "transform_file" ).Element();
    if (elem) this->m_transform_file = elem->GetText();

    this->m_prefix_transform_file = "";
    elem = docHandle.FirstChild( "prefix_transform_file" ).Element();
    if (elem) this->m_prefix_transform_file = elem->GetText();

    this->m_procrustes_interval = 0;
    elem = docHandle.FirstChild( "procrustes_interval" ).Element();
    if (elem) this->m_procrustes_interval = atoi(elem->GetText());

    this->m_recompute_regularization_interval = 1;
    elem = docHandle.FirstChild( "recompute_regularization_interval" ).Element();
    if (elem) this->m_recompute_regularization_interval = atoi(elem->GetText());

    this->m_procrustes_scaling = 1;
    elem = docHandle.FirstChild( "procrustes_scaling" ).Element();
    if (elem) this->m_procrustes_scaling = atoi(elem->GetText());

    this->m_adaptivity_mode = 0;
    elem = docHandle.FirstChild( "adaptivity_mode" ).Element();
    if (elem) this->m_adaptivity_mode = atoi(elem->GetText());

    this->m_keep_checkpoints = 0;
    elem = docHandle.FirstChild( "keep_checkpoints" ).Element();
    if (elem) this->m_keep_checkpoints = atoi(elem->GetText());
  }

  // Write out the parameters
  std::cout << "m_processing_mode = " << m_processing_mode << std::endl;
  std::cout << "m_number_of_particles = " << m_number_of_particles << std::endl;
  std::cout << "m_optimization_iterations = " << m_optimization_iterations << std::endl;
  std::cout << "m_output_points_prefix = " << m_output_points_prefix << std::endl;
  std::cout << "m_output_transform_file = " << m_output_transform_file << std::endl;
  std::cout << "m_domains_per_shape = " << m_domains_per_shape << std::endl;
  std::cout << "m_timepts_per_subject = " << m_timepts_per_subject << std::endl;
  std::cout << "m_starting_regularization = " << m_starting_regularization << std::endl;
  std::cout << "m_ending_regularization = " << m_ending_regularization << std::endl;
  std::cout << "m_iterations_per_split = " << m_iterations_per_split << std::endl;
  std::cout << "m_relative_weighting = " << m_relative_weighting << std::endl;
  std::cout << "m_norm_penalty_weighting = " << m_norm_penalty_weighting << std::endl;
  std::cout << "m_initial_relative_weighting = " << m_initial_relative_weighting << std::endl;
  std::cout << "m_initial_norm_penalty_weighting = " << m_initial_norm_penalty_weighting << std::endl;
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
  std::cout << "m_optimization_iterations_completed = " << m_optimization_iterations_completed << std::endl;

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

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetOptimizer()->SetTimeStep(1.0);
  m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();
  //  m_Sampler->GetOptimizer()->SetModeToJacobi();
  
  m_Sampler->SetSamplingOn();
  m_Sampler->SetCorrespondenceOn();
  if (this->m_use_initial_normal_penalty == true) m_Sampler->SetNormalEnergyOn();
  m_Sampler->SetAdaptivityMode(m_adaptivity_mode);
  m_Sampler->GetEnsembleEntropyFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetGeneralEntropyGradientFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetEnsembleRegressionEntropyFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  
  m_Sampler->Initialize();
  m_Sampler->GetOptimizer()->SetTolerance(0.0);
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::Initialize()
{
  std::cerr << "------------------------------\n";
  std::cerr << "*** Initialize Step\n";
  std::cerr << "------------------------------\n";

  m_disable_checkpointing = true;
  m_disable_procrustes = true;
  m_Sampler->GetCurvatureGradientFunction()->SetRho(0.0);
  m_Sampler->GetOmegaGradientFunction()->SetRho(0.0);
  m_Sampler->SetCorrespondenceOn();
  if (this->m_use_initial_normal_penalty == true) m_Sampler->SetNormalEnergyOn();
  m_Sampler->SetCorrespondenceMode(0);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeNormGradientScaling(m_initial_norm_penalty_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeNormEnergyScaling(m_initial_norm_penalty_weighting);

  //if (m_Sampler->GetParticleSystem()->GetNumberOfParticles() < m_number_of_particles)
  //  {  this->AddSinglePoint(); }

  if (m_Sampler->GetParticleSystem()->GetNumberOfParticles() < 1)
  {
    this->AddSinglePoint();
  }

  while (m_Sampler->GetParticleSystem()->GetNumberOfParticles() < m_number_of_particles)
  {
    this->SplitAllParticles();
    std::cout << std::endl << "Particle count: "
              << m_Sampler->GetParticleSystem()->GetNumberOfParticles() << std::endl;
    
    m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
    m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
    m_Sampler->Modified();
    m_Sampler->Update();

    this->WritePointFiles();
    this->WriteTransformFile();
  }
  this->WritePointFiles();
  this->WriteTransformFile();
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::AddAdaptivity()
{
  std::cerr << "------------------------------\n";
  std::cerr << "*** AddAdaptivity Step\n";
  std::cerr << "------------------------------\n";

  if (m_adaptivity_strength == 0.0) return;
  m_disable_checkpointing = true;
  m_disable_procrustes = true;
  std::cout << "Adding adaptivity." << std::endl;
  m_Sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeNormGradientScaling(m_initial_norm_penalty_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeNormEnergyScaling(m_initial_norm_penalty_weighting);

  //m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(0.05);
  //m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(0.05);
  //m_Sampler->GetLinkingFunction()->SetRelativeNormGradientScaling(0.0);
  //m_Sampler->GetLinkingFunction()->SetRelativeNormEnergyScaling(0.0);

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
  std::cerr << "------------------------------\n";
  std::cerr << "*** Optimize Step\n";
  std::cerr << "------------------------------\n";

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
  {
    m_Procrustes->RunRegistration();
    this->WritePointFiles();
    this->WriteTransformFile();
  }
  
  //  m_Sampler->GetOptimizer()->SetModeToGaussSeidel();
  m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();

  // Set up the minimum variance decay
  m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                   m_ending_regularization,
                                                                   m_optimization_iterations-
																                                   m_optimization_iterations_completed);
  m_Sampler->GetGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                          m_ending_regularization,
                                                                          m_optimization_iterations-
																		                                      m_optimization_iterations_completed);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                             m_ending_regularization,
                                                                             m_optimization_iterations-
																			                                       m_optimization_iterations_completed);

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                               m_ending_regularization,
                                                                               m_optimization_iterations-
																			                                         m_optimization_iterations_completed);

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                             m_ending_regularization,
                                                                             m_optimization_iterations-
																			                                       m_optimization_iterations_completed);
  
  std::cout << "Optimizing correspondences." << std::endl;

  if (m_use_normal_penalty == true) m_Sampler->SetNormalEnergyOn();
    
  if (m_attributes_per_domain > 0)
  {
    m_Sampler->SetCorrespondenceMode(2); // General entropy
  }
  else if (m_use_regression == true)
  {
    if (m_use_mixed_effects == true)
    {
      m_Sampler->SetCorrespondenceMode(4); // MixedEffects
    }
    else
    {
      m_Sampler->SetCorrespondenceMode(3); // Regression
    }
  }
  else if (m_starting_regularization == m_ending_regularization)
  {
      m_Sampler->SetCorrespondenceMode(0); // mean force
  }
  else
  {
    m_Sampler->SetCorrespondenceMode(1); // Normal
  }
                                                        
  if (m_optimization_iterations-m_optimization_iterations_completed > 0)
    m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_optimization_iterations-m_optimization_iterations_completed);
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
ShapeWorksRunApp<SAMPLERTYPE>::ReadExplanatoryVariables(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    std::istringstream inputsBuffer;
    std::vector<double> evars;
    double etmp;

    elem = docHandle.FirstChild( "explanatory_variable" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> etmp)
      {
        evars.push_back(etmp);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
        (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())->SetExplanatory(evars);

      dynamic_cast<itk::ParticleShapeMixedEffectsMatrixAttribute<double,3> *>
        (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->SetExplanatory(evars);

      m_use_regression = true;
      if (this->m_timepts_per_subject > 1) m_use_mixed_effects = true;
    }
  } 
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
  
  slopename = std::string( m_output_points_prefix ) + std::string(".slope");
  interceptname = std::string( m_output_points_prefix ) + std::string(".intercept");
	
  if( iter >= 0 )
  {
    std::stringstream ss;
    ss << iter+m_optimization_iterations_completed;

    slopename = "./.iter" + ss.str() + "/" + slopename;
    interceptname = "./.iter" + ss.str() + "/" + interceptname;
  }
  
  std::cout << "writing " << slopename << std::endl;
  std::cout << "writing " << interceptname << std::endl;

  std::vector< double > slope;
  std::vector< double > intercept;

  if (m_use_mixed_effects == true)
  {
    vnl_vector<double> slopevec = dynamic_cast<itk::ParticleShapeMixedEffectsMatrixAttribute<double,3> *>
        (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++)
    {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out( slopename.c_str() );
    for (unsigned int i = 0; i < slope.size(); i++)
    {
      out << slope[i] << "\n";
    }
    out.close();
     
    vnl_vector<double> interceptvec = dynamic_cast<itk::ParticleShapeMixedEffectsMatrixAttribute<double,3> *>
      (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->GetIntercept();
    
    for (unsigned int i = 0; i < slopevec.size(); i++)
    {
      intercept.push_back(interceptvec[i]);
    }

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < slope.size(); i++)
    {
      out << intercept[i] << "\n";
    }
    out.close();

    slopename     = std::string( m_output_points_prefix ) + std::string(".sloperand");
    interceptname = std::string( m_output_points_prefix ) + std::string(".interceptrand");

    if( iter >= 0 )
    {
      std::stringstream ss;
      ss << iter+m_optimization_iterations_completed;

      slopename = "./.iter" + ss.str() + "/" + slopename;
      interceptname = "./.iter" + ss.str() + "/" + interceptname;
    }

    std::cout << "writing " << slopename << std::endl;
    std::cout << "writing " << interceptname << std::endl;

    vnl_matrix<double> sloperand_mat = dynamic_cast<itk::ParticleShapeMixedEffectsMatrixAttribute<double,3> *>
        (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->GetSlopeRandom();
    
    out.open(slopename.c_str());
    for (unsigned int i = 0; i < sloperand_mat.rows(); i++)
    {
      for (unsigned int j = 0; j < sloperand_mat.cols(); j++)
      {
        out << sloperand_mat.get(i,j) << " ";
      }
      out << "\n";
    }
    out.close();
      
    vnl_matrix<double> interceptrand_mat = dynamic_cast<itk::ParticleShapeMixedEffectsMatrixAttribute<double,3> *>
        (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->GetInterceptRandom();
    
    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < interceptrand_mat.rows(); i++)
    {
      for (unsigned int j = 0; j < interceptrand_mat.cols(); j++)
      {
        out << interceptrand_mat.get(i,j) << " ";
      }
      out << "\n";
    }
    out.close();
  }
  else
  {
    vnl_vector<double> slopevec = dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
        (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++)
    {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out( slopename.c_str() );
    for (unsigned int i = 0; i < slope.size(); i++)
    {
      out << slope[i] << "\n";
    }
    out.close();
     
    std::vector< double > intercept;
    vnl_vector<double> interceptvec = dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
      (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())->GetIntercept();
    
    for (unsigned int i = 0; i < slopevec.size(); i++)
    {
      intercept.push_back(interceptvec[i]);
    }

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < slope.size(); i++)
    {
      out << intercept[i] << "\n";
    }
    out.close();   
  }
}

template < class SAMPLERTYPE>
void
ShapeWorksRunApp<SAMPLERTYPE>::FlagDomainFct(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    std::istringstream inputsBuffer;
    std::vector<int> f;
    int ftmp;

    // set up fixed landmark positions

    elem = docHandle.FirstChild( "fixed_landmarks" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> ftmp)
      {
        f.push_back(ftmp);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      for (unsigned int i = 0; i < f.size(); i++)
      {
        m_Sampler->GetParticleSystem()->SetFixedParticleFlag(f[i]);
      }
    }

    f.clear();
    elem = docHandle.FirstChild( "fixed_domains" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> ftmp)
      {
        f.push_back(ftmp);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      for (unsigned int i = 0; i < f.size(); i++)
      {
        //if (f[i] > 0.0)
        {
          std::cerr << "domain " << f[i] << " is flagged!\n";
          m_Sampler->GetParticleSystem()->FlagDomain(f[i]);
        }
      }
    }
  }
}

