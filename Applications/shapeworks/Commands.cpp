#include "Commands.h"

#include <Analyze/Analyze.h>
#include <Application/DeepSSM/DeepSSMJob.h>
#include <Application/Job/PythonWorker.h>
#include <Groom/Groom.h>
#include <Logging.h>
#include <Image/Image.h>
#include <Mesh/Mesh.h>
#include <Mesh/MeshWarper.h>
#include <Optimize/Optimize.h>
#include <Optimize/OptimizeParameterFile.h>
#include <Optimize/OptimizeParameters.h>
#include <Particles/ParticleFile.h>
#include <Profiling.h>
#include <Project/Project.h>
#include <ShapeworksUtils.h>
#include <Utils/StringUtils.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>
#include <numeric>

#include <QCoreApplication>
#include <boost/filesystem.hpp>

namespace shapeworks {

// boilerplate for a command. Copy this to start a new command
// Next, be sure to add the command to shapeworks.cpp!
#if 0
///////////////////////////////////////////////////////////////////////////////
// Example
///////////////////////////////////////////////////////////////////////////////
void Example::buildParser()
{
  const std::string prog = "example";
  const std::string desc = "brief description of command";
  parser.prog(prog).description(desc);

  parser.add_option("--optionname").action("store").type("double").set_default(0.01).help("Description of optionname.");
  //additional options... 
  
  Command::buildParser();
}

bool Example::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  double optionName = static_cast<double>(options.get("optionname"));
  //read additional options... 

  sharedData.image.example(optionName, ...);
  return true;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Seed
///////////////////////////////////////////////////////////////////////////////
void Seed::buildParser() {
  const std::string prog = "seed";
  const std::string desc = "sets the seed for random number generation (useful for debugging)";
  parser.prog(prog).description(desc);

  parser.add_option("--value")
      .action("store")
      .type("int")
      .set_default(std::chrono::system_clock::now().time_since_epoch().count())
      .help("Value of seed.");

  Command::buildParser();
}

bool Seed::execute(const optparse::Values& options, SharedCommandData& sharedData) {
  int value = static_cast<int>(options.get("value"));

  ShapeWorksUtils::set_rng_seed(value);

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Optimize
///////////////////////////////////////////////////////////////////////////////
void OptimizeCommand::buildParser() {
  const std::string prog = "optimize";
  const std::string desc = "generate a particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Path to project file.");
  parser.add_option("--progress").action("store_true").set_default(false).help("Show progress [default: false].");
  parser.add_option("--xmlconsole")
      .action("store_true")
      .set_default(false)
      .help("XML console output [default: false].");

  Command::buildParser();
}

bool OptimizeCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
  const std::string& projectFile(static_cast<std::string>(options.get("name")));
  bool show_progress = static_cast<bool>(options.get("progress"));
  bool xml_status = static_cast<bool>(options.get("xmlconsole"));

  if (projectFile.length() == 0) {
    std::cerr << "Must specify project name with --name <project.xlsx|.swproj>\n";
    return false;
  }

  bool isProject = StringUtils::hasSuffix(projectFile, "xlsx") || StringUtils::hasSuffix(projectFile, "swproj");

  Optimize app;
  ShapeWorksUtils::setup_console_logging(show_progress, xml_status);

