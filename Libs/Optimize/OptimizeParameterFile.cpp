#include <OptimizeParameterFile.h>
#include <Optimize.h>

#include <itkImageFileReader.h>

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

  this->verbosity_level_ = 5;
  elem = docHandle.FirstChild("verbosity").Element();
  if (elem) { this->verbosity_level_ = atoi(elem->GetText()); }
  optimize->SetVerbosity(this->verbosity_level_);

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

  if (!this->set_debug_parameters(&docHandle, optimize)) {
    return false;
  }

  if (!this->read_inputs(&docHandle, optimize)) {
    return false;
  }

  if (!this->read_mesh_inputs(&docHandle, optimize)) {
    return false;
  }

  if (!this->read_constraints(&docHandle, optimize)) {
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::set_io_parameters(TiXmlHandle* docHandle, Optimize* optimize)
{
  TiXmlElement* elem = nullptr;

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
  TiXmlElement* elem = nullptr;

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
bool OptimizeParameterFile::set_debug_parameters(TiXmlHandle* docHandle, Optimize* optimize)
{
  TiXmlElement* elem = nullptr;

  elem = docHandle->FirstChild("normal_angle").Element();
  if (elem) { optimize->SetNormalAngle(atof(elem->GetText()) * itk::Math::pi / 180.0);}

  elem = docHandle->FirstChild("report_bad_particles").Element();
  if (elem) { optimize->SetPerformGoodBad(static_cast<bool>(atoi(elem->GetText())));}

  elem = docHandle->FirstChild("log_energy").Element();
  if (elem) { optimize->SetLogEnergy(static_cast<bool>(atoi(elem->GetText())));}

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_inputs(TiXmlHandle* docHandle, Optimize* optimize)
{

  TiXmlElement* elem = nullptr;

  elem = docHandle->FirstChild("inputs").Element();
  if (!elem) {
    std::cerr << "No input files have been specified\n";
    return false;
  }

  std::istringstream inputsBuffer;
  std::string filename;
  int numShapes = 0;

  // load input shapes
  std::vector < std::string > shapeFiles;
  std::vector < Optimize::ImageType::Pointer > images;

  inputsBuffer.str(elem->GetText());
  while (inputsBuffer >> filename) {

    if (this->verbosity_level_ > 1) {
      std::cout << "Reading inputfile: " << filename << "...\n" << std::flush;
    }
    typename itk::ImageFileReader < Optimize::ImageType > ::Pointer reader = itk::ImageFileReader <
      Optimize::ImageType > ::New();
    reader->SetFileName(filename);
    reader->UpdateLargestPossibleRegion();
    images.push_back(reader->GetOutput());

    shapeFiles.push_back(filename);
  }

  inputsBuffer.clear();
  inputsBuffer.str("");

  numShapes = shapeFiles.size();

  optimize->SetImages(images);

  std::vector < std::string > filenames;

  for (int i = 0; i < numShapes; i++) {
    char* str = new char[shapeFiles[i].length() + 1];
    strcpy(str, shapeFiles[i].c_str());

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

    filenames.push_back(std::string(fname));
  }

  optimize->SetFilenames(filenames);

  // load point files
  std::vector <std::string> pointFiles;
  elem = docHandle->FirstChild("point_files").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> filename) {
      pointFiles.push_back(filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");

    // read point files only if they are all present
    if (pointFiles.size() != numShapes) {
      std::cerr << "ERROR: incorrect number of point files!" << std::endl;
      return false;
    }
    else {
      optimize->SetPointFiles(pointFiles);
    }
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_mesh_inputs(TiXmlHandle* docHandle, Optimize* optimize)
{
  TiXmlElement* elem = nullptr;
  std::istringstream inputsBuffer;
  std::string filename;
  int numShapes = optimize->GetNumShapes();

  // load mesh files
  elem = docHandle->FirstChild("mesh_files").Element();
  if (elem) {
    std::vector<std::string> meshFiles;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> filename) {
      meshFiles.push_back(filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");

    // read mesh files only if they are all present
    if (meshFiles.size() != numShapes) {
      std::cerr << "Error: incorrect number of mesh files!" << std::endl;
      return false;
    }
    else {
      optimize->SetMeshFiles(meshFiles);
    }
  }

  std::vector<int> attributes_per_domain = optimize->GetAttributesPerDomain();

  // attributes
  if ((attributes_per_domain.size() >= 1 &&
       *std::max_element(attributes_per_domain.begin(),
                         attributes_per_domain.end()) > 0) ||
      optimize->GetUseMeshBasedAttributes()) {
    // attribute scales
    double sc;
    std::vector < double > attr_scales;

    elem = docHandle->FirstChild("attribute_scales").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());

      while (inputsBuffer >> sc) {
        attr_scales.push_back(sc);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");
    }
    int numTot = 0;
    for (int i = 0; i < optimize->GetDomainsPerShape(); i++) {
      if (optimize->GetUseXYZ()[i]) {
        numTot += 3;
      }
      if (optimize->GetUseNormals()[i]) {
        numTot += 3;
      }
      if (attributes_per_domain.size() > 0) {
        numTot += attributes_per_domain[i];
      }
    }

    if (numTot != attr_scales.size()) {
      std::cerr << "not enough attribute scale values!!!" << std::endl;
      return false;
    }
    optimize->SetAttributeScales(attr_scales);

    // attribute files
    std::vector < std::string > attrFiles;
    elem = docHandle->FirstChild("attribute_files").Element();
    if (elem) {
      inputsBuffer.str(elem->GetText());

      while (inputsBuffer >> filename) {
        attrFiles.push_back(filename);
      }

      inputsBuffer.clear();
      inputsBuffer.str("");

      optimize->SetFeaFiles(attrFiles);
    }

    // need fids for mesh based fea
    if (optimize->GetUseMeshBasedAttributes()) {
      std::vector < std::string > feaGradFiles;
      elem = docHandle->FirstChild("attribute_grad_files").Element();
      if (elem) {
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> filename) {
          feaGradFiles.push_back(filename);
        }

        inputsBuffer.clear();
        inputsBuffer.str("");

        int totAttributes = std::accumulate(
          attributes_per_domain.begin(), attributes_per_domain.end(), 0);
        if (feaGradFiles.size() != numShapes * totAttributes / optimize->GetDomainsPerShape()) {
          std::cerr << "ERROR: Invalid number of attribute gradient files!!!" << std::endl;
          return false;
        }
        else {
          optimize->SetFeaGradFiles(feaGradFiles);
        }
      }
      else {
        if (attributes_per_domain.size() >= 1 &&
            *std::max_element(attributes_per_domain.begin(),
                              attributes_per_domain.end()) > 0) {
          std::cerr << "ERROR: No feature gradient files!!!" << std::endl;
          return false;
        }
      }

      std::vector < std::string > fidsFiles;
      elem = docHandle->FirstChild("fids").Element();
      if (elem) {
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> filename) {
          fidsFiles.push_back(filename);
        }

        inputsBuffer.clear();
        inputsBuffer.str("");
        if (fidsFiles.size() != numShapes) {
          std::cerr << "ERROR: Invalid number of fids files!!" << std::endl;
          return false;
        }
        else {
          optimize->SetFidsFiles(fidsFiles);
        }
      }
      else {
        if (attributes_per_domain.size() >= 1 &&
            *std::max_element(attributes_per_domain.begin(),
                              attributes_per_domain.end()) > 0) {
          std::cerr << "ERROR: Must provide fids!!" << std::endl;
          return false;
        }
      }
    }
  }
  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_constraints(TiXmlHandle* doc_handle, Optimize* optimize)
{
  if (optimize->GetDistributionDomainID() > -1) {
    if (!this->read_distribution_cutting_plane(doc_handle, optimize)) {
      return false;
    }
  }
  else {
    if (!this->read_cutting_planes(doc_handle, optimize)) {
      return false;
    }
  }

  return this->read_cutting_spheres(doc_handle, optimize);
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_distribution_cutting_plane(TiXmlHandle* doc_handle,
                                                            Optimize* optimize)
{
  TiXmlElement* elem = nullptr;

  std::istringstream inputsBuffer;
  int numShapes = optimize->GetNumShapes();

  // Distribution cutting planes activated
  elem = doc_handle->FirstChild("distribution_cutting_plane").Element();
  if (!elem) {
    return true;
  }

  inputsBuffer.str(elem->GetText());

  std::vector<double> cpVals;
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
    return false;
  }
  else {
    optimize->SetUseCuttingPlanes(true);
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
      if (optimize->GetPrefixTransformFile() != "" && optimize->GetTransformFile() != "") {
        itk::ParticleSystem < 3 > ::PointType pa;
        itk::ParticleSystem < 3 > ::PointType pb;
        itk::ParticleSystem < 3 > ::PointType pc;

        pa[0] = a[0]; pa[1] = a[1]; pa[2] = a[2];
        pb[0] = b[0]; pb[1] = b[1]; pb[2] = b[2];
        pc[0] = c[0]; pc[1] = c[1]; pc[2] = c[2];

        itk::ParticleSystem < 3 >::TransformType T =
          optimize->GetSampler()->GetParticleSystem()->GetTransform(
            shapeCount);
        itk::ParticleSystem < 3 >::TransformType prefT =
          optimize->GetSampler()->GetParticleSystem()->GetPrefixTransform(shapeCount);
        pa = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pa, T * prefT);
        pb = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pb, T * prefT);
        pc = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pc, T * prefT);

        a[0] = pa[0]; a[1] = pa[1]; a[2] = pa[2];
        b[0] = pb[0]; b[1] = pb[1]; b[2] = pb[2];
        c[0] = pc[0]; c[1] = pc[1]; c[2] = pc[2];
      }

      optimize->SetCuttingPlane(shapeCount, a, b, c);
      ctr = 0;             // use same cutting plane for all shapes
    }
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_cutting_planes(TiXmlHandle* docHandle, Optimize* optimize)
{
  TiXmlElement* elem;
  std::istringstream inputsBuffer;
  int numShapes = optimize->GetNumShapes();

  std::vector<int> cutting_planes_per_input;

  elem = docHandle->FirstChild("num_planes_per_input").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    double val;

    while (inputsBuffer >> val) {
      cutting_planes_per_input.push_back(val);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
    if (cutting_planes_per_input.size() != numShapes) {
      std::cerr <<
        "ERROR: Incomplete cutting plane data! Number of cutting planes for every input shape is required!!"
                << std::endl;
      throw 1;
    }
  }
  int numPlanes = std::accumulate(
    cutting_planes_per_input.begin(), cutting_planes_per_input.end(), 0);

  // otherwise read separate cutting plane for every shape
  elem = docHandle->FirstChild("cutting_planes").Element();
  if (!elem) {
    return true;
  }

  inputsBuffer.str(elem->GetText());

  std::vector<double> cpVals;

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
    optimize->SetUseCuttingPlanes(true);
    vnl_vector_fixed < double, 3 > a, b, c;
    int ctr = 0;

    for (int shapeCount = 0; shapeCount < numShapes; shapeCount++) {
      for (int planeCount = 0; planeCount < cutting_planes_per_input[shapeCount];
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
        if (optimize->GetPrefixTransformFile() != "" && optimize->GetTransformFile() != "") {
          itk::ParticleSystem < 3 > ::PointType pa;
          itk::ParticleSystem < 3 > ::PointType pb;
          itk::ParticleSystem < 3 > ::PointType pc;

          pa[0] = a[0]; pa[1] = a[1]; pa[2] = a[2];
          pb[0] = b[0]; pb[1] = b[1]; pb[2] = b[2];
          pc[0] = c[0]; pc[1] = c[1]; pc[2] = c[2];

          itk::ParticleSystem < 3 >::TransformType T =
            optimize->GetSampler()->GetParticleSystem()->GetTransform(
              shapeCount);
          itk::ParticleSystem < 3 >::TransformType prefT =
            optimize->GetSampler()->GetParticleSystem()->GetPrefixTransform(shapeCount);
          pa = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pa, T * prefT);
          pb = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pb, T * prefT);
          pc = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pc, T * prefT);

          a[0] = pa[0]; a[1] = pa[1]; a[2] = pa[2];
          b[0] = pb[0]; b[1] = pb[1]; b[2] = pb[2];
          c[0] = pc[0]; c[1] = pc[1]; c[2] = pc[2];
        }

        optimize->GetSampler()->SetCuttingPlane(shapeCount, a, b, c);
      }
    }
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_cutting_spheres(TiXmlHandle* doc_handle, Optimize* optimize)
{
  TiXmlElement* elem = nullptr;
  std::istringstream inputsBuffer;
  int numShapes = optimize->GetNumShapes();
  // sphere radii and centers
  std::vector<int> spheres_per_input;

  elem = doc_handle->FirstChild("spheres_per_domain").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    double val;

    while (inputsBuffer >> val) {
      spheres_per_input.push_back(val);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
    if (spheres_per_input.size() != numShapes) {
      std::cerr <<
        "ERROR: Incomplete cutting plane data! Number of cutting spheres for every input shape is required!!"
                << std::endl;
      return false;
    }
  }
  int numSpheres = std::accumulate(spheres_per_input.begin(), spheres_per_input.end(), 0);

  std::vector<double> radList;
  double r;

  elem = doc_handle->FirstChild("sphere_radii").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());

    while (inputsBuffer >> r) {
      radList.push_back(r);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");

    if (radList.size() != numSpheres) {
      std::cerr << "ERROR: Incomplete sphere radius data! No spheres will be loaded!!" <<
        std::endl;
      return false;
    }
    else {
      elem = doc_handle->FirstChild("sphere_centers").Element();
      if (elem) {
        inputsBuffer.str(elem->GetText());

        std::vector<double> spVals;
        double pt;

        while (inputsBuffer >> pt) {
          spVals.push_back(pt);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");

        if (spVals.size() < 3 * numSpheres) {
          std::cerr << "ERROR: Incomplete sphere center data! No spheres will be loaded!!" <<
            std::endl;
          return false;
        }
        else {
          vnl_vector_fixed < double, 3 > center;
          double rad;
          int c_ctr = 0;
          int r_ctr = 0;

          for (int shapeCount = 0; shapeCount < numShapes; shapeCount++) {
            for (int sphereCount = 0; sphereCount < spheres_per_input[shapeCount];
                 sphereCount++) {
              center[0] = spVals[c_ctr++];
              center[1] = spVals[c_ctr++];
              center[2] = spVals[c_ctr++];

              rad = radList[r_ctr++];

              optimize->GetSampler()->AddSphere(shapeCount, center, rad);
            }
          }
        }
      }
    }
  }
  return true;
}
//---------------------------------------------------------------------------
