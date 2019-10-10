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
#include "itkMultiThreaderBase.h"
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

#include "itkImageToVTKImageFilter.h"
#include <vtkContourFilter.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkMassProperties.h>

#include "TriMesh.h"
#include "itkParticleImageDomain.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImplicitSurfaceDomain.h"
#include "itkParticleImageDomainWithHessians.h"
#include <numeric>

#ifdef _WIN32
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <dirent.h>

#include <algorithm>
#include "Utils.h"

// Constructor and destructor
template < class SAMPLERTYPE >
ShapeWorksRunApp < SAMPLERTYPE > ::ShapeWorksRunApp(const char* fn) {
  // Initialize some member variables
  this->SetVerbosity(fn);
  if (m_verbosity_level == 0) {
    std::cout <<
      "Verbosity 0: This will be the only output on your screen, unless there are any errors. Increase the verbosity if needed."
              << std::endl;
  }

  m_CheckpointCounter = 0;
  m_ProcrustesCounter = 0;
  m_SaturationCounter = 0;
  m_disable_procrustes = true;
  m_disable_checkpointing = true;
  m_use_cutting_planes = false;
  m_optimizing = false;
  m_use_regression = false;
  m_use_mixed_effects = false;

  // Read parameter file
  this->startMessage("Reading i/o parameters...");
  this->ReadIOParameters(fn);

  this->doneMessage();

  this->startMessage("Reading optimization parameters...");
  this->ReadOptimizationParameters(fn);
  this->doneMessage();

  this->startMessage("Reading debugging parameters...");
  this->SetDebugParameters(fn);
  this->doneMessage();

  // Set up the optimization process
  m_Sampler = itk::MaximumEntropyCorrespondenceSampler < ImageType > ::New();
  m_Sampler->SetDomainsPerShape(m_domains_per_shape);   // must be done first!
  m_Sampler->SetTimeptsPerIndividual(m_timepts_per_subject);
  m_Sampler->GetParticleSystem()->SetDomainsPerShape(m_domains_per_shape);

  m_Sampler->SetVerbosity(m_verbosity_level);

  // Set up the procrustes registration object.
  m_Procrustes = itk::ParticleProcrustesRegistration < 3 > ::New();
  m_Procrustes->SetParticleSystem(m_Sampler->GetParticleSystem());
  m_Procrustes->SetDomainsPerShape(m_domains_per_shape);

  if (m_use_xyz.size() > 0) {
    for (int i = 0; i < m_domains_per_shape; i++) {
      m_Sampler->SetXYZ(i, m_use_xyz[i]);
    }
  }
  else {
    for (int i = 0; i < m_domains_per_shape; i++) {
      m_Sampler->SetXYZ(i, false);
    }
  }

  if (m_use_normals.size() > 0) {
    for (int i = 0; i < m_domains_per_shape; i++) {
      m_Sampler->SetNormals(i, m_use_normals[i]);
    }
  }
  else {
    for (int i = 0; i < m_domains_per_shape; i++) {
      m_Sampler->SetNormals(i, false);
    }
  }

  if (m_procrustes_scaling == 0) {
    m_Procrustes->ScalingOff();
  }
  else {
    m_Procrustes->ScalingOn();
  }

  this->startMessage("Reading input file paths...");
  this->ReadInputs(fn);
  this->doneMessage();

  this->startMessage("Reading mesh input file paths...");
  this->ReadMeshInputs(fn);
  this->doneMessage();

  this->startMessage("Reading constraints on input shapes...");
  this->ReadConstraints(fn);
  this->doneMessage();

  this->SetIterationCommand();
  this->startMessage("Initializing variables...");
  this->InitializeSampler();
  this->doneMessage();

//    this->startMessage("Reading explanatory variables...");
  this->ReadExplanatoryVariables(fn);
//    this->doneMessage();

  m_p_flgs.clear();
  m_p_flgs = this->FlagParticlesFct(fn);

  m_d_flgs.clear();
  m_d_flgs = this->FlagDomainFct(fn);

  if (m_use_normals.size() > 0) {
    int numShapes = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
    for (int i = 0; i < numShapes; i++) {
      if (m_use_normals[i % m_domains_per_shape]) {
        continue;
      }

      itk::ParticleImageDomainWithHessians < float,
      3 > *domainWithHess = static_cast < itk::ParticleImageDomainWithHessians < float,
      3 > * > (m_Sampler->GetParticleSystem()->GetDomain(i));
      domainWithHess->DeletePartialDerivativeImages();
    }
  }
  else {
    int numShapes = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
    for (int i = 0; i < numShapes; i++) {
      itk::ParticleImageDomainWithHessians < float,
      3 > *domainWithHess = static_cast < itk::ParticleImageDomainWithHessians < float,
      3 > * > (m_Sampler->GetParticleSystem()->GetDomain(i));
      domainWithHess->DeletePartialDerivativeImages();
    }
  }

  if (m_d_flgs.size() > 0) {
    for (int i = 0; i < m_d_flgs.size(); i++) {
      itk::ParticleImageDomainWithHessians < float,
      3 > *domainWithHess = static_cast < itk::ParticleImageDomainWithHessians < float,
      3 > * > (m_Sampler->GetParticleSystem()->GetDomain(m_d_flgs[i]));
      if (m_use_normals.size() > 0) {
        if (m_use_normals[i % m_domains_per_shape]) {
          domainWithHess->DeletePartialDerivativeImages();
        }
        else {
          domainWithHess->DeleteImages();
        }
      }
    }
  }

  if (m_verbosity_level > 1) {
    this->PrintParamInfo();
  }

  m_GoodBad = itk::ParticleGoodBadAssessment < float, 3 > ::New();
  m_GoodBad->SetDomainsPerShape(m_domains_per_shape);
  m_GoodBad->SetCriterionAngle(m_normalAngle);
  m_GoodBad->SetPerformAssessment(m_performGoodBad);

  m_EnergyA.clear();
  m_EnergyB.clear();
  m_TotalEnergy.clear();

  // Now read the transform file if present.
  if (m_transform_file != "") { this->ReadTransformFile();}
  if (m_prefix_transform_file != "") { this->ReadPrefixTransformFile(m_prefix_transform_file);}
}

template < class SAMPLERTYPE >
ShapeWorksRunApp < SAMPLERTYPE > ::~ShapeWorksRunApp() {}