  if (isProject) {
    try {
      // load spreadsheet project
      ProjectHandle project = std::make_shared<Project>();
      try {
        project->load(projectFile);
      } catch (std::exception& e) {
        SW_ERROR("Project failed to load: {}", e.what());
        return false;
      }

      const auto oldBasePath = boost::filesystem::current_path();
      auto base = StringUtils::getPath(projectFile);
      if (base != projectFile) {
        boost::filesystem::current_path(base.c_str());
        project->set_filename(StringUtils::getFilename(projectFile));
      }

      TIME_START("set_up_optimize");
      // set up Optimize class based on project parameters
      OptimizeParameters params(project);
      params.set_up_optimize(&app);
      app.SetProject(project);
      TIME_STOP("set_up_optimize");

      bool success = app.Run();

      boost::filesystem::current_path(oldBasePath);
      if (success) {
        project->save(projectFile);
      }
      if (show_progress) {
        // final newline so that the next line doesn't garble with the progress
        std::cout << "\n";
      }

      return success;
    } catch (std::exception& e) {
      SW_ERROR("{}", e.what());
      return false;
    }
  } else {
    OptimizeParameterFile param;
    param.load_parameter_file(projectFile.c_str(), &app);
    return app.Run();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Groom
///////////////////////////////////////////////////////////////////////////////
void GroomCommand::buildParser() {
  const std::string prog = "groom";
  const std::string desc = "groom a shapeworks project";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Path to project file.");
  parser.add_option("--progress").action("store_true").set_default(false).help("Show progress [default: false].");
  parser.add_option("--xmlconsole")
      .action("store_true")
      .set_default(false)
      .help("XML console output [default: false].");

  Command::buildParser();
}

bool GroomCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
  const std::string& projectFile(static_cast<std::string>(options.get("name")));
  bool show_progress = static_cast<bool>(options.get("progress"));
  bool xml_status = static_cast<bool>(options.get("xmlconsole"));

  if (projectFile.length() == 0) {
    std::cerr << "Must specify project name with --name <project.xlsx|.swproj>\n";
    return false;
  }

  ShapeWorksUtils::setup_console_logging(show_progress, xml_status);

  try {
    ProjectHandle project = std::make_shared<Project>();
    project->load(projectFile);

    const auto oldBasePath = boost::filesystem::current_path();
    auto base = StringUtils::getPath(projectFile);
    if (base != projectFile) {
      boost::filesystem::current_path(base.c_str());
      project->set_filename(StringUtils::getFilename(projectFile));
    }

    Groom app(project);
    bool success = app.run();

    boost::filesystem::current_path(oldBasePath);
    if (success) {
      project->save(projectFile);
    }
    if (show_progress) {
      // final newline so that the next line doesn't garble with the progress
      std::cout << "\n";
    }

    return success;
  } catch (std::exception& e) {
    SW_ERROR("{}", e.what());
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Analyze
///////////////////////////////////////////////////////////////////////////////
void AnalyzeCommand::buildParser() {
  const std::string prog = "analyze";
  const std::string desc = "Offline analysis of a shapeworks project, output json and meshes";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Path to project file.");
  parser.add_option("--output").action("store").type("string").set_default("").help("Path to output file.");
  parser.add_option("--range").action("store").type("float").set_default(3.0f).help(
      "Standard deviation range for PCA [default: 3.0].");
  parser.add_option("--steps").action("store").type("int").set_default(21).help(
      "Number of steps to use for PCA [default: 21].");

  Command::buildParser();
}

bool AnalyzeCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
  const std::string& projectFile(static_cast<std::string>(options.get("name")));
  const std::string& outputFile(static_cast<std::string>(options.get("output")));
  const float range = static_cast<float>(options.get("range"));
  const int steps = static_cast<int>(options.get("steps"));

  if (projectFile.length() == 0) {
    std::cerr << "Must specify project name with --name <project.xlsx|.swproj>\n";
    return false;
  }

  if (outputFile.empty()) {
    std::cerr << "No output file specified, must pass `--output <filename>`\n";
    return false;
  }

  try {
    ProjectHandle project = std::make_shared<Project>();

    const auto oldBasePath = boost::filesystem::current_path();
    auto base = StringUtils::getPath(projectFile);
    auto filename = StringUtils::getFilename(projectFile);
    if (base != projectFile) {
      SW_LOG("Setting path to {}", base);
      boost::filesystem::current_path(base.c_str());
      project->set_filename(filename);
    }

    SW_LOG("Loading project: {}", filename);
    project->load(filename);

    Analyze analyze(project);

    boost::filesystem::path dir(oldBasePath);
    boost::filesystem::path file(outputFile);
    boost::filesystem::path full_output = dir / file;
    analyze.run_offline_analysis(full_output.string(), range, steps);

    boost::filesystem::current_path(oldBasePath);

    return true;
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// CorrespondenceQuality
///////////////////////////////////////////////////////////////////////////////
namespace {

Mesh load_groomed_as_mesh(const std::string& path) {
  const std::string ext = StringUtils::getLowerExtension(path);
  if (ext == ".nrrd" || ext == ".mha" || ext == ".mhd" || ext == ".nii" || ext == ".gz" || ext == ".tif" ||
      ext == ".tiff") {
    Image img(path);
    return img.toMesh(0.0);
  }
  return Mesh(path);
}

Eigen::MatrixXd load_particles_matrix(const std::string& filename) {
  Eigen::VectorXd v = particles::read_particles(filename);
  const int n = static_cast<int>(v.size() / 3);
  // read_particles returns interleaved [x0,y0,z0, x1,y1,z1, ...]
  Eigen::MatrixXd m(n, 3);
  for (int i = 0; i < n; ++i) {
    m(i, 0) = v(3 * i + 0);
    m(i, 1) = v(3 * i + 1);
    m(i, 2) = v(3 * i + 2);
  }
  return m;
}

struct DistanceStats {
  double mean = 0.0;
  double max = 0.0;
  double median = 0.0;
  double p95 = 0.0;
};

DistanceStats summarize(std::vector<double> values) {
  DistanceStats s;
  if (values.empty()) return s;
  std::sort(values.begin(), values.end());
  s.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
  s.max = values.back();
  s.median = values[values.size() / 2];
  const size_t p95_idx = std::min(values.size() - 1, static_cast<size_t>(0.95 * values.size()));
  s.p95 = values[p95_idx];
  return s;
}

struct SubjectResult {
  std::string subject;
  int domain;
  double mean_dist;
  double max_dist;
  double bbox_diag;
  double norm_mean;  // mean_dist / bbox_diag
  double norm_max;   // max_dist / bbox_diag
  bool is_template;
};

}  // namespace

void CorrespondenceQualityCommand::buildParser() {
  const std::string prog = "correspondence-quality";
  const std::string desc =
      "Evaluate per-subject correspondence quality by reconstructing each shape from its local particles "
      "(biharmonic mesh warp from the cohort L1-medoid template, matching Studio's median selection) and "
      "measuring distance to the groomed mesh. Reports per-subject and aggregate statistics.";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help(
      "Path to project file (.swproj or .xlsx).");
  parser.add_option("--output").action("store").type("string").set_default("").help(
      "Optional path to write per-subject CSV.");
  parser.add_option("--output_meshes").action("store").type("string").set_default("").help(
      "Optional directory; write reconstructed meshes with per-vertex distance field for visual inspection.");
  std::list<std::string> methods{"point-to-cell", "point-to-point"};
  parser.add_option("--method")
      .action("store")
      .type("choice")
      .choices(methods.begin(), methods.end())
      .set_default("point-to-cell")
      .help("Distance method [default: %default].");
  parser.add_option("--worst")
      .action("store")
      .type("int")
      .set_default(5)
      .help("Number of worst-ranked subjects to print [default: %default].");

  Command::buildParser();
}

bool CorrespondenceQualityCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
  const std::string& projectFile(static_cast<std::string>(options.get("name")));
  const std::string& outputFile(static_cast<std::string>(options.get("output")));
  const std::string& outputMeshesDir(static_cast<std::string>(options.get("output_meshes")));
  const std::string methodopt(options.get("method"));
  const int worst_n = static_cast<int>(options.get("worst"));

  if (projectFile.empty()) {
    std::cerr << "Must specify project name with --name <project.xlsx|.swproj>\n";
    return false;
  }

  const Mesh::DistanceMethod distance_method =
      (methodopt == "point-to-point") ? Mesh::DistanceMethod::PointToPoint : Mesh::DistanceMethod::PointToCell;

  try {
    ProjectHandle project = std::make_shared<Project>();

    const auto oldBasePath = boost::filesystem::current_path();
    auto base = StringUtils::getPath(projectFile);
    auto filename = StringUtils::getFilename(projectFile);
    if (base != projectFile) {
      SW_LOG("Setting path to {}", base);
      boost::filesystem::current_path(base.c_str());
      project->set_filename(filename);
    }

    SW_LOG("Loading project: {}", filename);
    project->load(filename);

    auto subjects = project->get_non_excluded_subjects();
    if (subjects.empty()) {
      SW_ERROR("No (non-excluded) subjects in project");
      boost::filesystem::current_path(oldBasePath);
      return false;
    }
    const int num_domains = project->get_number_of_domains_per_subject();
    if (num_domains <= 0) {
      SW_ERROR("Project has no domains");
      boost::filesystem::current_path(oldBasePath);
      return false;
    }

    // Pass 1: load every (subject, domain) particles + groomed paths. Keep only subjects
    // that have complete data across all domains, so the L1-medoid is computed over a
    // consistent cohort and the same global template is used in every domain.
    std::vector<std::string> name_per_subject;
    std::vector<std::vector<Eigen::MatrixXd>> particles_per_subject_domain;   // [subject][domain]
    std::vector<std::vector<std::string>> groomed_per_subject_domain;          // [subject][domain]

    for (auto& subj : subjects) {
      auto particle_files = subj->get_local_particle_filenames();
      auto groomed_files = subj->get_groomed_filenames();

      bool complete = true;
      std::vector<Eigen::MatrixXd> subj_particles;
      std::vector<std::string> subj_groomed;
      subj_particles.reserve(num_domains);
      subj_groomed.reserve(num_domains);

      for (int d = 0; d < num_domains; ++d) {
        if (d >= static_cast<int>(particle_files.size()) || particle_files[d].empty() ||
            d >= static_cast<int>(groomed_files.size()) || groomed_files[d].empty()) {
          SW_LOG("Skipping subject '{}': missing particles or groomed for domain {}", subj->get_display_name(), d);
          complete = false;
          break;
        }
        try {
          subj_particles.push_back(load_particles_matrix(particle_files[d]));
        } catch (const std::exception& e) {
          SW_LOG("Skipping subject '{}' domain {}: particle load failed: {}", subj->get_display_name(), d, e.what());
          complete = false;
          break;
        }
        subj_groomed.push_back(groomed_files[d]);
      }

      if (!complete) continue;
      name_per_subject.push_back(subj->get_display_name());
      particles_per_subject_domain.push_back(std::move(subj_particles));
      groomed_per_subject_domain.push_back(std::move(subj_groomed));
    }

    const int num_subjects = static_cast<int>(name_per_subject.size());
    if (num_subjects < 2) {
      SW_ERROR("Need at least 2 subjects with complete data across all domains (have {})", num_subjects);
      boost::filesystem::current_path(oldBasePath);
      return false;
    }

    // Verify particle counts match across subjects per domain
    std::vector<int> num_particles_per_domain(num_domains);
    for (int d = 0; d < num_domains; ++d) {
      num_particles_per_domain[d] = static_cast<int>(particles_per_subject_domain[0][d].rows());
      for (int s = 1; s < num_subjects; ++s) {
        if (particles_per_subject_domain[s][d].rows() != num_particles_per_domain[d]) {
          SW_ERROR("Domain {}: subject '{}' has {} particles, expected {}", d, name_per_subject[s],
                   particles_per_subject_domain[s][d].rows(), num_particles_per_domain[d]);
          boost::filesystem::current_path(oldBasePath);
          return false;
        }
      }
    }

    // L1-medoid template selection over concatenated per-domain local particles (matches
    // Studio's compute_median_shape: argmin_i sum_j ||x_i - x_j||_1).
    int template_idx = 0;
    double best_l1_sum = std::numeric_limits<double>::infinity();
    for (int i = 0; i < num_subjects; ++i) {
      double sum_l1 = 0.0;
      for (int j = 0; j < num_subjects; ++j) {
        if (i == j) continue;
        double pair_l1 = 0.0;
        for (int d = 0; d < num_domains; ++d) {
          pair_l1 += (particles_per_subject_domain[i][d] - particles_per_subject_domain[j][d]).cwiseAbs().sum();
        }
        sum_l1 += pair_l1;
      }
      if (sum_l1 < best_l1_sum) {
        best_l1_sum = sum_l1;
        template_idx = i;
      }
    }
    SW_LOG("Template subject (L1-medoid): '{}' (sum of L1 distances to others = {:.4f})",
           name_per_subject[template_idx], best_l1_sum);

    // Resolve output meshes dir (create if missing)
    boost::filesystem::path meshes_dir;
    if (!outputMeshesDir.empty()) {
      meshes_dir = boost::filesystem::path(outputMeshesDir);
      if (!meshes_dir.is_absolute()) {
        meshes_dir = boost::filesystem::path(oldBasePath) / meshes_dir;
      }
      boost::filesystem::create_directories(meshes_dir);
      SW_LOG("Writing reconstructed meshes to: {}", meshes_dir.string());
    }

    std::vector<SubjectResult> all_results;
    std::vector<double> all_means;       // pooled raw mean distances (excluding template)
    std::vector<double> all_norm_means;  // pooled bbox-normalized mean distances (excluding template)

    // Pass 2: per-domain warp + distance using the single global template.
    for (int domain = 0; domain < num_domains; ++domain) {
      SW_LOG("=== Domain {} ===", domain);

      Mesh template_mesh = load_groomed_as_mesh(groomed_per_subject_domain[template_idx][domain]);
      MeshWarper warper;
      warper.set_warp_method(WarpMethod::Biharmonic);
      warper.set_reference_mesh(template_mesh.getVTKMesh(), particles_per_subject_domain[template_idx][domain]);
      if (!warper.generate_warp()) {
        SW_ERROR("Domain {}: failed to generate warp from template '{}'", domain, name_per_subject[template_idx]);
        boost::filesystem::current_path(oldBasePath);
        return false;
      }

      for (int i = 0; i < num_subjects; ++i) {
        vtkSmartPointer<vtkPolyData> reconstructed = warper.build_mesh(particles_per_subject_domain[i][domain]);
        if (!reconstructed) {
          SW_LOG("Domain {}: subject '{}' reconstruction returned null, skipping", domain, name_per_subject[i]);
          continue;
        }

        Mesh recon_mesh(reconstructed);
        Mesh groomed_mesh = load_groomed_as_mesh(groomed_per_subject_domain[i][domain]);
        auto field = recon_mesh.distance(groomed_mesh, distance_method)[0];

        const int n = field->GetNumberOfTuples();
        if (n == 0) continue;
        double sum = 0.0;
        double maxv = 0.0;
        for (int k = 0; k < n; ++k) {
          const double v = std::fabs(field->GetTuple1(k));
          sum += v;
          if (v > maxv) maxv = v;
        }
        const double mean_d = sum / n;

        const auto bbox = groomed_mesh.boundingBox();
        const double bbox_diag = (bbox.max - bbox.min).GetNorm();
        const double norm_mean = (bbox_diag > 0.0) ? mean_d / bbox_diag : 0.0;
        const double norm_max = (bbox_diag > 0.0) ? maxv / bbox_diag : 0.0;

        const bool is_template = (i == template_idx);
        all_results.push_back({name_per_subject[i], domain, mean_d, maxv, bbox_diag, norm_mean, norm_max, is_template});
        if (!is_template) {
          all_means.push_back(mean_d);
          all_norm_means.push_back(norm_mean);
        }

        // Optional: write reconstructed mesh with distance field
        if (!meshes_dir.empty()) {
          field->SetName("distance");
          recon_mesh.setField("distance", field, Mesh::FieldType::Point);
          std::string fname = name_per_subject[i] + "_domain" + std::to_string(domain) + "_reconstructed.vtk";
          if (is_template) fname = name_per_subject[i] + "_domain" + std::to_string(domain) + "_reconstructed_TEMPLATE.vtk";
          boost::filesystem::path out_mesh = meshes_dir / fname;
          recon_mesh.write(out_mesh.string());
        }
      }
    }

    if (all_results.empty()) {
      SW_ERROR("No subjects evaluated");
      boost::filesystem::current_path(oldBasePath);
      return false;
    }

    // Aggregate (excluding template — its reconstruction is near-identity and would skew
    // small cohorts; e.g. with N=4 it dominates 25% of the average).
    DistanceStats agg = summarize(all_means);
    DistanceStats agg_norm = summarize(all_norm_means);
    const size_t num_template_rows = all_results.size() - all_means.size();

    std::cout << "\n=== Correspondence Quality Summary ===\n";
    std::cout << "Subjects evaluated: " << all_results.size() << " (" << all_means.size() << " in aggregate, "
              << num_template_rows << " template row(s) excluded)\n";
    std::cout << "Template subject: " << name_per_subject[template_idx] << "\n";
    std::cout << "Distance method: " << methodopt << "\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Per-subject mean distance (reconstructed -> groomed):\n";
    std::cout << "  mean   = " << agg.mean << "\n";
    std::cout << "  median = " << agg.median << "\n";
    std::cout << "  p95    = " << agg.p95 << "\n";
    std::cout << "  max    = " << agg.max << "\n";
    std::cout << "Normalized by per-subject groomed-mesh bbox diagonal (fraction):\n";
    std::cout << "  mean   = " << agg_norm.mean << "  (" << std::setprecision(3) << (agg_norm.mean * 100.0) << "%)\n";
    std::cout << std::setprecision(6);
    std::cout << "  median = " << agg_norm.median << "\n";
    std::cout << "  p95    = " << agg_norm.p95 << "\n";
    std::cout << "  max    = " << agg_norm.max << "\n";

    // Worst-N (sorted by normalized mean for scale-invariant ranking; template excluded)
    if (worst_n > 0) {
      std::vector<SubjectResult> sorted_results;
      sorted_results.reserve(all_results.size());
      for (const auto& r : all_results) {
        if (!r.is_template) sorted_results.push_back(r);
      }
      std::sort(sorted_results.begin(), sorted_results.end(),
                [](const SubjectResult& a, const SubjectResult& b) { return a.norm_mean > b.norm_mean; });
      const int limit = std::min(worst_n, static_cast<int>(sorted_results.size()));
      std::cout << "\nWorst " << limit << " subjects (ranked by normalized mean; template excluded):\n";
      for (int i = 0; i < limit; ++i) {
        const auto& r = sorted_results[i];
        std::cout << "  " << r.subject << "  (domain " << r.domain << ")"
                  << "  norm_mean=" << r.norm_mean << "  (" << std::setprecision(3) << (r.norm_mean * 100.0) << "%)"
                  << std::setprecision(6) << "  mean=" << r.mean_dist << "  max=" << r.max_dist
                  << "  bbox_diag=" << r.bbox_diag << "\n";
      }
    }

    // CSV
    if (!outputFile.empty()) {
      boost::filesystem::path out_path(outputFile);
      if (!out_path.is_absolute()) {
        out_path = boost::filesystem::path(oldBasePath) / out_path;
      }
      std::ofstream csv(out_path.string());
      if (!csv) {
        SW_ERROR("Failed to open output file: {}", out_path.string());
        boost::filesystem::current_path(oldBasePath);
        return false;
      }
      csv << "subject,domain,is_template,mean_dist,max_dist,bbox_diag,norm_mean,norm_max\n";
      csv << std::fixed << std::setprecision(8);
      for (const auto& r : all_results) {
        csv << r.subject << "," << r.domain << "," << (r.is_template ? 1 : 0) << "," << r.mean_dist << ","
            << r.max_dist << "," << r.bbox_diag << "," << r.norm_mean << "," << r.norm_max << "\n";
      }
      SW_LOG("Wrote per-subject CSV: {}", out_path.string());
    }

    boost::filesystem::current_path(oldBasePath);
    return true;
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Convert Project
///////////////////////////////////////////////////////////////////////////////
void ConvertProjectCommand::buildParser() {
  const std::string prog = "convert-project";
  const std::string desc = "convert a shapeworks project (xlsx or swproj)";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help(
      "Path to input project file (xlsx or swproj).");
  parser.add_option("--output")
      .action("store")
      .type("string")
      .set_default("")
      .help("Path to output project file (xlsx or swproj).");

  Command::buildParser();
}

bool ConvertProjectCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
  const std::string& projectFile(static_cast<std::string>(options.get("name")));
  const std::string& outputFile(static_cast<std::string>(options.get("output")));

  if (projectFile.length() == 0) {
    std::cerr << "No input file specified, must pass `--name <filename>`\n";
    return false;
  }

  auto extension = StringUtils::getLowerExtension(projectFile);
  if (extension != ".xlsx" && extension != ".swproj") {
    std::cerr << "Input project file must be either .xlsx or .swproj\n";
    return false;
  }

  if (outputFile.empty()) {
    std::cerr << "No output file specified, must pass `--output <filename>`\n";
    return false;
  }

  extension = StringUtils::getLowerExtension(outputFile);
  if (extension != ".xlsx" && extension != ".swproj") {
    std::cerr << "Output project file must be either .xlsx or .swproj\n";
    return false;
  }

  try {
    ProjectHandle project = std::make_shared<Project>();
    project->load(projectFile);
    project->save(outputFile);
    return true;
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// DeepSSM
///////////////////////////////////////////////////////////////////////////////
void DeepSSMCommand::buildParser() {
  const std::string prog = "deepssm";
  const std::string desc = "run deepssm steps";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help(
      "Path to input project file (xlsx or swproj).");

  // Create a vector of choices first
  std::vector<std::string> prep_choices = {"all", "groom_training", "optimize_training", "optimize_validation",
                                           "groom_images"};

  // --prep option with choices
  parser.add_option("--prep")
      .action("store")
      .type("choice")
      .choices(prep_choices.begin(), prep_choices.end())
      //.set_default("all")
      .help("Preparation step to run");

  // Boolean flag options
  parser.add_option("--augment").action("store_true").help("Run data augmentation");

  parser.add_option("--train").action("store_true").help("Run training");

  parser.add_option("--test").action("store_true").help("Run testing");

  parser.add_option("--all").action("store_true").help("Run all steps");

  // add num_workers option
  parser.add_option("--num_workers")
      .action("store")
      .type("int")
      .set_default(0)
      .help("Number of data loader workers (default: 0)");

  parser.add_option("--aug_processes")
      .action("store")
      .type("int")
      .set_default(0)
      .help("Number of augmentation processes (default: 0 = use all cores)");

  Command::buildParser();
}

bool DeepSSMCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
  // QCoreApplication provides the event loop needed for PythonWorker's QThread,
  // without requiring Qt platform plugins (which may not be available on headless CI).
  int argc = 1;
  char* argv[1];
  argv[0] = const_cast<char*>("shapeworks");

  QCoreApplication app(argc, argv);

  // Handle project file: either from --name or first positional argument
  std::string project_file;
  if (options.is_set_by_user("name")) {
    // User explicitly provided --name
    project_file = options["name"];
  } else if (!parser.args().empty()) {
    // Use first positional argument
    project_file = parser.args()[0];
  } else {
    // No project file provided at all
    parser.error("Project file must be provided either as --name or as a positional argument");
  }

  // Handle prep option with manual default
  std::string prep_step;
  if (options.is_set_by_user("prep")) {
    prep_step = options["prep"];
  } else {
    prep_step = "all";  // Manual default
  }

  std::cout << "DeepSSM: Using project file: " << project_file << std::endl;

  bool do_prep = options.is_set("prep") || options.is_set("all");
  bool do_augment = options.is_set("augment") || options.is_set("all");
  bool do_train = options.is_set("train") || options.is_set("all");
  bool do_test = options.is_set("test") || options.is_set("all");

  int num_workers = static_cast<int>(options.get("num_workers"));
  int aug_processes = static_cast<int>(options.get("aug_processes"));

  std::cout << "Prep step:    " << (do_prep ? "on" : "off") << "\n";
  std::cout << "Augment step: " << (do_augment ? "on" : "off") << "\n";
  std::cout << "Train step:   " << (do_train ? "on" : "off") << "\n";
  std::cout << "Test step:    " << (do_test ? "on" : "off") << "\n";
  std::cout << "Num dataloader workers:  " << num_workers << "\n";
  std::cout << "Augmentation processes:  " << (aug_processes == 0 ? QThread::idealThreadCount() : aug_processes) << "\n";

  if (!do_prep && !do_augment && !do_train && !do_test) {
    do_prep = true;
    do_augment = true;
    do_train = true;
    do_test = true;
  }

  ProjectHandle project = std::make_shared<Project>();
  project->load(project_file);

  // Check if PyTorch is available; install if needed
  if (!PythonWorker::is_torch_available()) {
    std::cout << "PyTorch is not installed. Installing via light-the-torch...\n";
    bool success = PythonWorker::install_torch([](std::string msg) {
      std::cout << msg << "\n";
    });
    if (!success) {
      std::cerr << "Error: Failed to install PyTorch. DeepSSM requires PyTorch.\n";
      return false;
    }
  }

  PythonWorker python_worker;
  python_worker.set_cli_mode(true);

  auto wait_for_job = [&](auto job) {
    // This lambda will block until the job is complete
    while (!job->is_complete()) {
      QCoreApplication::processEvents();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if (job->is_aborted()) {
        return false;
      }
    }
    return true;
  };

  if (do_prep) {
    auto job = QSharedPointer<DeepSSMJob>::create(project, DeepSSMJob::JobType::DeepSSM_PrepType);
    job->set_num_dataloader_workers(num_workers);
    if (prep_step == "all") {
      job->set_prep_step(DeepSSMJob::PrepStep::NOT_STARTED);
    } else if (prep_step == "groom_training") {
      job->set_prep_step(DeepSSMJob::PrepStep::GROOM_TRAINING);
    } else if (prep_step == "optimize_training") {
      job->set_prep_step(DeepSSMJob::PrepStep::OPTIMIZE_TRAINING);
    } else if (prep_step == "optimize_validation") {
      job->set_prep_step(DeepSSMJob::PrepStep::OPTIMIZE_VALIDATION);
    } else if (prep_step == "groom_images") {
      job->set_prep_step(DeepSSMJob::PrepStep::GROOM_IMAGES);
    } else {
      SW_ERROR("Unknown prep step: {}", prep_step);
      return false;
    }
    std::cout << "Running DeepSSM preparation step...\n";
    python_worker.run_job(job);
    if (!wait_for_job(job)) {
      return false;
    }
    std::cout << "DeepSSM preparation step completed.\n";
  }
  if (do_augment) {
    std::cout << "Running DeepSSM data augmentation...\n";
    auto job = QSharedPointer<DeepSSMJob>::create(project, DeepSSMJob::JobType::DeepSSM_AugmentationType);
    job->set_aug_processes(aug_processes);
    python_worker.run_job(job);
    if (!wait_for_job(job)) {
      return false;
    }
    std::cout << "DeepSSM data augmentation completed.\n";
  }
  if (do_train) {
    std::cout << "Running DeepSSM training...\n";
    auto job = QSharedPointer<DeepSSMJob>::create(project, DeepSSMJob::JobType::DeepSSM_TrainingType);
    python_worker.run_job(job);
    if (!wait_for_job(job)) {
      return false;
    }
    std::cout << "DeepSSM training completed.\n";
  }
  if (do_test) {
    std::cout << "Running DeepSSM testing...\n";
    auto job = QSharedPointer<DeepSSMJob>::create(project, DeepSSMJob::JobType::DeepSSM_TestingType);
    python_worker.run_job(job);
    if (!wait_for_job(job)) {
      return false;
    }
    std::cout << "DeepSSM testing completed.\n";
  }

  project->save();

  return true;
}

}  // namespace shapeworks
