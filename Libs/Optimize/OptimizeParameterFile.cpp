#include <OptimizeParameterFile.h>
#include <Optimize.h>

//---------------------------------------------------------------------------
OptimizeParameterFile::OptimizeParameterFile()
{}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::set_parameters(std::string filename, Optimize* optimize)
{
  TiXmlDocument doc(filename.c_str());
  bool loadOkay = doc.LoadFile();

  if (!loadOkay) {
    return false;
  }

  TiXmlHandle docHandle(&doc);
  TiXmlElement* elem;

  int verbosity_level = 5;
  elem = docHandle.FirstChild("verbosity").Element();
  if (elem) { verbosity_level = atoi(elem->GetText()); }
  optimize->SetVerbosity(verbosity_level);

  int domains_per_shape = 1;
  elem = docHandle.FirstChild("domains_per_shape").Element();
  if (elem) {
    domains_per_shape = atoi(elem->GetText());
  }
  optimize->SetDomainsPerShape(domains_per_shape);

  std::vector<unsigned int> number_of_particles;
  elem = docHandle.FirstChild("number_of_particles").Element();
  if (elem) {
    std::istringstream inputsBuffer;
    std::string num;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> num) {
      number_of_particles.push_back(atoi(num.c_str()));
    }

    /// TODO: Not sure we need to check this here as it will be checked by Optimize anyway
    if (domains_per_shape != number_of_particles.size()) {
      std::cerr <<
        "Inconsistency in parameters... m_domains_per_shape != m_number_of_particles.size()" <<
        std::endl;
      return false;
    }

    optimize->SetNumberOfParticles(number_of_particles);
  }
  else {
    std::cerr << "Number of particles not specified" << std::endl;
    return false;
  }

  if (!this->set_io_parameters(&docHandle, optimize)) {
    return false;
  }

  if (!this->set_optimization_parameters(&docHandle, optimize)) {
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::set_io_parameters(TiXmlHandle* docHandle, Optimize* optimize)
{
  TiXmlElement* elem;
  // transform file
  std::string transform_file = "";
  elem = docHandle->FirstChild("transform_file").Element();
  if (elem) { transform_file = elem->GetText();}
  optimize->SetTransformFile(transform_file);

  // prefix transform file
  std::string prefix_transform_file = "";
  elem = docHandle->FirstChild("prefix_transform_file").Element();
  if (elem) { prefix_transform_file = elem->GetText();}
  optimize->SetPrefixTransformFile(prefix_transform_file);

  // output dir
  std::string output_dir = "";
  elem = docHandle->FirstChild("output_dir").Element();
  if (elem) { output_dir = elem->GetText();}
  optimize->SetOutputDir(output_dir);

  // output transform file
  std::string output_transform_file = "transform";
  elem = docHandle->FirstChild("output_transform_file").Element();
  if (elem) { output_transform_file = elem->GetText();}
  optimize->SetOutputTransformFile(output_transform_file);

  // mesh based attributes
  bool use_mesh_based_attributes = false;
  std::vector<bool> use_xyz;
  std::vector<bool> use_normals;

  elem = docHandle->FirstChild("mesh_based_attributes").Element();
  if (elem) { use_mesh_based_attributes = (bool) atoi(elem->GetText());}
  if (use_mesh_based_attributes) {
    elem = docHandle->FirstChild("use_xyz").Element();
    if (elem) {
      std::istringstream inputsBuffer;
      std::string num;
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> num) {
        use_xyz.push_back((bool) atoi(num.c_str()));
      }

      if (optimize->GetDomainsPerShape() != use_xyz.size()) {
        std::cerr << "Inconsistency in parameters... m_domains_per_shape != m_use_xyz.size()" <<
          std::endl;
        return false;
      }
    }

    elem = docHandle->FirstChild("use_normals").Element();
    if (elem) {
      std::istringstream inputsBuffer;
      std::string num;
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> num) {
        use_normals.push_back((bool) atoi(num.c_str()));
      }

      if (optimize->GetDomainsPerShape() != use_normals.size()) {
        std::cerr <<
          "Inconsistency in parameters... m_domains_per_shape != m_use_normals.size()" <<
          std::endl;
        return false;
      }
    }
  }

  optimize->SetUseMeshBasedAttributes(use_mesh_based_attributes);
  optimize->SetUseXYZ(use_xyz);
  optimize->SetUseNormals(use_normals);

  // attributes per domain
  std::vector<int> attributes_per_domain;
  elem = docHandle->FirstChild("attributes_per_domain").Element();
  if (elem) {
    std::istringstream inputsBuffer;
    std::string num;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> num) {
      attributes_per_domain.push_back(atoi(num.c_str()));
    }

    if (optimize->GetDomainsPerShape() != attributes_per_domain.size()) {
      std::cerr <<
        "Inconsistency in parameters... m_domains_per_shape != m_attributes_per_domain.size()" <<
        std::endl;
      return false;
    }
  }
  optimize->SetAttributesPerDomain(attributes_per_domain);

  // distribution domain id
  int distribution_domain_id = -1;
  elem = docHandle->FirstChild("distribution_plane_id").Element();
  if (elem) { distribution_domain_id = atoi(elem->GetText());}
  optimize->SetDistributionDomainID(distribution_domain_id);

  // output cutting plane file
  std::string output_cutting_plane_file = "";
  elem = docHandle->FirstChild("output_cutting_plane_file").Element();
  if (elem) { output_cutting_plane_file = elem->GetText();}
  optimize->SetOutputCuttingPlaneFile(output_cutting_plane_file);

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::set_optimization_parameters(TiXmlHandle* docHandle, Optimize* optimize)
{
  TiXmlElement* elem;

  elem = docHandle->FirstChild("processing_mode").Element();
  if (elem) { optimize->SetProcessingMode(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("adaptivity_mode").Element();
  if (elem) { optimize->SetAdaptivityMode(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("adaptivity_strength").Element();
  if (elem) { optimize->SetAdaptivityStrength(atof(elem->GetText()));}

  elem = docHandle->FirstChild("pairwise_potential_type").Element();
  if (elem) { optimize->SetPairwisePotentialType(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("optimizer_type").Element();
  if (elem) { optimize->SetOptimizerType(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("timepts_per_subject").Element();
  if (elem) { optimize->SetTimePtsPerSubject(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("optimization_iterations").Element();
  if (elem) { optimize->SetOptimizationIterations(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("optimization_iterations_completed").Element();
  if (elem) { optimize->SetOptimizationIterationsCompleted(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("iterations_per_split").Element();
  if (elem) { optimize->SetIterationsPerSplit(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("init_criterion").Element();
  if (elem) { optimize->SetInitCriterion(atof(elem->GetText()));}

  elem = docHandle->FirstChild("opt_criterion").Element();
  if (elem) { optimize->SetOptCriterion(atof(elem->GetText()));}

  elem = docHandle->FirstChild("use_shape_statistics_in_init").Element();
  if (elem) { optimize->SetUseShapeStatisticsInInit((bool) atoi(elem->GetText()));}

  elem = docHandle->FirstChild("procrustes_interval").Element();
  if (elem) { optimize->SetProcrustesInterval(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("procrustes_scaling").Element();
  if (elem) { optimize->SetProcrustesScaling(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("relative_weighting").Element();
  if (elem) { optimize->SetRelativeWeighting(atof(elem->GetText()));}

  elem = docHandle->FirstChild("initial_relative_weighting").Element();
  if (elem) { optimize->SetInitialRelativeWeighting(atof(elem->GetText()));}

  elem = docHandle->FirstChild("starting_regularization").Element();
  if (elem) { optimize->SetStartingRegularization(atof(elem->GetText()));}

  elem = docHandle->FirstChild("ending_regularization").Element();
  if (elem) { optimize->SetEndingRegularization(atof(elem->GetText()));}

  elem = docHandle->FirstChild("recompute_regularization_interval").Element();
  if (elem) { optimize->SetRecomputeRegularizationInterval(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("save_init_splits").Element();
  if (elem) { optimize->SetSaveInitSplits((bool) atoi(elem->GetText()));}

  elem = docHandle->FirstChild("checkpointing_interval").Element();
  if (elem) { optimize->SetCheckpointingInterval(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("keep_checkpoints").Element();
  if (elem) { optimize->SetKeepCheckpoints(atoi(elem->GetText()));}

  elem = docHandle->FirstChild("cotan_sigma_factor").Element();
  if (elem) { optimize->SetCotanSigmaFactor(atof(elem->GetText()));}

  return true;
}
//---------------------------------------------------------------------------