// Reading inputs and parameters from xml file
template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::SetVerbosity(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;

    m_verbosity_level = 0;
    elem = docHandle.FirstChild("verbosity").Element();
    if (elem) { this->m_verbosity_level = atoi(elem->GetText());}
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadIOParameters(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;

    this->m_domains_per_shape = 1;
    elem = docHandle.FirstChild("domains_per_shape").Element();
    if (elem) { this->m_domains_per_shape = atoi(elem->GetText());}

    this->m_number_of_particles.clear();
    elem = docHandle.FirstChild("number_of_particles").Element();
    if (elem) {
      std::istringstream inputsBuffer;
      std::string num;
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> num) {
        this->m_number_of_particles.push_back(atoi(num.c_str()));
      }
    }
    else {
      std::cerr << "Number of particles not specified" << std::endl;
      throw 1;
    }
    if (this->m_domains_per_shape != this->m_number_of_particles.size()) {
      std::cerr <<
        "Inconsistency in parameters... m_domains_per_shape != m_number_of_particles.size()" <<
        std::endl;
      throw 1;
    }

    this->m_transform_file = "";
    elem = docHandle.FirstChild("transform_file").Element();
    if (elem) { this->m_transform_file = elem->GetText();}

    this->m_prefix_transform_file = "";
    elem = docHandle.FirstChild("prefix_transform_file").Element();
    if (elem) { this->m_prefix_transform_file = elem->GetText();}

    this->m_output_dir = "";
    elem = docHandle.FirstChild("output_dir").Element();
    if (elem) { this->m_output_dir = elem->GetText();}

    this->m_output_transform_file = "transform";
    elem = docHandle.FirstChild("output_transform_file").Element();
    if (elem) { this->m_output_transform_file = elem->GetText();}

    this->m_mesh_based_attributes = false;
    elem = docHandle.FirstChild("mesh_based_attributes").Element();
    if (elem) { this->m_mesh_based_attributes = (bool) atoi(elem->GetText());}
    if (m_mesh_based_attributes) {
      m_use_xyz.clear();
      elem = docHandle.FirstChild("use_xyz").Element();
      if (elem) {
        std::istringstream inputsBuffer;
        std::string num;
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> num) {
          this->m_use_xyz.push_back((bool) atoi(num.c_str()));
        }

        if (this->m_domains_per_shape != this->m_use_xyz.size()) {
          std::cerr << "Inconsistency in parameters... m_domains_per_shape != m_use_xyz.size()" <<
            std::endl;
          throw 1;
        }
      }

      m_use_normals.clear();
      elem = docHandle.FirstChild("use_normals").Element();
      if (elem) {
        std::istringstream inputsBuffer;
        std::string num;
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> num) {
          this->m_use_normals.push_back((bool) atoi(num.c_str()));
        }

        if (this->m_domains_per_shape != this->m_use_normals.size()) {
          std::cerr <<
            "Inconsistency in parameters... m_domains_per_shape != m_use_normals.size()" <<
            std::endl;
          throw 1;
        }
      }
    }

    this->m_attributes_per_domain.clear();
    elem = docHandle.FirstChild("attributes_per_domain").Element();
    if (elem) {
      std::istringstream inputsBuffer;
      std::string num;
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> num) {
        this->m_attributes_per_domain.push_back(atoi(num.c_str()));
      }

      if (this->m_domains_per_shape != this->m_attributes_per_domain.size()) {
        std::cerr <<
          "Inconsistency in parameters... m_domains_per_shape != m_attributes_per_domain.size()" <<
          std::endl;
        throw 1;
      }
    }

    this->m_distribution_domain_id = -1;
    elem = docHandle.FirstChild("distribution_plane_id").Element();
    if (elem) { this->m_distribution_domain_id = atoi(elem->GetText());}

    this->m_output_cutting_plane_file = "";
    elem = docHandle.FirstChild("output_cutting_plane_file").Element();
    if (elem) { this->m_output_cutting_plane_file = elem->GetText();}
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadOptimizationParameters(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;

    this->m_processing_mode = 3;
    elem = docHandle.FirstChild("processing_mode").Element();
    if (elem) { this->m_processing_mode = atoi(elem->GetText());}

    this->m_adaptivity_mode = 0;
    elem = docHandle.FirstChild("adaptivity_mode").Element();
    if (elem) { this->m_adaptivity_mode = atoi(elem->GetText());}

    this->m_adaptivity_strength = 0.0;
    elem = docHandle.FirstChild("adaptivity_strength").Element();
    if (elem) { this->m_adaptivity_strength = atof(elem->GetText());}

    this->m_pairwise_potential_type = 0;     // 0 - gaussian (Cates work), 1 - modified cotangent (Meyer)
    elem = docHandle.FirstChild("pairwise_potential_type").Element();
    if (elem) { this->m_pairwise_potential_type = atoi(elem->GetText());}

    this->m_optimizer_type = 2;     // 0 : jacobi, 1 : gauss seidel, 2 : adaptive gauss seidel (with bad moves)
    elem = docHandle.FirstChild("optimizer_type").Element();
    if (elem) { this->m_optimizer_type = atoi(elem->GetText());}

    this->m_timepts_per_subject = 1;
    elem = docHandle.FirstChild("timepts_per_subject").Element();
    if (elem) { this->m_timepts_per_subject = atoi(elem->GetText());}

    this->m_optimization_iterations = 2000;
    elem = docHandle.FirstChild("optimization_iterations").Element();
    if (elem) { this->m_optimization_iterations = atoi(elem->GetText());}

    this->m_optimization_iterations_completed = 0;
    elem = docHandle.FirstChild("optimization_iterations_completed").Element();
    if (elem) { this->m_optimization_iterations_completed = atoi(elem->GetText());}

    this->m_iterations_per_split = 1000;
    elem = docHandle.FirstChild("iterations_per_split").Element();
    if (elem) { this->m_iterations_per_split = atoi(elem->GetText());}

    m_init_criterion = 1e-6;
    elem = docHandle.FirstChild("init_criterion").Element();
    if (elem) { this->m_init_criterion = atof(elem->GetText());}

    m_opt_criterion = 1e-6;
    elem = docHandle.FirstChild("opt_criterion").Element();
    if (elem) { this->m_opt_criterion = atof(elem->GetText());}

    m_use_shape_statistics_in_init = false;
    elem = docHandle.FirstChild("use_shape_statistics_in_init").Element();
    if (elem) { this->m_use_shape_statistics_in_init = (bool) atoi(elem->GetText());}

    this->m_procrustes_interval = 3;
    elem = docHandle.FirstChild("procrustes_interval").Element();
    if (elem) { this->m_procrustes_interval = atoi(elem->GetText());}

    this->m_procrustes_scaling = 1;
    elem = docHandle.FirstChild("procrustes_scaling").Element();
    if (elem) { this->m_procrustes_scaling = atoi(elem->GetText());}

    this->m_relative_weighting = 1.0;
    elem = docHandle.FirstChild("relative_weighting").Element();
    if (elem) { this->m_relative_weighting = atof(elem->GetText());}

    this->m_initial_relative_weighting = 0.05;
    elem = docHandle.FirstChild("initial_relative_weighting").Element();
    if (elem) { this->m_initial_relative_weighting = atof(elem->GetText());}

    this->m_starting_regularization = 1000.0;
    elem = docHandle.FirstChild("starting_regularization").Element();
    if (elem) { this->m_starting_regularization = atof(elem->GetText());}

    this->m_ending_regularization = 1.0;
    elem = docHandle.FirstChild("ending_regularization").Element();
    if (elem) { this->m_ending_regularization = atof(elem->GetText());}

    this->m_recompute_regularization_interval = 1;
    elem = docHandle.FirstChild("recompute_regularization_interval").Element();
    if (elem) { this->m_recompute_regularization_interval = atoi(elem->GetText());}

    m_save_init_splits = true;
    elem = docHandle.FirstChild("save_init_splits").Element();
    if (elem) { this->m_save_init_splits = (bool) atoi(elem->GetText());}

    this->m_checkpointing_interval = 50;
    elem = docHandle.FirstChild("checkpointing_interval").Element();
    if (elem) { this->m_checkpointing_interval = atoi(elem->GetText());}

    this->m_keep_checkpoints = 0;
    elem = docHandle.FirstChild("keep_checkpoints").Element();
    if (elem) { this->m_keep_checkpoints = atoi(elem->GetText());}

    this->m_cotan_sigma_factor = 5.0;
    elem = docHandle.FirstChild("cotan_sigma_factor").Element();
    if (elem) { this->m_cotan_sigma_factor = atof(elem->GetText());}
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::SetDebugParameters(const char* fname) {

  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;

    const float pi = std::acos(-1.0);
    this->m_normalAngle = pi / 2.0;
    elem = docHandle.FirstChild("normal_angle").Element();
    if (elem) { this->m_normalAngle = atof(elem->GetText()) * pi / 180.0;}

    this->m_performGoodBad = false;
    elem = docHandle.FirstChild("report_bad_particles").Element();
    if (elem) { this->m_performGoodBad = (bool) atoi(elem->GetText());}

    this->m_logEnergy = false;
    elem = docHandle.FirstChild("log_energy").Element();
    if (elem) { this->m_logEnergy = bool(atoi(elem->GetText()));}
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadInputs(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;

    std::istringstream inputsBuffer;
    std::string filename;
    int numShapes = 0;

    // load input shapes
    std::vector < std::string > shapeFiles;
    elem = docHandle.FirstChild("inputs").Element();
    if (!elem) {
      std::cerr << "No input files have been specified" << std::endl;
      throw 1;
    }
    else {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> filename) {
        shapeFiles.push_back(filename);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      numShapes = shapeFiles.size();

      m_Sampler->SetImageFiles(shapeFiles);

      int shapeCount = 0;
      typename itk::ImageFileReader < ImageType > ::Pointer reader = itk::ImageFileReader <
                                                                     ImageType > ::New();
      reader->SetFileName(shapeFiles[shapeCount].c_str());
      std::cerr << "Now reading: " << shapeFiles[shapeCount] << "\n";
      reader->UpdateLargestPossibleRegion();
      m_Sampler->SetInput(shapeCount, reader->GetOutput());       // set the 0th input

      m_spacing = reader->GetOutput()->GetSpacing()[0];

      m_filenames.clear();

      for (int shapeCount = 0; shapeCount < numShapes; shapeCount++) {
        char* str = new char[shapeFiles[shapeCount].length() + 1];
        strcpy(str, shapeFiles[shapeCount].c_str());

        char* fname;
        char* pch;
        pch = strtok(str, "/");
        while (pch != NULL) {
          fname = pch;
          pch = strtok(NULL, "/");
        }

        char* pch2;
        pch2 = strrchr(fname, '.');
        int num = pch2 - fname + 1;
        int num2 = strlen(fname);
        strncpy(pch2, "", num2 - num);

        m_filenames.push_back(std::string(fname));
      }

      shapeFiles.clear();
    }
    // load point files
    pointFiles.clear();
    elem = docHandle.FirstChild("point_files").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> filename) {
        pointFiles.push_back(filename);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      // read point files only if they are all present
      if (pointFiles.size() < numShapes) {
        std::cerr << "ERROR: not enough point files, none will be loaded!!!" << std::endl;
        throw 1;
      }
      else {
        for (int shapeCount = 0; shapeCount < numShapes; shapeCount++) {
          m_Sampler->SetPointsFile(shapeCount, pointFiles[shapeCount]);
        }
      }

      pointFiles.clear();
    }
  }   // end: document check
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadMeshInputs(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;
    std::istringstream inputsBuffer;
    std::string filename;
    int numShapes = m_filenames.size();

    if (m_mesh_based_attributes) {
      m_Sampler->RegisterGeneralShapeMatrices();
    }

#if defined(SW_USE_MESH) || defined(SW_USE_FEAMESH)
    // load mesh files
    std::vector < std::string > meshFiles;
    elem = docHandle.FirstChild("mesh_files").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> filename) {
        meshFiles.push_back(filename);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      // read mesh files only if they are all present
      if (meshFiles.size() < numShapes) {
        std::cerr << "not enough mesh files!!!" << std::endl;
        throw 1;
      }
      else {
        m_Sampler->SetMeshFiles(meshFiles);
      }
      meshFiles.clear();
    }
#endif /* if defined(SW_USE_MESH) || defined(SW_USE_FEAMESH) */

    // attributes
    if ((this->m_attributes_per_domain.size() >= 1 &&
         *std::max_element(m_attributes_per_domain.begin(),
                           m_attributes_per_domain.end()) > 0) || m_mesh_based_attributes) {
      // attribute scales
      double sc;
      std::vector < double > attr_scales;

      elem = docHandle.FirstChild("attribute_scales").Element();
      if (elem) {
        inputsBuffer.str(elem->GetText());

        while (inputsBuffer >> sc) {
          attr_scales.push_back(sc);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
      }
      int numTot = 0;
      for (int i = 0; i < m_domains_per_shape; i++) {
        if (m_use_xyz[i]) {
          numTot += 3;
        }
        if (m_use_normals[i]) {
          numTot += 3;
        }
        if (m_attributes_per_domain.size() > 0) {
          numTot += m_attributes_per_domain[i];
        }
      }

      if (numTot != attr_scales.size()) {
        std::cerr << "not enough attribute scale values!!!" << std::endl;
        throw 1;
      }
      m_Sampler->SetAttributeScales(attr_scales);

      // attribute files
      std::vector < std::string > attrFiles;
      elem = docHandle.FirstChild("attribute_files").Element();
      if (elem) {
        inputsBuffer.str(elem->GetText());

        while (inputsBuffer >> filename) {
          attrFiles.push_back(filename);
        }

        inputsBuffer.clear();
        inputsBuffer.str("");

#ifdef SW_USE_FEAMESH

        m_Sampler->SetFeaFiles(attrFiles);
#else
        std::cerr << "ERROR: Rebuild with BUILD_FeaMeshSupport option turned ON in CMakeFile!!" <<
          std::endl;
        throw 1;
#endif /* ifdef SW_USE_FEAMESH */
      }

      // need fids for mesh based fea
      if (m_mesh_based_attributes) {
#ifdef SW_USE_FEAMESH
        m_Sampler->SetAttributesPerDomain(this->m_attributes_per_domain);
        std::vector < std::string > feaGradFiles;
        elem = docHandle.FirstChild("attribute_grad_files").Element();
        if (elem) {
          inputsBuffer.str(elem->GetText());
          while (inputsBuffer >> filename) {
            feaGradFiles.push_back(filename);
          }

          inputsBuffer.clear();
          inputsBuffer.str("");

          int totAttributes = std::accumulate(
            m_attributes_per_domain.begin(), m_attributes_per_domain.end(), 0);
          if (feaGradFiles.size() != numShapes * totAttributes / m_domains_per_shape) {
            std::cerr << "ERROR: Invalid number of attribute gradient files!!!" << std::endl;
            throw 1;
          }
          else {
            m_Sampler->SetFeaGradFiles(feaGradFiles);
          }
        }
        else {
          if (this->m_attributes_per_domain.size() >= 1 &&
              *std::max_element(m_attributes_per_domain.begin(),
                                m_attributes_per_domain.end()) > 0) {
            std::cerr << "ERROR: No feature gradient files!!!" << std::endl;
            throw 1;
          }
        }

        std::vector < std::string > fidsFiles;
        elem = docHandle.FirstChild("fids").Element();
        if (elem) {
          inputsBuffer.str(elem->GetText());
          while (inputsBuffer >> filename) {
            fidsFiles.push_back(filename);
          }

          inputsBuffer.clear();
          inputsBuffer.str("");
          if (fidsFiles.size() != numShapes) {
            std::cerr << "ERROR: Invalid number of fids files!!" << std::endl;
            throw 1;
          }
          else {
            m_Sampler->SetFidsFiles(fidsFiles);
          }
        }
        else {
          if (this->m_attributes_per_domain.size() >= 1 &&
              *std::max_element(m_attributes_per_domain.begin(),
                                m_attributes_per_domain.end()) > 0) {
            std::cerr << "ERROR: Must provide fids!!" << std::endl;
            throw 1;
          }
        }
#else
        std::cerr << "ERROR: Rebuild with BUILD_FeaMeshSupport option turned ON in CMakeFile!!" <<
          std::endl;
        throw 1;
#endif /* ifdef SW_USE_FEAMESH */
      }
    }
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadConstraints(const char* fname) {
  if (this->m_distribution_domain_id > -1) {
    this->ReadDistributionCuttingPlane(fname);
  }
  else {
    this->ReadCuttingPlanes(fname);
  }

  this->ReadCuttingSpheres(fname);
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadDistributionCuttingPlane(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;
    std::istringstream inputsBuffer;
    int numShapes = m_filenames.size();
    // Distribution cutting planes activated
    elem = docHandle.FirstChild("distribution_cutting_plane").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());

      cpVals.clear();
      double pt;

      while (inputsBuffer >> pt) {
        cpVals.push_back(pt);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      if (cpVals.size() < 9) {
        std::cerr <<
          "ERROR: Incomplete cutting plane data for reference shape! No cutting planes will be loaded!!"
                  << std::endl;
        throw 1;
      }
      else {
        m_use_cutting_planes = true;
        vnl_vector_fixed < double, 3 > a, b, c;
        int ctr = 0;

        for (int shapeCount = 0; shapeCount < numShapes; shapeCount++) {
          a[0] = cpVals[ctr++];
          a[1] = cpVals[ctr++];
          a[2] = cpVals[ctr++];

          b[0] = cpVals[ctr++];
          b[1] = cpVals[ctr++];
          b[2] = cpVals[ctr++];

          c[0] = cpVals[ctr++];
          c[1] = cpVals[ctr++];
          c[2] = cpVals[ctr++];

          // If initial transform provided, transform cutting plane points
          if (m_prefix_transform_file != "" && m_transform_file != "") {
            itk::ParticleSystem < 3 > ::PointType pa;
            itk::ParticleSystem < 3 > ::PointType pb;
            itk::ParticleSystem < 3 > ::PointType pc;

            pa[0] = a[0]; pa[1] = a[1]; pa[2] = a[2];
            pb[0] = b[0]; pb[1] = b[1]; pb[2] = b[2];
            pc[0] = c[0]; pc[1] = c[1]; pc[2] = c[2];

            itk::ParticleSystem < 3 >
            ::TransformType T = m_Sampler->GetParticleSystem()->GetTransform(shapeCount);
            itk::ParticleSystem < 3 >
            ::TransformType prefT = m_Sampler->GetParticleSystem()->GetPrefixTransform(shapeCount);
            pa = m_Sampler->GetParticleSystem()->TransformPoint(pa, T * prefT);
            pb = m_Sampler->GetParticleSystem()->TransformPoint(pb, T * prefT);
            pc = m_Sampler->GetParticleSystem()->TransformPoint(pc, T * prefT);

            a[0] = pa[0]; a[1] = pa[1]; a[2] = pa[2];
            b[0] = pb[0]; b[1] = pb[1]; b[2] = pb[2];
            c[0] = pc[0]; c[1] = pc[1]; c[2] = pc[2];
          }

          m_Sampler->SetCuttingPlane(shapeCount, a, b, c);
          ctr = 0;           // use same cutting plane for all shapes
        }
      }
    }
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadCuttingPlanes(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;
    std::istringstream inputsBuffer;
    int numShapes = m_filenames.size();

    m_cutting_planes_per_input.clear();

    elem = docHandle.FirstChild("num_planes_per_input").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());
      double val;

      while (inputsBuffer >> val) {
        m_cutting_planes_per_input.push_back(val);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");
      if (m_cutting_planes_per_input.size() < numShapes) {
        std::cerr <<
          "ERROR: Incomplete cutting plane data! Number of cutting planes for every input shape is required!!"
                  << std::endl;
        throw 1;
      }
    }
    int numPlanes = std::accumulate(
      m_cutting_planes_per_input.begin(), m_cutting_planes_per_input.end(), 0);

    // otherwise read separate cutting plane for every shape
    elem = docHandle.FirstChild("cutting_planes").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());

      cpVals.clear();
      double pt;

      while (inputsBuffer >> pt) {
        cpVals.push_back(pt);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      if (cpVals.size() < 9 * numPlanes) {
        std::cerr << "ERROR: Incomplete cutting plane data! No cutting planes will be loaded!!" <<
          std::endl;
        throw 1;
      }
      else {
        m_use_cutting_planes = true;
        vnl_vector_fixed < double, 3 > a, b, c;
        int ctr = 0;

        for (int shapeCount = 0; shapeCount < numShapes; shapeCount++) {
          for (int planeCount = 0; planeCount < m_cutting_planes_per_input[shapeCount];
               planeCount++) {
            a[0] = cpVals[ctr++];
            a[1] = cpVals[ctr++];
            a[2] = cpVals[ctr++];

            b[0] = cpVals[ctr++];
            b[1] = cpVals[ctr++];
            b[2] = cpVals[ctr++];

            c[0] = cpVals[ctr++];
            c[1] = cpVals[ctr++];
            c[2] = cpVals[ctr++];

            // If initial transform provided, transform cutting plane points
            if (m_prefix_transform_file != "" && m_transform_file != "") {
              itk::ParticleSystem < 3 > ::PointType pa;
              itk::ParticleSystem < 3 > ::PointType pb;
              itk::ParticleSystem < 3 > ::PointType pc;

              pa[0] = a[0]; pa[1] = a[1]; pa[2] = a[2];
              pb[0] = b[0]; pb[1] = b[1]; pb[2] = b[2];
              pc[0] = c[0]; pc[1] = c[1]; pc[2] = c[2];

              itk::ParticleSystem < 3 >
              ::TransformType T = m_Sampler->GetParticleSystem()->GetTransform(shapeCount);
              itk::ParticleSystem < 3 >
              ::TransformType prefT =
                m_Sampler->GetParticleSystem()->GetPrefixTransform(shapeCount);
              pa = m_Sampler->GetParticleSystem()->TransformPoint(pa, T * prefT);
              pb = m_Sampler->GetParticleSystem()->TransformPoint(pb, T * prefT);
              pc = m_Sampler->GetParticleSystem()->TransformPoint(pc, T * prefT);

              a[0] = pa[0]; a[1] = pa[1]; a[2] = pa[2];
              b[0] = pb[0]; b[1] = pb[1]; b[2] = pb[2];
              c[0] = pc[0]; c[1] = pc[1]; c[2] = pc[2];
            }

            m_Sampler->SetCuttingPlane(shapeCount, a, b, c);
          }
        }
      }
    }
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadCuttingSpheres(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;
    std::istringstream inputsBuffer;
    int numShapes = m_filenames.size();
    // sphere radii and centers
    m_spheres_per_input.clear();

    elem = docHandle.FirstChild("spheres_per_domain").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());
      double val;

      while (inputsBuffer >> val) {
        m_spheres_per_input.push_back(val);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");
      if (m_spheres_per_input.size() < numShapes) {
        std::cerr <<
          "ERROR: Incomplete cutting plane data! Number of cutting spheres for every input shape is required!!"
                  << std::endl;
        throw 1;
      }
    }
    int numSpheres = std::accumulate(m_spheres_per_input.begin(), m_spheres_per_input.end(), 0);

    radList.clear();
    double r;

    elem = docHandle.FirstChild("sphere_radii").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());

      while (inputsBuffer >> r) {
        radList.push_back(r);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      if (radList.size() < numSpheres) {
        std::cerr << "ERROR: Incomplete sphere radius data! No spheres will be loaded!!" <<
          std::endl;
        throw 1;
      }
      else {
        elem = docHandle.FirstChild("sphere_centers").Element();
        if (elem) {
          inputsBuffer.str(elem->GetText());

          spVals.clear();
          double pt;

          while (inputsBuffer >> pt) {
            spVals.push_back(pt);
          }
          inputsBuffer.clear();
          inputsBuffer.str("");

          if (spVals.size() < 3 * numSpheres) {
            std::cerr << "ERROR: Incomplete sphere center data! No spheres will be loaded!!" <<
              std::endl;
            throw 1;
          }
          else {
            vnl_vector_fixed < double, 3 > center;
            double rad;
            int c_ctr = 0;
            int r_ctr = 0;

            for (int shapeCount = 0; shapeCount < numShapes; shapeCount++) {
              for (int sphereCount = 0; sphereCount < m_spheres_per_input[shapeCount];
                   sphereCount++) {
                center[0] = spVals[c_ctr++];
                center[1] = spVals[c_ctr++];
                center[2] = spVals[c_ctr++];

                rad = radList[r_ctr++];

                m_Sampler->AddSphere(shapeCount, center, rad);
              }
            }
          }
        }
      }
    }
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadExplanatoryVariables(const char* fname) {
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;

    std::istringstream inputsBuffer;
    std::vector < double > evars;
    double etmp;

    elem = docHandle.FirstChild("explanatory_variable").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> etmp) {
        evars.push_back(etmp);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      dynamic_cast < itk::ParticleShapeLinearRegressionMatrixAttribute < double, 3 > * >
      (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())->SetExplanatory(evars);

      dynamic_cast < itk::ParticleShapeMixedEffectsMatrixAttribute < double, 3 > * >
      (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->SetExplanatory(evars);

      m_use_regression = true;
      if (this->m_timepts_per_subject > 1) { m_use_mixed_effects = true;}
    }
  }
}

