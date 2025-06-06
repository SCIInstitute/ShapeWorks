#include "OptimizeParameterFile.h"

#include <Mesh/MeshUtils.h>
#include <Utils/StringUtils.h>
#include <itkImageFileReader.h>
#include <vtkPLYReader.h>

#include "ExternalLibs/tinyxml/tinyxml.h"
#include "Libs/Optimize/Domain/DomainType.h"
#include "Libs/Optimize/Domain/Surface.h"
#include "Optimize.h"

namespace shapeworks {

//---------------------------------------------------------------------------
OptimizeParameterFile::OptimizeParameterFile() {}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::load_parameter_file(std::string filename, Optimize* optimize) {
  TiXmlDocument doc(filename.c_str());
  bool loadOkay = doc.LoadFile();

  if (!loadOkay) {
    std::cerr << "Could not parse XML\n";
    return false;
  }

  TiXmlHandle doc_handle(&doc);
  TiXmlElement* elem;

  this->verbosity_level_ = 5;
  elem = doc_handle.FirstChild("verbosity").Element();
  if (elem) {
    this->verbosity_level_ = atoi(elem->GetText());
  }
  optimize->SetVerbosity(this->verbosity_level_);

  int domains_per_shape = 1;
  elem = doc_handle.FirstChild("domains_per_shape").Element();
  if (elem) {
    domains_per_shape = atoi(elem->GetText());
  }
  optimize->SetDomainsPerShape(domains_per_shape);

  shapeworks::DomainType domain_type = shapeworks::DomainType::Image;
  elem = doc_handle.FirstChild("domain_type").Element();
  if (elem) {
    std::string text = elem->GetText();
    if (text == "image") {
      domain_type = shapeworks::DomainType::Image;
    } else if (text == "mesh") {
      domain_type = shapeworks::DomainType::Mesh;
    } else if (text == "contour") {
      domain_type = shapeworks::DomainType::Contour;
    }
  }
  optimize->SetDomainType(domain_type);

  if (optimize->GetDomainType() == shapeworks::DomainType::Mesh) {
    if (!this->set_visualizer_parameters(&doc_handle, optimize)) {
      return false;
    }
  }

  std::vector<int> number_of_particles;
  elem = doc_handle.FirstChild("number_of_particles").Element();
  if (elem) {
    std::istringstream inputsBuffer;
    std::string num;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> num) {
      number_of_particles.push_back(atoi(num.c_str()));
    }

    /// TODO: Not sure we need to check this here as it will be checked by Optimize anyway
    if (domains_per_shape != number_of_particles.size()) {
      std::cerr << "Inconsistency in parameters... m_domains_per_shape != m_number_of_particles.size()" << std::endl;
      return false;
    }

    optimize->SetNumberOfParticles(number_of_particles);
  } else {
    std::cerr << "Number of particles not specified" << std::endl;
    return false;
  }

  if (!this->set_io_parameters(&doc_handle, optimize)) {
    return false;
  }

  if (!this->set_optimization_parameters(&doc_handle, optimize)) {
    return false;
  }

  if (!this->set_debug_parameters(&doc_handle, optimize)) {
    return false;
  }

  if (!this->read_explanatory_variables(&doc_handle, optimize)) {
    return false;
  }

  if (!this->read_flag_particles(&doc_handle, optimize)) {
    return false;
  }

  // Reads constraints
  if (!this->read_constraints(&doc_handle, optimize)) {
    return false;
  }

  if (!this->read_flag_domains(&doc_handle, optimize)) {
    return false;
  }
  // read last so that we can skip loading any images from fixed domains
  if (optimize->GetDomainType() == shapeworks::DomainType::Image) {
    if (!this->read_image_inputs(&doc_handle, optimize)) {
      return false;
    }
    if (!this->read_mesh_attributes(&doc_handle, optimize)) {
      return false;
    }
  } else if (optimize->GetDomainType() == shapeworks::DomainType::Mesh) {
    if (!this->read_mesh_inputs(&doc_handle, optimize)) {
      return false;
    }
    if (!this->read_mesh_attributes(&doc_handle, optimize)) {
      return false;
    }
  } else if (optimize->GetDomainType() == shapeworks::DomainType::Contour) {
    if (!this->read_contour_inputs(&doc_handle, optimize)) {
      return false;
    }
    if (!this->read_mesh_attributes(&doc_handle, optimize)) {
      return false;
    }
  }
  if (!this->read_point_files(&doc_handle, optimize)) {
    return false;
  }