template < class SAMPLERTYPE >
std::vector < int >
ShapeWorksRunApp < SAMPLERTYPE > ::FlagParticlesFct(const char* fname) {
  std::vector < int > f;
  f.clear();
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;

    std::istringstream inputsBuffer;

    int ftmp;

    // set up fixed landmark positions // domain_index particle_index
    elem = docHandle.FirstChild("fixed_landmarks").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> ftmp) {
        f.push_back(ftmp);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      for (unsigned int i = 0; i < f.size() / 2; i++) {
        m_Sampler->GetParticleSystem()->SetFixedParticleFlag(f[2 * i], f[2 * i + 1]);
      }
    }
  }
  return f;
}

template < class SAMPLERTYPE >
std::vector < int >
ShapeWorksRunApp < SAMPLERTYPE > ::FlagDomainFct(const char* fname) {
  std::vector < int > f;
  f.clear();
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay) {
    TiXmlHandle docHandle( &doc);
    TiXmlElement* elem;

    std::istringstream inputsBuffer;

    int ftmp;
    // set up fixed domains // domain_index
    elem = docHandle.FirstChild("fixed_domains").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> ftmp) {
        f.push_back(ftmp);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");

      for (unsigned int i = 0; i < f.size(); i++) {
        m_Sampler->GetParticleSystem()->FlagDomain(f[i]);
      }
    }
  }
  return f;
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadTransformFile() {
  object_reader < itk::ParticleSystem < 3 > ::TransformType > reader;
  reader.SetFileName(m_transform_file);
  reader.Update();
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_Sampler->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReadPrefixTransformFile(const std::string &fn) {
  object_reader < itk::ParticleSystem < 3 > ::TransformType > reader;
  reader.SetFileName(fn.c_str());
  reader.Update();
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_Sampler->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
  }
}

// Initialization and Optimization
template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::InitializeSampler() {
  float nbhd_to_sigma = 3.0;   // 3.0 -> 1.0
  float flat_cutoff = 0.3;   // 0.3 -> 0.85

  m_Sampler->SetPairwisePotentialType(m_pairwise_potential_type);

  m_Sampler->GetGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetCurvatureGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);
  m_Sampler->GetCurvatureGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);
  m_Sampler->GetQualifierGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetQualifierGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_Sampler->GetModifiedCotangentGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetModifiedCotangentGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_Sampler->GetOmegaGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetOmegaGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVariance(
    m_starting_regularization);
  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetOptimizer()->SetTimeStep(1.0);

  if (m_optimizer_type == 0) {
    m_Sampler->GetOptimizer()->SetModeToJacobi();
  }
  else if (m_optimizer_type == 1) {
    m_Sampler->GetOptimizer()->SetModeToGaussSeidel();
  }
  else {
    m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();
  }

  m_Sampler->SetSamplingOn();

  m_Sampler->SetCorrespondenceOn();

  m_Sampler->SetAdaptivityMode(m_adaptivity_mode);
  m_Sampler->GetEnsembleEntropyFunction()
  ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()
  ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetEnsembleRegressionEntropyFunction()
  ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()
  ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);

  m_Sampler->Initialize();

  m_Sampler->GetOptimizer()->SetTolerance(0.0);
}