  return true;
}

bool OptimizeParameterFile::set_visualizer_parameters(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;
  // Currently the visualizer only works if you call AddMesh on it for every domain.
  // In order to get it working for image domains, need to add code that extracts meshes from each image and adds them
  // to the visualizer_.
  elem = docHandle->FirstChild("visualizer_enable").Element();
  if (elem) {
    optimize->SetShowVisualizer((bool)atoi(elem->GetText()));

    elem = docHandle->FirstChild("visualizer_wireframe").Element();
    if (elem) {
      optimize->GetVisualizer().SetWireFrame((bool)atoi(elem->GetText()));
    }
    elem = docHandle->FirstChild("visualizer_screenshot_directory").Element();
    if (elem) {
      std::cout << "WARNING Saving screenshots will increase run time even more!\n";
      std::string dir = elem->GetText();
      optimize->GetVisualizer().SetSaveScreenshots(true, dir);
    }
  }
  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::set_io_parameters(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;

  // transform file
  std::string transform_file = "";
  elem = docHandle->FirstChild("transform_file").Element();
  if (elem) {
    transform_file = elem->GetText();
  }
  optimize->SetTransformFile(transform_file);

  // prefix transform file
  std::string prefix_transform_file = "";
  elem = docHandle->FirstChild("prefix_transform_file").Element();
  if (elem) {
    prefix_transform_file = elem->GetText();
  }
  optimize->SetPrefixTransformFile(prefix_transform_file);

  // output dir
  std::string output_dir = "";
  elem = docHandle->FirstChild("output_dir").Element();
  if (elem) {
    output_dir = elem->GetText();
  }
  optimize->SetOutputDir(output_dir);

  // output transform file
  std::string output_transform_file = "transform";
  elem = docHandle->FirstChild("output_transform_file").Element();
  if (elem) {
    output_transform_file = elem->GetText();
  }
  optimize->SetOutputTransformFile(output_transform_file);

  // write individual transform files
  bool write_individual_output_transforms = false;
  elem = docHandle->FirstChild("write_transform_files").Element();
  if (elem) {
    write_individual_output_transforms = (bool)atoi(elem->GetText());
  }
  optimize->SetOutputIndividualTransformFiles(write_individual_output_transforms);

  // python filename
  std::string python_file = "";
  elem = docHandle->FirstChild("python_filename").Element();
  if (elem) {
    python_file = elem->GetText();
  }
  optimize->SetPythonFile(python_file);

  // mesh based attributes
  bool use_mesh_based_attributes = false;
  std::vector<bool> use_xyz;
  std::vector<bool> use_normals;

  elem = docHandle->FirstChild("mesh_based_attributes").Element();
  if (elem) {
    use_mesh_based_attributes = (bool)atoi(elem->GetText());
  }
  if (use_mesh_based_attributes) {
    elem = docHandle->FirstChild("use_xyz").Element();
    if (elem) {
      std::istringstream inputsBuffer;
      std::string num;
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> num) {
        use_xyz.push_back((bool)atoi(num.c_str()));
      }

      if (optimize->GetDomainsPerShape() != use_xyz.size()) {
        std::cerr << "Inconsistency in parameters... m_domains_per_shape != m_use_xyz.size()" << std::endl;
        return false;
      }
    }

    elem = docHandle->FirstChild("use_normals").Element();
    if (elem) {
      std::istringstream inputsBuffer;
      std::string num;
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> num) {
        use_normals.push_back((bool)atoi(num.c_str()));
      }

      if (optimize->GetDomainsPerShape() != use_normals.size()) {
        std::cerr << "Inconsistency in parameters... m_domains_per_shape != m_use_normals.size()" << std::endl;
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
      std::cerr << "Inconsistency in parameters... m_domains_per_shape != m_attributes_per_domain.size()" << std::endl;
      return false;
    }
  }
  optimize->SetAttributesPerDomain(attributes_per_domain);

  // distribution domain id
  int distribution_domain_id = -1;
  elem = docHandle->FirstChild("distribution_plane_id").Element();
  if (elem) {
    distribution_domain_id = atoi(elem->GetText());
  }
  optimize->SetDistributionDomainID(distribution_domain_id);

  // output cutting plane file
  std::string output_cutting_plane_file = "";
  elem = docHandle->FirstChild("output_cutting_plane_file").Element();
  if (elem) {
    output_cutting_plane_file = elem->GetText();
  }
  optimize->SetOutputCuttingPlaneFile(output_cutting_plane_file);

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::set_optimization_parameters(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;

  elem = docHandle->FirstChild("timepts_per_subject").Element();
  if (elem) {
    optimize->SetTimePtsPerSubject(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("optimization_iterations").Element();
  if (elem) {
    optimize->SetOptimizationIterations(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("optimization_iterations_completed").Element();
  if (elem) {
    optimize->SetOptimizationIterationsCompleted(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("iterations_per_split").Element();
  if (elem) {
    optimize->SetIterationsPerSplit(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("use_shape_statistics_in_init").Element();
  if (elem) {
    optimize->SetUseShapeStatisticsInInit((bool)atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("procrustes_interval").Element();
  if (elem) {
    optimize->SetProcrustesInterval(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("procrustes_scaling").Element();
  if (elem) {
    optimize->SetProcrustesScaling(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("procrustes_rotation_translation").Element();
  if (elem) {
    optimize->SetProcrustesRotationTranslation(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("relative_weighting").Element();
  if (elem) {
    optimize->SetRelativeWeighting(atof(elem->GetText()));
  }

  elem = docHandle->FirstChild("initial_relative_weighting").Element();
  if (elem) {
    optimize->SetInitialRelativeWeighting(atof(elem->GetText()));
  }

  elem = docHandle->FirstChild("starting_regularization").Element();
  if (elem) {
    optimize->SetStartingRegularization(atof(elem->GetText()));
  }

  elem = docHandle->FirstChild("ending_regularization").Element();
  if (elem) {
    optimize->SetEndingRegularization(atof(elem->GetText()));
  }

  elem = docHandle->FirstChild("recompute_regularization_interval").Element();
  if (elem) {
    optimize->SetRecomputeRegularizationInterval(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("save_init_splits").Element();
  if (elem) {
    optimize->SetSaveInitSplits((bool)atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("checkpointing_interval").Element();
  if (elem) {
    optimize->SetCheckpointingInterval(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("keep_checkpoints").Element();
  if (elem) {
    optimize->SetKeepCheckpoints(atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("narrow_band").Element();
  if (elem) {
    optimize->SetNarrowBand(atof(elem->GetText()));
  }

  elem = docHandle->FirstChild("use_shape_statistics_after").Element();
  if (elem) {
    optimize->SetUseShapeStatisticsAfter(atof(elem->GetText()));
  }

  elem = docHandle->FirstChild("geodesics_enabled").Element();
  if (elem) {
    optimize->SetGeodesicsEnabled((bool)atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("geodesics_cache_size_multiplier").Element();
  if (elem) {
    optimize->SetGeodesicsCacheSizeMultiplier((size_t)atol(elem->GetText()));
  }

  elem = docHandle->FirstChild("mesh_ffc_mode").Element();
  if (elem) {
    optimize->SetMeshFFCMode((bool)atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("shared_boundary_enabled").Element();
  if (elem) {
    optimize->SetSharedBoundaryEnabled((bool)atoi(elem->GetText()));
  }

  elem = docHandle->FirstChild("shared_boundary_weight").Element();
  if (elem) {
    optimize->SetSharedBoundaryWeight(atof(elem->GetText()));
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::set_debug_parameters(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;

  elem = docHandle->FirstChild("normal_angle").Element();
  if (elem) {
    optimize->SetNormalAngle(atof(elem->GetText()) * itk::Math::pi / 180.0);
  }

  elem = docHandle->FirstChild("report_bad_particles").Element();
  if (elem) {
    optimize->SetPerformGoodBad(static_cast<bool>(atoi(elem->GetText())));
  }

  elem = docHandle->FirstChild("log_energy").Element();
  if (elem) {
    optimize->SetLogEnergy(static_cast<bool>(atoi(elem->GetText())));
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_image_inputs(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;

  elem = docHandle->FirstChild("inputs").Element();
  if (!elem) {
    std::cerr << "No input images have been specified\n";
    return false;
  }

  std::istringstream inputsBuffer;

  inputsBuffer.str(elem->GetText());
  auto flags = optimize->GetDomainFlags();

  // load input images
  std::vector<std::string> imageFiles;
  std::string imagefilename;
  while (inputsBuffer >> imagefilename) {
    imageFiles.push_back(imagefilename);
  }

  for (int index = 0; index < imageFiles.size(); index++) {
    bool fixed_domain = false;
    for (int i = 0; i < flags.size(); i++) {
      if (flags[i] == index) {
        fixed_domain = true;
      }
    }

    if (!fixed_domain) {
      if (this->verbosity_level_ > 1) {
        std::cout << "Reading inputfile: " << imageFiles[index] << "...\n" << std::flush;
      }
      typename itk::ImageFileReader<Optimize::ImageType>::Pointer reader =
          itk::ImageFileReader<Optimize::ImageType>::New();
      reader->SetFileName(imageFiles[index]);
      reader->UpdateLargestPossibleRegion();
      const auto image = reader->GetOutput();
      optimize->AddImage(image, imageFiles[index]);
    } else {
      optimize->AddImage(nullptr);
    }
  }

  inputsBuffer.clear();
  inputsBuffer.str("");

  optimize->SetFilenames(StringUtils::getFileNamesFromPaths(imageFiles));
  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_mesh_inputs(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;

  elem = docHandle->FirstChild("inputs").Element();
  if (!elem) {
    std::cerr << "No input meshes have been specified\n";
    return false;
  }

  std::istringstream inputsBuffer;

  inputsBuffer.str(elem->GetText());
  auto flags = optimize->GetDomainFlags();

  // load input images
  std::vector<std::string> meshFiles;
  std::string meshfilename;
  while (inputsBuffer >> meshfilename) {
    meshFiles.push_back(meshfilename);
  }

  // passing cutting plane constraints
  // planes dimensions [number_of_inputs, planes_per_input, normal/point]
  std::vector<std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>>> planes =
      optimize->GetSampler()->ComputeCuttingPlanes();
  auto ffcs = optimize->GetSampler()->GetFFCs();

  for (int index = 0; index < meshFiles.size(); index++) {
    bool fixed_domain = false;
    for (int i = 0; i < flags.size(); i++) {
      if (flags[i] == index) {
        fixed_domain = true;
      }
    }

    if (!fixed_domain) {
      if (this->verbosity_level_ > 1) {
        std::cout << "Reading inputfile: " << meshFiles[index] << "...\n" << std::flush;
      }


      Mesh mesh = MeshUtils::threadSafeReadMesh(meshFiles[index].c_str());

      if (index < planes.size()) {
        for (size_t i = 0; i < planes[index].size(); i++) {
          // clip by plane
          vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
          plane->SetNormal(planes[index][i].first[0], planes[index][i].first[1], planes[index][i].first[2]);
          plane->SetOrigin(planes[index][i].second[0], planes[index][i].second[1], planes[index][i].second[2]);
          mesh.clip(plane);
        }
      }

      if (optimize->GetMeshFFCMode() == 0) {
        if (this->verbosity_level_ > 1) {
          std::cout << "ffcssize " << ffcs.size() << std::endl;
        }
        if (index < ffcs.size()) {
          ffcs[index].applyToPolyData(mesh.getVTKMesh());
          mesh = Mesh(mesh.clipByField("ffc_paint", 0.0));
        }
      }

      auto poly_data = mesh.getVTKMesh();

      if (poly_data) {
        optimize->AddMesh(poly_data);
      } else {
        std::cerr << "Failed to read " << meshFiles[index] << "\n";
        return false;
      }

      /*
      if (optimize->GetShowVisualizer()) {
        vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
        reader->SetFileName(meshFiles[index].c_str());
        reader->Update();
        optimize->GetVisualizer().AddMesh(reader->GetOutput(), themesh);
      }
       */
    } else {
      optimize->AddMesh(nullptr);
    }
  }

  optimize->SetFilenames(StringUtils::getFileNamesFromPaths(meshFiles));

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_contour_inputs(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;

  elem = docHandle->FirstChild("inputs").Element();
  if (!elem) {
    std::cerr << "No input contours have been specified\n";
    return false;
  }

  std::istringstream inputsBuffer;

  inputsBuffer.str(elem->GetText());
  auto flags = optimize->GetDomainFlags();

  std::vector<std::string> contourFiles;
  std::string contourfilename;
  while (inputsBuffer >> contourfilename) {
    contourFiles.push_back(contourfilename);
  }

  for (int index = 0; index < contourFiles.size(); index++) {
    bool fixed_domain = false;
    for (int i = 0; i < flags.size(); i++) {
      if (flags[i] == index) {
        fixed_domain = true;
      }
    }

    if (!fixed_domain) {
      if (this->verbosity_level_ > 1) {
        std::cout << "Reading inputfile: " << contourFiles[index] << "...\n" << std::flush;
      }

      auto poly_data = MeshUtils::threadSafeReadMesh(contourFiles[index].c_str()).getVTKMesh();
      if (poly_data) {
        optimize->AddContour(poly_data);
      } else {
        std::cerr << "Failed to read " << contourFiles[index] << "\n";
        return false;
      }
    } else {
      optimize->AddContour(nullptr);
    }
  }

  optimize->SetFilenames(StringUtils::getFileNamesFromPaths(contourFiles));

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_point_files(TiXmlHandle* docHandle, Optimize* optimize) {
  // load point files
  TiXmlElement* elem = nullptr;
  std::istringstream inputsBuffer;
  std::vector<std::string> pointFiles;
  elem = docHandle->FirstChild("point_files").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    std::string pointsfilename;
    while (inputsBuffer >> pointsfilename) {
      pointFiles.push_back(pointsfilename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");

    // read point files only if they are all present
    if (pointFiles.size() != optimize->GetNumShapes()) {
      std::cerr << "ERROR: incorrect number of point files!" << std::endl;
      return false;
    } else {
      optimize->SetPointFiles(pointFiles);
    }
  }
  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_mesh_attributes(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;
  std::istringstream inputsBuffer;
  std::string filename;
  int numShapes = optimize->GetNumShapes();

  std::vector<int> attributes_per_domain = optimize->GetAttributesPerDomain();

  // attributes
  if ((attributes_per_domain.size() >= 1 &&
       *std::max_element(attributes_per_domain.begin(), attributes_per_domain.end()) > 0) ||
      optimize->GetUseMeshBasedAttributes()) {
    // attribute scales
    double sc;
    std::vector<double> attr_scales;

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
  }
  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_constraints(TiXmlHandle* doc_handle, Optimize* optimize) {
  if (optimize->GetDistributionDomainID() > -1) {
    if (!this->read_distribution_cutting_plane(doc_handle, optimize)) {
      return false;
    }
  } else {
    if (!this->read_cutting_planes(doc_handle, optimize)) {
      return false;
    }
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_distribution_cutting_plane(TiXmlHandle* doc_handle, Optimize* optimize) {
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
    std::cerr << "ERROR: Incomplete cutting plane data for reference shape! No cutting planes will be loaded!!"
              << std::endl;
    return false;
  } else {
    optimize->SetUseCuttingPlanes(true);
    vnl_vector_fixed<double, 3> a, b, c;
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
        ParticleSystem::PointType pa;
        ParticleSystem::PointType pb;
        ParticleSystem::PointType pc;

        pa[0] = a[0];
        pa[1] = a[1];
        pa[2] = a[2];
        pb[0] = b[0];
        pb[1] = b[1];
        pb[2] = b[2];
        pc[0] = c[0];
        pc[1] = c[1];
        pc[2] = c[2];

        ParticleSystem::TransformType T = optimize->GetSampler()->GetParticleSystem()->GetTransform(shapeCount);
        ParticleSystem::TransformType prefT =
            optimize->GetSampler()->GetParticleSystem()->GetPrefixTransform(shapeCount);
        pa = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pa, T * prefT);
        pb = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pb, T * prefT);
        pc = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pc, T * prefT);

        a[0] = pa[0];
        a[1] = pa[1];
        a[2] = pa[2];
        b[0] = pb[0];
        b[1] = pb[1];
        b[2] = pb[2];
        c[0] = pc[0];
        c[1] = pc[1];
        c[2] = pc[2];
      }

      optimize->SetCuttingPlane(shapeCount, a, b, c);
      ctr = 0;  // use same cutting plane for all shapes
    }
  }

  return true;
}

int OptimizeParameterFile::get_num_inputs(TiXmlHandle* docHandle) {
  // Works for any type of domain
  // list inputs to figure out number
  TiXmlElement* elem = nullptr;

  elem = docHandle->FirstChild("inputs").Element();
  if (!elem) {
    std::cerr << "No input domains have been specified\n";
    return 0;
  }

  std::istringstream buffer;

  buffer.str(elem->GetText());
  // load input domains
  std::vector<std::string> inputs;
  std::string filename;
  while (buffer >> filename) {
    inputs.push_back(filename);
  }

  int domains_per_shape = 1;
  elem = docHandle->FirstChild("domains_per_shape").Element();
  if (elem) {
    domains_per_shape = atoi(elem->GetText());
  }

  if (inputs.size() % domains_per_shape != 0) {
    std::string message = "Error, number of inputs must be a multiple of domains_per_shape";
    throw std::invalid_argument(message);
  }

  return inputs.size();
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_cutting_planes(TiXmlHandle* docHandle, Optimize* optimize) {
  TiXmlElement* elem;
  std::istringstream inputsBuffer;
  // int numShapes = optimize->GetNumShapes();
  int numShapes = this->get_num_inputs(docHandle);

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
      std::cerr << "ERROR: Incomplete cutting plane data! Number of cutting planes for every input shape is required!!"
                << std::endl;
      throw 1;
    }
  }
  int numPlanes = std::accumulate(cutting_planes_per_input.begin(), cutting_planes_per_input.end(), 0);

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
    std::cerr << "ERROR: Incomplete cutting plane data! No cutting planes will be loaded!!" << std::endl;
    throw 1;
  } else {
    optimize->SetUseCuttingPlanes(true);
    vnl_vector_fixed<double, 3> a, b, c;
    int ctr = 0;

    for (int shapeCount = 0; shapeCount < numShapes; shapeCount++) {
      for (int planeCount = 0; planeCount < cutting_planes_per_input[shapeCount]; planeCount++) {
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
          ParticleSystem::PointType pa;
          ParticleSystem::PointType pb;
          ParticleSystem::PointType pc;

          pa[0] = a[0];
          pa[1] = a[1];
          pa[2] = a[2];
          pb[0] = b[0];
          pb[1] = b[1];
          pb[2] = b[2];
          pc[0] = c[0];
          pc[1] = c[1];
          pc[2] = c[2];

          ParticleSystem::TransformType T = optimize->GetSampler()->GetParticleSystem()->GetTransform(shapeCount);
          ParticleSystem::TransformType prefT =
              optimize->GetSampler()->GetParticleSystem()->GetPrefixTransform(shapeCount);
          pa = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pa, T * prefT);
          pb = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pb, T * prefT);
          pc = optimize->GetSampler()->GetParticleSystem()->TransformPoint(pc, T * prefT);

          a[0] = pa[0];
          a[1] = pa[1];
          a[2] = pa[2];
          b[0] = pb[0];
          b[1] = pb[1];
          b[2] = pb[2];
          c[0] = pc[0];
          c[1] = pc[1];
          c[2] = pc[2];
        }

        optimize->GetSampler()->SetCuttingPlane(shapeCount, a, b, c);
      }
    }
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_explanatory_variables(TiXmlHandle* doc_handle, Optimize* optimize) {
  TiXmlElement* elem = nullptr;

  std::istringstream inputsBuffer;
  std::vector<double> evars;
  double etmp;

  elem = doc_handle->FirstChild("explanatory_variable").Element();
  if (!elem) {
    return true;
  }
  inputsBuffer.str(elem->GetText());
  while (inputsBuffer >> etmp) {
    evars.push_back(etmp);
  }

  dynamic_cast<LinearRegressionShapeMatrix*>(
      optimize->GetSampler()->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())
      ->SetExplanatory(evars);

  dynamic_cast<MixedEffectsShapeMatrix*>(
      optimize->GetSampler()->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())
      ->SetExplanatory(evars);

  optimize->SetUseRegression(true);
  if (optimize->GetTimePtsPerSubject() > 1) {
    optimize->SetUseMixedEffects(true);
  }

  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_flag_particles(TiXmlHandle* doc_handle, Optimize* optimize) {
  optimize->SetParticleFlags(this->read_int_list(doc_handle, "fixed_landmarks"));
  return true;
}

//---------------------------------------------------------------------------
bool OptimizeParameterFile::read_flag_domains(TiXmlHandle* doc_handle, Optimize* optimize) {
  optimize->SetFixedDomains(this->read_int_list(doc_handle, "fixed_domains"));
  return true;
}

//---------------------------------------------------------------------------
std::vector<int> OptimizeParameterFile::read_int_list(TiXmlHandle* doc_handle, std::string name) {
  std::vector<int> list;
  TiXmlElement* elem = doc_handle->FirstChild(name.c_str()).Element();
  if (elem) {
    std::istringstream inputsBuffer;
    inputsBuffer.str(elem->GetText());
    int temp_int;
    while (inputsBuffer >> temp_int) {
      list.push_back(temp_int);
    }
  }
  return list;
}
//---------------------------------------------------------------------------
}  // namespace shapeworks