template < class SAMPLERTYPE >
double
ShapeWorksRunApp < SAMPLERTYPE > ::GetMinNeighborhoodRadius() {
  double rad = 0.0;
  typename itk::ImageToVTKImageFilter < ImageType > ::Pointer itk2vtkConnector;
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {

    const itk::ParticleImageDomain < float,
    3 > * domain = static_cast < const itk::ParticleImageDomain < float,
    3 > * > (m_Sampler->GetParticleSystem()->GetDomain(i));

    itk2vtkConnector = itk::ImageToVTKImageFilter < ImageType > ::New();
    itk2vtkConnector->SetInput(domain->GetImage());
    vtkSmartPointer < vtkContourFilter > ls = vtkSmartPointer < vtkContourFilter > ::New();
    ls->SetInputData(itk2vtkConnector->GetOutput());
    ls->SetValue(0, 0.0);
    ls->Update();
    vtkSmartPointer < vtkMassProperties > mp = vtkSmartPointer < vtkMassProperties > ::New();
    mp->SetInputData(ls->GetOutput());
    mp->Update();
    double area = mp->GetSurfaceArea();
    double sigma =
      std::sqrt(area / (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) * M_PI));
    if (rad < sigma) {
      rad = sigma;
    }
  }
  return rad;
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::AddSinglePoint() {
  typedef itk::ParticleSystem < 3 > ParticleSystemType;
  typedef ParticleSystemType::PointType PointType;
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++) {
    if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) > 0) {
      continue;
    }

    bool done = false;

    ImageType::Pointer img = dynamic_cast < itk::ParticleImageDomain < float, 3 > * > (
      m_Sampler->GetParticleSystem()->GetDomain(i))->GetImage();

    itk::ZeroCrossingImageFilter < ImageType, ImageType > ::Pointer zc =
      itk::ZeroCrossingImageFilter < ImageType, ImageType > ::New();
    zc->SetInput(img);
    zc->Update();
    itk::ImageRegionConstIteratorWithIndex < ImageType > it(zc->GetOutput(),
                                                            zc->GetOutput()->GetRequestedRegion());

    for (it.GoToReverseBegin(); !it.IsAtReverseEnd() && done == false; --it) {
      if (it.Get() == 1.0) {
        PointType pos;
        img->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
        done = true;
        try
        {
          m_Sampler->GetParticleSystem()->AddPosition(pos, i);
        }
        catch(itk::ExceptionObject &) {
          done = false;
        }
      }
    }
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::Initialize() {
  if (m_verbosity_level > 0) {
    std::cout << "------------------------------\n";
    std::cout << "*** Initialize Step\n";
    std::cout << "------------------------------\n";
  }

  m_disable_checkpointing = true;
  m_disable_procrustes = true;

  m_disable_procrustes = false;
  m_Procrustes->SetComputeTransformationOn();
  if (m_procrustes_interval != 0) { // Initial registration
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) > 10) {
        m_Procrustes->RunRegistration(i);
      }
    }
    this->WritePointFiles();
    this->WriteTransformFile();
  }
  m_disable_procrustes = true;
  m_Procrustes->SetComputeTransformationOff();

  m_Sampler->GetParticleSystem()->SynchronizePositions();

  m_Sampler->GetCurvatureGradientFunction()->SetRho(0.0);
  m_Sampler->GetOmegaGradientFunction()->SetRho(0.0);

  m_Sampler->SetCorrespondenceOn();

  if (m_use_shape_statistics_in_init) {
    if (m_attributes_per_domain.size() > 0 &&
        *std::max_element(m_attributes_per_domain.begin(), m_attributes_per_domain.end()) > 0) {
      if (m_mesh_based_attributes) {
        m_Sampler->SetCorrespondenceMode(5);
      }
      else {
        m_Sampler->SetCorrespondenceMode(2);
      }
    }
    else {
      if (m_mesh_based_attributes) {
        m_Sampler->SetCorrespondenceMode(5);
      }
      else {
        m_Sampler->SetCorrespondenceMode(1);
      }
    }

    m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                     m_ending_regularization,
                                                                     m_iterations_per_split);

    m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(
      m_starting_regularization,
      m_ending_regularization,
      m_iterations_per_split);
  }
  else {
    // force to mean
    if ((m_attributes_per_domain.size() > 0 &&
         *std::max_element(m_attributes_per_domain.begin(),
                           m_attributes_per_domain.end()) > 0) || m_mesh_based_attributes) {
      m_Sampler->SetCorrespondenceMode(6);
    }
    else {
      m_Sampler->SetCorrespondenceMode(0);
    }
  }

  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);

  this->AddSinglePoint();

  m_Sampler->GetParticleSystem()->SynchronizePositions();

  int split_number = 0;

  int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
  vnl_vector_fixed < double, 3 > random;
  srand(1);
  for (int i = 0; i < 3; i++) {
    random[i] = static_cast < double > (rand());
  }
  double norm = random.magnitude();
  random /= norm;

  double epsilon = this->m_spacing;
  bool flag_split = false;

  for (int i = 0; i < n; i++) {
    int d = i % m_domains_per_shape;
    if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
      flag_split = true;
      break;
    }
  }

  while (flag_split) {
    //        m_Sampler->GetEnsembleEntropyFunction()->PrintShapeMatrix();
    this->optimize_stop();
    for (int i = 0; i < n; i++) {
      int d = i % m_domains_per_shape;
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
        m_Sampler->GetParticleSystem()->SplitAllParticlesInDomain(random, epsilon, i, 0);
      }
    }

    m_Sampler->GetParticleSystem()->SynchronizePositions();

    split_number++;

    if (m_verbosity_level > 0) {
      std::cout << "split number = " << split_number << std::endl;

      std::cout << std::endl << "Particle count: ";
      for (unsigned int i = 0; i < this->m_domains_per_shape; i++) {
        std::cout << m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) << "  ";
      }
      std::cout << std::endl;
    }

    if (m_save_init_splits == true) {
      std::stringstream ss;
      ss << split_number;

      std::stringstream ssp;
      std::string dir_name = "split" + ss.str();
      for (int i = 0; i < m_domains_per_shape; i++) {
        ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles(i);
        dir_name += "_" + ssp.str();
        ssp.str("");
      }
      dir_name += "pts_wo_opt";
      std::string out_path = m_output_dir;

      std::string tmp_dir_name = out_path + "/" + dir_name;

      this->WritePointFiles(tmp_dir_name + "/");
      this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      this->WriteParameters(split_number);
    }

    m_EnergyA.clear();
    m_EnergyB.clear();
    m_TotalEnergy.clear();
    std::stringstream ss;
    ss << split_number;
    std::stringstream ssp;

    ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles();     // size from domain 0
    m_strEnergy = "split" + ss.str();

    for (int i = 0; i < m_domains_per_shape; i++) {
      ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles(i);
      m_strEnergy += "_" + ssp.str();
      ssp.str("");
    }
    m_strEnergy += "pts_init";

    if (this->m_pairwise_potential_type == 1) {
      this->SetCotanSigma();

      double minRad = 3.0 * this->GetMinNeighborhoodRadius();

      m_Sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
      m_Sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(
        minRad);
    }

    m_SaturationCounter = 0;
    m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
    m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
    m_Sampler->Modified();
    m_Sampler->Update();

    if (m_save_init_splits == true) {
      std::stringstream ss;
      ss << split_number;
      std::stringstream ssp;
      std::string dir_name = "split" + ss.str();
      for (int i = 0; i < m_domains_per_shape; i++) {
        ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles(i);
        dir_name += "_" + ssp.str();
        ssp.str("");
      }
      dir_name += "pts_w_opt";
      std::string out_path = m_output_dir;
      std::string tmp_dir_name = out_path + "/" + dir_name;

      this->WritePointFiles(tmp_dir_name + "/");
      this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      this->WriteParameters(split_number);
    }
    this->WritePointFiles();
    this->WritePointFilesWithFeatures();
    this->WriteEnergyFiles();
    this->WriteTransformFile();

    flag_split = false;
    for (int i = 0; i < n; i++) {
      int d = i % m_domains_per_shape;
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
        flag_split = true;
        break;
      }
    }
  }
  this->WritePointFiles();
  this->WritePointFilesWithFeatures();
  this->WriteTransformFile();
  this->WriteCuttingPlanePoints();
  if (m_verbosity_level > 0) {
    std::cout << "Finished initialization!!!" << std::endl;
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::AddAdaptivity() {
  if (m_verbosity_level > 0) {
    std::cout << "------------------------------\n";
    std::cout << "*** AddAdaptivity Step\n";
    std::cout << "------------------------------\n";
  }

  if (m_adaptivity_strength == 0.0) { return;}
  m_disable_checkpointing = true;
  m_disable_procrustes = true;

  if (this->m_pairwise_potential_type == 1) {
    this->SetCotanSigma();
  }

  double minRad = 3.0 * this->GetMinNeighborhoodRadius();

  m_Sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
  m_Sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);

  m_Sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);

  m_SaturationCounter = 0;
  m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
  m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_Sampler->Modified();
  m_Sampler->Update();

  this->WritePointFiles();
  this->WritePointFilesWithFeatures();
  this->WriteTransformFile();
  this->WriteCuttingPlanePoints();

  if (m_verbosity_level > 0) {
    std::cout << "Finished adaptivity!!!" << std::endl;
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::Optimize() {
  if (m_verbosity_level > 0) {
    std::cout << "------------------------------\n";
    std::cout << "*** Optimize Step\n";
    std::cout << "------------------------------\n";
  }

  m_optimizing = true;
  m_Sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_relative_weighting);

  if (this->m_pairwise_potential_type == 1) {
    this->SetCotanSigma();

    double minRad = 3.0 * this->GetMinNeighborhoodRadius();

    m_Sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
    m_Sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(
      minRad);
  }

  m_disable_checkpointing = false;
  m_disable_procrustes = false;

  if (m_procrustes_interval != 0) { // Initial registration
    m_Procrustes->RunRegistration();
    this->WritePointFiles();
    this->WriteTransformFile();

    if (m_use_cutting_planes == true && m_distribution_domain_id > -1) {
      // transform cutting planes
      m_Sampler->TransformCuttingPlanes(m_distribution_domain_id);
    }
  }

  if (m_optimizer_type == 0) {
    m_Sampler->GetOptimizer()->SetModeToJacobi();
  }
  else if (m_optimizer_type == 1) {
    m_Sampler->GetOptimizer()->SetModeToGaussSeidel();
  }
  else {
    m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();
  }

  // Set up the minimum variance decay
  m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                   m_ending_regularization,
                                                                   m_optimization_iterations -
                                                                   m_optimization_iterations_completed);

  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_Sampler->SetCorrespondenceOn();

  if ((m_attributes_per_domain.size() > 0 &&
       *std::max_element(m_attributes_per_domain.begin(),
                         m_attributes_per_domain.end()) > 0) || m_mesh_based_attributes) {
    m_Sampler->SetCorrespondenceMode(5);
  }
  else if (m_use_regression == true) {
    if (m_use_mixed_effects == true) {
      m_Sampler->SetCorrespondenceMode(4);       // MixedEffects
    }
    else {
      m_Sampler->SetCorrespondenceMode(3);       // Regression
    }
  }
  else if (m_starting_regularization == m_ending_regularization) {
    m_Sampler->SetCorrespondenceMode(0);     // mean force
  }
  else {
    m_Sampler->SetCorrespondenceMode(1);     // Ensemble Entropy
  }

  if (m_optimization_iterations - m_optimization_iterations_completed > 0) {
    m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(
      m_optimization_iterations - m_optimization_iterations_completed);
  }
  else { m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(0);}

  m_EnergyA.clear();
  m_EnergyB.clear();
  m_TotalEnergy.clear();
  m_strEnergy = "opt";

  m_SaturationCounter = 0;
  m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_Sampler->GetOptimizer()->SetTolerance(0.0);
  m_Sampler->Modified();
  m_Sampler->Update();

  this->WritePointFiles();
  this->WritePointFilesWithFeatures();
  this->WriteEnergyFiles();
  this->WriteTransformFile();
  this->WriteModes();
  this->WriteCuttingPlanePoints();
  this->WriteParameters();
  if (m_verbosity_level > 0) {
    std::cout << "Finished optimization!!!" << std::endl;
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::optimize_start() {
  m_Sampler->GetOptimizer()->StartOptimization();
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::optimize_stop() {
  m_Sampler->GetOptimizer()->StopOptimization();
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::IterateCallback(itk::Object*, const itk::EventObject &) {
  if (m_performGoodBad == true) {
    std::vector < std::vector < int >> tmp;
    tmp = m_GoodBad->RunAssessment(m_Sampler->GetParticleSystem(),
                                   m_Sampler->GetMeanCurvatureCache());

    if (!tmp.empty()) {
      if (this->m_badIds.empty()) {
        this->m_badIds.resize(m_domains_per_shape);
      }

      for (int i = 0; i < m_domains_per_shape; i++) {
        for (int j = 0; j < tmp[i].size(); j++) {
          if (m_badIds[i].empty()) {
            this->m_badIds[i].push_back(tmp[i][j]);
          }
          else {
            if (std::count(m_badIds[i].begin(), m_badIds[i].end(), tmp[i][j]) == 0) {
              this->m_badIds[i].push_back(tmp[i][j]);
            }
          }
        }
      }
    }
    ReportBadParticles();
  }

  this->ComputeEnergyAfterIteration();

  int lnth = m_TotalEnergy.size();
  if (lnth > 1) {
    double val = std::abs(m_TotalEnergy[lnth - 1] - m_TotalEnergy[lnth - 2]) / std::abs(
      m_TotalEnergy[lnth - 2]);
    if ((m_optimizing == false && val < m_init_criterion) ||
        (m_optimizing == true && val < m_opt_criterion)) {
      m_SaturationCounter++;
    }
    else {
      m_SaturationCounter = 0;
    }
    if (m_SaturationCounter > 10) {
      if (m_verbosity_level > 2) {
        std::cout << " \n ----Early termination due to minimal energy decay---- \n";
      }
      this->optimize_stop();
    }
  }

  if (m_checkpointing_interval != 0 && m_disable_checkpointing == false) {
    m_CheckpointCounter++;
    if (m_CheckpointCounter == (int)m_checkpointing_interval) {
      m_CheckpointCounter = 0;

      this->WritePointFiles();
      this->WriteTransformFile();
      this->WritePointFilesWithFeatures();
      this->WriteModes();
      this->WriteParameters();
      this->WriteEnergyFiles();
    }
  }

  if (m_optimizing == false) { return;}

  if (m_procrustes_interval != 0 && m_disable_procrustes == false) {
    m_ProcrustesCounter++;

    if (m_ProcrustesCounter >= (int)m_procrustes_interval) {
      m_ProcrustesCounter = 0;
      m_Procrustes->RunRegistration();

      if (m_use_cutting_planes == true && m_distribution_domain_id > -1) {
        // transform cutting planes
        m_Sampler->TransformCuttingPlanes(m_distribution_domain_id);
      }
    }
  }

  static unsigned int iteration_no = 0;
  // Checkpointing after procrustes (override for optimizing step)
  if (m_checkpointing_interval != 0 && m_disable_checkpointing == false) {

    m_CheckpointCounter++;

    if (m_CheckpointCounter == (int)m_checkpointing_interval) {
      iteration_no += m_checkpointing_interval;
      m_CheckpointCounter = 0;

      this->WritePointFiles();
      this->WriteTransformFile();
      this->WritePointFilesWithFeatures();
      this->WriteModes();
      this->WriteParameters();
      this->WriteEnergyFiles();

      if (m_keep_checkpoints) {
        this->WritePointFiles(iteration_no);
        this->WritePointFilesWithFeatures(iteration_no);
        this->WriteTransformFile(iteration_no);
        this->WriteParameters(iteration_no);
      }
    }
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ComputeEnergyAfterIteration() {
  int numShapes = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
  double corrEnergy = 0.0;

  double sampEnergy = 0.0;
  for (int i = 0; i < numShapes; i++) {
    m_Sampler->GetLinkingFunction()->SetDomainNumber(i);
    for (int j = 0; j < m_Sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      if (m_Sampler->GetParticleSystem()->GetDomainFlag(i)) {
        sampEnergy += 0.0;
      }
      else {
        sampEnergy +=
          m_Sampler->GetLinkingFunction()->EnergyA(j, i, m_Sampler->GetParticleSystem());
      }
      if (m_Sampler->GetCorrespondenceMode() == 0) {
        corrEnergy +=
          m_Sampler->GetLinkingFunction()->EnergyB(j, i, m_Sampler->GetParticleSystem());
      }
    }
  }

  if (m_Sampler->GetCorrespondenceMode() > 0) {
    corrEnergy = m_Sampler->GetLinkingFunction()->EnergyB(0, 0, m_Sampler->GetParticleSystem());
  }

  double totalEnergy = sampEnergy + corrEnergy;
  m_EnergyA.push_back(sampEnergy);
  m_EnergyB.push_back(corrEnergy);
  m_TotalEnergy.push_back(totalEnergy);
  if (m_verbosity_level > 2) {
    std::cout << "Energy: " << totalEnergy << std::endl;
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::SetCotanSigma() {
  typename itk::ImageToVTKImageFilter < ImageType > ::Pointer itk2vtkConnector;
  m_Sampler->GetModifiedCotangentGradientFunction()->ClearGlobalSigma();
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    const itk::ParticleImageDomain < float,
    3 > * domain = static_cast < const itk::ParticleImageDomain < float,
    3 > * > (m_Sampler->GetParticleSystem()->GetDomain(i));

    itk2vtkConnector = itk::ImageToVTKImageFilter < ImageType > ::New();
    itk2vtkConnector->SetInput(domain->GetImage());
    vtkSmartPointer < vtkContourFilter > ls = vtkSmartPointer < vtkContourFilter > ::New();
    ls->SetInputData(itk2vtkConnector->GetOutput());
    ls->SetValue(0, 0.0);
    ls->Update();
    vtkSmartPointer < vtkMassProperties > mp = vtkSmartPointer < vtkMassProperties > ::New();
    mp->SetInputData(ls->GetOutput());
    mp->Update();
    double area = mp->GetSurfaceArea();
    double sigma = m_cotan_sigma_factor *
                   std::sqrt(area /
                             (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) * M_PI));
    m_Sampler->GetModifiedCotangentGradientFunction()->SetGlobalSigma(sigma);
  }
}

// File writers and info display functions
template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::PrintParamInfo() {

  if (m_verbosity_level < 2) {
    return;
  }

#ifdef SW_USE_OPENMP
  std::cout << "OpenMP is enabled ... \n" << std::flush;
#else
  std::cout << "OpenMP is disabled ... \n" << std::flush;
#endif
  // Write out the parameters
  std::cout << "---------------------" << std::endl;
  std::cout << "   I/O parameters" << std::endl;
  std::cout << "---------------------" << std::endl << std::endl;

  std::cout << "Domains per shape = " << m_domains_per_shape << std::endl;
  std::cout << m_filenames.size() << " image files provided!!!" << std::endl;

  if (m_d_flgs.size() > 0) {
    std::cout << "Following " << m_d_flgs.size() << " domains have been declared fixed: " <<
      std::endl;

    for (int i = 0; i < m_d_flgs.size(); i++) {
      std::cout << m_d_flgs[i] << "\t" << m_filenames[m_d_flgs[i]] << std::endl;
    }

    std::cout << std::endl;
  }

  if (m_adaptivity_mode == 3) {
    std::cout << std::endl << std::endl << "*****Using constraints on shapes*****" << std::endl;

    // distribution cutting plane

    if (m_distribution_domain_id > -1) {
      int i = 0;
      std::cout << std::endl << "Using distribution cutting plane: " << std::endl;
      std::cout << "(" << cpVals[i++] << "  " << cpVals[i++] << "  " << cpVals[i++] << ") ";
      std::cout << "(" << cpVals[i++] << "  " << cpVals[i++] << "  " << cpVals[i++] << ") ";
      std::cout << "(" << cpVals[i++] << "  " << cpVals[i++] << "  " << cpVals[i++] << ") ";
      std::cout << std::endl;
    }

    // cutting planes
    if (cpVals.size() > 0) {
      std::cout << std::endl << "Using individual cutting planes: " << std::endl;
      for (int i1 = 0; i1 < cpVals.size(); i1 += 9) {
        int i = i1;
        std::cout << "(" << cpVals[i++] << "  " << cpVals[i++] << "  " << cpVals[i++] << ") ";
        std::cout << "(" << cpVals[i++] << "  " << cpVals[i++] << "  " << cpVals[i++] << ") ";
        std::cout << "(" << cpVals[i++] << "  " << cpVals[i++] << "  " << cpVals[i++] << ") ";
        std::cout << std::endl;
      }
    }

    // cutting spheres
    if (radList.size() > 0) {
      std::cout << std::endl << "Using cutting spheres: " << std::endl;
      for (int i1 = 0; i1 < radList.size(); i1++) {
        int i = i1 * 3;
        std::cout << "center: (" << spVals[i++] << "  " << spVals[i++] << "  " << spVals[i++] <<
          "), radius: " << radList[i1] << std::endl;
      }
    }
  }

  std::cout << "Target number of particles = ";
  if (m_domains_per_shape == 1) {
    std::cout << m_number_of_particles[0];
  }
  else {
    for (unsigned int i = 0; i < this->m_domains_per_shape; i++) {
      std::cout << "domain " << i << " : " << m_number_of_particles[i] << ", ";
    }
  }
  std::cout << std::endl;

  if (m_p_flgs.size() > 0) {
    std::cout << "Total " << m_p_flgs.size() / 2 << " particles have been declared fixed." <<
      std::endl;
  }

  if (m_mesh_based_attributes) {
    std::cout << std::endl << std::endl << "*****Using attributes*****" << std::endl;

    std::cout << "Domain(s) using XYZ: ";
    for (int i = 0; i < m_domains_per_shape; i++) {
      if (m_use_xyz[i]) {
        std::cout << i << " ";
      }
    }
    std::cout << std::endl;

    std::cout << "Domain(s) using Normals: ";
    for (int i = 0; i < m_domains_per_shape; i++) {
      if (m_use_normals[i]) {
        std::cout << i << " ";
      }
    }
    std::cout << std::endl;

    if (this->m_attributes_per_domain.size() > 0) {
      std::cout << "Other attributes per domain:" << std::endl;
      for (int i = 0; i < m_domains_per_shape; i++) {
        std::cout << i << ":" << m_attributes_per_domain[i] << "  ";
      }
      std::cout << std::endl;
    }
  }

  if (m_transform_file.length() > 0) {
    std::cout << "m_transform_file = " << m_transform_file << std::endl;
  }
  if (m_prefix_transform_file.length() > 0) {
    std::cout << "m_prefix_transform_file = " << m_prefix_transform_file << std::endl;
  }

  std::cout << "Output path = " << m_output_dir << std::endl;
  std::cout << "Output transform filename = " << m_output_transform_file << std::endl;

  std::cout << std::endl;

  std::cout << "------------------------------" << std::endl;
  std::cout << "   Optimization parameters" << std::endl;
  std::cout << "------------------------------" << std::endl;

  std::cout << "Processing modes = ";
  if (m_processing_mode >= 0) {
    std::cout << "Initialization";
  }
  if ((m_processing_mode >= 1 || m_processing_mode == -1) && m_adaptivity_strength > 0.0) {
    std::cout << ", Adaptivity";
  }
  if (m_processing_mode >= 2 || m_processing_mode == -2) {
    std::cout << ", Optimization";
  }
  std::cout << std::endl;

  if (m_adaptivity_strength > 0.0) {
    std::cout << "adaptivity_strength = " << m_adaptivity_strength << std::endl;
  }

  std::cout << "pairwise_potential_type = ";
  if (m_pairwise_potential_type == 0) {
    std::cout << "gaussian" << std::endl;
  }
  else {
    std::cout << "cotan" << std::endl;
  }

  std::cout << "optimizer_type = ";
  if (m_optimizer_type == 0) {
    std::cout << "jacobi";
  }
  else if (m_optimizer_type == 1) {
    std::cout << "gauss seidel";
  }
  else if (m_optimizer_type == 2) {
    std::cout << "adaptive gauss seidel (with bad moves)";
  }
  else {
    std::cerr << "Incorrect option!!";
    throw 1;
  }
  std::cout << std::endl;

  std::cout << "m_optimization_iterations = " << m_optimization_iterations << std::endl;
  std::cout << "m_optimization_iterations_completed = " << m_optimization_iterations_completed <<
    std::endl;
  std::cout << "m_iterations_per_split = " << m_iterations_per_split << std::endl;
  std::cout << "m_init_criterion = " << m_init_criterion << std::endl;
  std::cout << "m_opt_criterion = " << m_opt_criterion << std::endl;
  std::cout << "m_use_shape_statistics_in_init = " << m_use_shape_statistics_in_init << std::endl;
  std::cout << "m_procrustes_interval = " << m_procrustes_interval << std::endl;
  std::cout << "m_procrustes_scaling = " << m_procrustes_scaling << std::endl;
  std::cout << "m_relative_weighting = " << m_relative_weighting << std::endl;
  std::cout << "m_initial_relative_weighting = " << m_initial_relative_weighting << std::endl;
  std::cout << "m_starting_regularization = " << m_starting_regularization << std::endl;
  std::cout << "m_ending_regularization = " << m_ending_regularization << std::endl;
  std::cout << "m_recompute_regularization_interval = " << m_recompute_regularization_interval <<
    std::endl;
  std::cout << "m_save_init_splits = " << m_save_init_splits << std::endl;
  std::cout << "m_checkpointing_interval = " << m_checkpointing_interval << std::endl;
  std::cout << "m_keep_checkpoints = " << m_keep_checkpoints << std::endl;

  std::cout << std::endl;

  if (m_performGoodBad) {
    std::cout <<
      "Debug: Bad particles will be reported during optimization, expect significant delays!!! " <<
      std::endl;
  }

  if (m_logEnergy) {
    std::cout << "Debug: Write energy logs, might increase runtime!!! " << std::endl;
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WriteTransformFile(int iter) const
{
  std::string output_file = m_output_dir + "/" + m_output_transform_file;

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
    std::stringstream ssp;
    ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles();     // size from domain 0
    output_file = m_output_dir + "/iter" + ss.str() + "_p" + ssp.str() + "/" +
                  m_output_transform_file;
  }
  this->WriteTransformFile(output_file);
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WriteTransformFile(std::string iter_prefix) const
{
  std::string output_file = iter_prefix;

  std::vector < itk::ParticleSystem < 3 > ::TransformType > tlist;

  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++) {
    tlist.push_back(m_Sampler->GetParticleSystem()->GetTransform(i));
  }

  std::string str = "writing " + output_file + " ...";
  startMessage(str);
  object_writer < itk::ParticleSystem < 3 > ::TransformType > writer;
  writer.SetFileName(output_file);
  writer.SetInput(tlist);
  writer.Update();
  doneMessage();
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WritePointFiles(int iter) {
  std::stringstream ss;
  ss << iter + m_optimization_iterations_completed;

  std::stringstream ssp;
  ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles();   // size from domain 0

  std::string out_path = m_output_dir;
  std::string tmp_dir_name = iter >=
                             0 ? out_path + "/iter" + ss.str() + "_p" + ssp.str() : out_path;

  this->WritePointFiles(tmp_dir_name);
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WritePointFiles(std::string iter_prefix) {
  this->startMessage("Writing point files...\n");
#ifdef _WIN32
  mkdir(iter_prefix.c_str());
#else
  mkdir(iter_prefix.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

  typedef  itk::MaximumEntropyCorrespondenceSampler < ImageType > ::PointType PointType;
  const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();

  int counter;
  for (int i = 0; i < n; i++) {
    counter = 0;

    std::string local_file = iter_prefix + "/" + m_filenames[i] + "_local.particles";
    std::string world_file = iter_prefix + "/" + m_filenames[i] + "_world.particles";

    std::ofstream out(local_file.c_str());
    std::ofstream outw(world_file.c_str());

    std::string str = "Writing " + world_file + " and " + local_file + " files...";
    this->startMessage(str, 1);
    if (!out) {
      std::cerr << "Error opening output file: " << local_file << std::endl;
      throw 1;
    }
    if (!outw) {
      std::cerr << "Error opening output file: " << world_file << std::endl;
      throw 1;
    }

    for (unsigned int j = 0; j < m_Sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      PointType pos = m_Sampler->GetParticleSystem()->GetPosition(j, i);
      PointType wpos = m_Sampler->GetParticleSystem()->GetTransformedPosition(j, i);

      for (unsigned int k = 0; k < 3; k++) {
        out << pos[k] << " ";
      }
      out << std::endl;

      for (unsigned int k = 0; k < 3; k++) {
        outw << wpos[k] << " ";
      }
      outw << std::endl;

      counter++;
    }      // end for points

    out.close();
    outw.close();

    std::stringstream st;
    st << counter;
    str = "with " + st.str() + "points...";
    this->startMessage(str, 1);
    this->doneMessage(1);
  }   // end for files
  this->doneMessage();
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WritePointFilesWithFeatures(int iter) {
  std::stringstream ss;
  ss << iter + m_optimization_iterations_completed;

  std::stringstream ssp;
  ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles();   // size from domain 0

  std::string out_path = m_output_dir;
  std::string tmp_dir_name = iter >=
                             0 ? out_path + "/iter" + ss.str() + "_p" + ssp.str() : out_path;

  this->WritePointFilesWithFeatures(tmp_dir_name);
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WritePointFilesWithFeatures(std::string iter_prefix) {
  if (!m_mesh_based_attributes) {
    return;
  }

  this->startMessage("Writing point with attributes files...\n");
  typedef  itk::MaximumEntropyCorrespondenceSampler < ImageType > ::PointType PointType;
  const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();

  int counter;
  for (int i = 0; i < n; i++) {
    counter = 0;

    std::string world_file = iter_prefix + "/" + m_filenames[i] + "_wptsFeatures.particles";

    std::ofstream outw(world_file.c_str());

    std::string str = "Writing " + world_file + "...";
    int attrNum = 3 * int(m_use_xyz[i % m_domains_per_shape]) + 3 *
                  int(m_use_normals[i % m_domains_per_shape]);
    if (m_attributes_per_domain.size() > 0) {
      attrNum += m_attributes_per_domain[i % m_domains_per_shape];
    }
    std::stringstream st;
    st << attrNum;
    str += "with " + st.str() + " attributes per point...";
    this->startMessage(str, 1);

    if (!outw) {
      std::cerr << "Error opening output file: " << world_file << std::endl;
      throw 1;
    }

    const itk::ParticleImplicitSurfaceDomain < float, 3 > * domain
      = static_cast < const itk::ParticleImplicitSurfaceDomain < float,
    3 > * > (m_Sampler->GetParticleSystem()->GetDomain(i));

    const itk::ParticleImageDomainWithGradients < float, 3 > * domainWithGrad
      = static_cast < const itk::ParticleImageDomainWithGradients < float,
    3 > * > (m_Sampler->GetParticleSystem()->GetDomain(i));

    TriMesh* ptr;
    std::vector < float > fVals;
    if (m_mesh_based_attributes && m_attributes_per_domain.size() > 0) {
      if (m_attributes_per_domain[i % m_domains_per_shape] > 0) {
        ptr = domain->GetMesh();
      }
    }

    for (unsigned int j = 0; j < m_Sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      PointType pos = m_Sampler->GetParticleSystem()->GetPosition(j, i);
      PointType wpos = m_Sampler->GetParticleSystem()->GetTransformedPosition(j, i);

      for (unsigned int k = 0; k < 3; k++) {
        outw << wpos[k] << " ";
      }

      if (m_use_normals[i % m_domains_per_shape]) {
//                if (m_Sampler->GetParticleSystem()->GetDomainFlag(i))
//                {
//                    outw << 0.0 << " " << 0.0 << " " << 0.0 << " ";
//                }
//                else
//                {
        typename itk::ParticleImageDomainWithGradients < float,
        3 > ::VnlVectorType pG = domainWithGrad->SampleNormalVnl(pos);
        VectorType pN;
        pN[0] = pG[0]; pN[1] = pG[1]; pN[2] = pG[2];
        pN = m_Sampler->GetParticleSystem()->TransformVector(pN,
                                                             m_Sampler->GetParticleSystem()->GetTransform(
                                                               i) * m_Sampler->GetParticleSystem()->GetPrefixTransform(
                                                               i));
        outw << pN[0] << " " << pN[1] << " " << pN[2] << " ";
//                }
      }

      if (m_attributes_per_domain.size() > 0) {
        if (m_attributes_per_domain[i % m_domains_per_shape] > 0) {
//                    if (m_Sampler->GetParticleSystem()->GetDomainFlag(i))
//                    {
//                        for (unsigned int k = 0; k < m_attributes_per_domain[i % m_domains_per_shape]; k++)
//                            outw << 0.0 << " ";
//                    }
//                    else
//                    {
          point pt;
          pt.clear();
          pt[0] = pos[0];
          pt[1] = pos[1];
          pt[2] = pos[2];
          fVals.clear();
          ptr->GetFeatureValues(pt, fVals);
          for (unsigned int k = 0; k < m_attributes_per_domain[i % m_domains_per_shape]; k++) {
            outw << fVals[k] << " ";
          }
//                    }
        }
      }

      outw << std::endl;

      counter++;
    }      // end for points

    outw.close();
    this->doneMessage(1);
  }   // end for files
  this->doneMessage();
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WriteEnergyFiles() {
  if (!this->m_logEnergy) {
    return;
  }
  this->startMessage("Writing energy files...\n");
  std::string strA = m_output_dir + "/" + this->m_strEnergy + "_samplingEnergy.txt";
  std::string strB = m_output_dir + "/" + this->m_strEnergy + "_correspondenceEnergy.txt";
  std::string strTotal = m_output_dir + "/" + this->m_strEnergy + "_totalEnergy.txt";
  std::ofstream outA(strA.c_str(), std::ofstream::app);
  std::ofstream outB(strB.c_str(), std::ofstream::app);
  std::ofstream outTotal(strTotal.c_str(), std::ofstream::app);

  if (!outA) {
    std::cerr << "Error opening output energy file: " << strA << std::endl;
    throw 1;
  }
  if (!outB) {
    std::cerr << "Error opening output energy file: " << strB << std::endl;
    throw 1;
  }
  if (!outTotal) {
    std::cerr << "Error opening output energy file: " << strTotal << std::endl;
    throw 1;
  }

  int n = m_EnergyA.size() - 1;
  n = n < 0 ? 0 : n;

  std::string str = "Appending to " + strA + " ...";
  this->startMessage(str, 1);
  outA << m_EnergyA[n] << std::endl;
  outA.close();
  this->doneMessage(1);

  str = "Appending to " + strB + " ...";
  this->startMessage(str, 1);
  outB << m_EnergyB[n] << std::endl;
  outB.close();
  this->doneMessage(1);

  str = "Appending to " + strTotal + " ...";
  this->startMessage(str, 1);
  outTotal << m_TotalEnergy[n] << std::endl;
  outTotal.close();
  this->doneMessage(1);

  this->doneMessage();
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WriteCuttingPlanePoints(int iter) {
  this->startMessage("Writing cutting plane points...\n");
  std::string output_file = m_output_cutting_plane_file;

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
    output_file = "./iter" + ss.str() + "/" + output_file;
  }

  std::ofstream out(output_file.c_str());

  std::string str = "Writing " + output_file + "...";
  this->startMessage(str, 1);

  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    const itk::ParticleImplicitSurfaceDomain < float, 3 > * dom
      = static_cast < const itk::ParticleImplicitSurfaceDomain < float
      , 3 > * > (m_Sampler->GetParticleSystem()->GetDomain(i));

    for (unsigned int j = 0; j < dom->GetNumberOfPlanes(); j++) {
      vnl_vector_fixed < double, 3 > a = dom->GetA(j);
      vnl_vector_fixed < double, 3 > b = dom->GetB(j);
      vnl_vector_fixed < double, 3 > c = dom->GetC(j);

      for (int d = 0; d < 3; d++) {
        out << a[d] << " ";
      }
      for (int d = 0; d < 3; d++) {
        out << b[d] << " ";
      }
      for (int d = 0; d < 3; d++) {
        out << c[d] << " ";
      }
      out << std::endl;
    }
  }
  out.close();
  this->doneMessage(1);
  this->doneMessage();
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::WriteParameters(int iter) {
  if (!m_use_regression) {
    return;
  }
  std::string slopename, interceptname;

  slopename = std::string(m_output_dir) + std::string("slope");
  interceptname = std::string(m_output_dir) + std::string("intercept");

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;

    slopename = "./.iter" + ss.str() + "/" + slopename;
    interceptname = "./.iter" + ss.str() + "/" + interceptname;
  }

  std::cout << "writing " << slopename << std::endl;
  std::cout << "writing " << interceptname << std::endl;

  std::vector < double > slope;
  std::vector < double > intercept;

  if (m_use_mixed_effects == true) {
    vnl_vector < double > slopevec = dynamic_cast < itk::ParticleShapeMixedEffectsMatrixAttribute <
                                     double, 3 > * >
    (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out(slopename.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << slope[i] << "\n";
    }
    out.close();

    vnl_vector < double > interceptvec = dynamic_cast <
                                         itk::ParticleShapeMixedEffectsMatrixAttribute < double,
    3 > * >
    (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->GetIntercept();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      intercept.push_back(interceptvec[i]);
    }

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << intercept[i] << "\n";
    }
    out.close();

    slopename = std::string(m_output_dir) + std::string("sloperand");
    interceptname = std::string(m_output_dir) + std::string("interceptrand");

    if (iter >= 0) {
      std::stringstream ss;
      ss << iter + m_optimization_iterations_completed;

      slopename = "./.iter" + ss.str() + "/" + slopename;
      interceptname = "./.iter" + ss.str() + "/" + interceptname;
    }

    std::cout << "writing " << slopename << std::endl;
    std::cout << "writing " << interceptname << std::endl;

    vnl_matrix < double > sloperand_mat = dynamic_cast <
                                          itk::ParticleShapeMixedEffectsMatrixAttribute < double,
    3 > * >
    (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->GetSlopeRandom();

    out.open(slopename.c_str());
    for (unsigned int i = 0; i < sloperand_mat.rows(); i++) {
      for (unsigned int j = 0; j < sloperand_mat.cols(); j++) {
        out << sloperand_mat.get(i, j) << " ";
      }
      out << "\n";
    }
    out.close();

    vnl_matrix < double > interceptrand_mat = dynamic_cast <
                                              itk::ParticleShapeMixedEffectsMatrixAttribute <
                                              double, 3 > * >
    (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())->GetInterceptRandom();

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < interceptrand_mat.rows(); i++) {
      for (unsigned int j = 0; j < interceptrand_mat.cols(); j++) {
        out << interceptrand_mat.get(i, j) << " ";
      }
      out << "\n";
    }
    out.close();
  }
  else {
    vnl_vector < double > slopevec = dynamic_cast <
                                     itk::ParticleShapeLinearRegressionMatrixAttribute < double,
    3 > * >
    (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out(slopename.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << slope[i] << "\n";
    }
    out.close();

    std::vector < double > intercept;
    vnl_vector < double > interceptvec = dynamic_cast <
                                         itk::ParticleShapeLinearRegressionMatrixAttribute < double,
    3 > * >
    (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())->GetIntercept();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      intercept.push_back(interceptvec[i]);
    }

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << intercept[i] << "\n";
    }
    out.close();
  }
}

template < class SAMPLERTYPE >
void
ShapeWorksRunApp < SAMPLERTYPE > ::ReportBadParticles() {
  this->startMessage("Reporting bad particles...", 2);
  typedef  itk::MaximumEntropyCorrespondenceSampler < ImageType > ::PointType PointType;
  const int totalDomains = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
  const int numShapes = totalDomains / m_domains_per_shape;
  std::string outDomDir;
  std::string outPtDir;
  if (this->m_badIds.empty()) {
    return;
  }
  for (int i = 0; i < this->m_domains_per_shape; i++) {
    if (this->m_badIds[i].empty()) {
      continue;
    }

    std::stringstream ss;
    ss << i;
    outDomDir = m_output_dir + "/" + this->m_strEnergy + "_BadParticles_domain" + ss.str();
#ifdef _WIN32
    mkdir(outDomDir.c_str());
#else
    mkdir(outDomDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    for (int j = 0; j < this->m_badIds[i].size(); j++) {
      std::stringstream ssj;
      ssj << m_badIds[i][j];
      outPtDir = outDomDir + "/particle" + ssj.str();
#ifdef _WIN32
      mkdir(outPtDir.c_str());
#else
      mkdir(outPtDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
      for (int k = 0; k < numShapes; k++) {
        int dom = k * m_domains_per_shape + i;
        std::string localPtFile = outPtDir + "/" + m_filenames[dom] + ".particles";
        std::ofstream outl(localPtFile.c_str(), std::ofstream::app);
        PointType pos = m_Sampler->GetParticleSystem()->GetPosition(m_badIds[i][j], dom);
        for (unsigned int d = 0; d < 3; d++) {
          outl << pos[d] << " ";
        }
        outl << std::endl;
        outl.close();
      }
    }
  }
  this->doneMessage(2);
}
