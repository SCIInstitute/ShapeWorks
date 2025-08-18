#include "Commands.h"

#include <Analyze/Analyze.h>
#include <Application/DeepSSM/DeepSSMJob.h>
#include <Application/Job/PythonWorker.h>
#include <Groom/Groom.h>
#include <Logging.h>
#include <Optimize/Optimize.h>
#include <Optimize/OptimizeParameterFile.h>
#include <Optimize/OptimizeParameters.h>
#include <Profiling.h>
#include <ShapeworksUtils.h>
#include <Utils/StringUtils.h>

#include <QApplication>
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

  Command::buildParser();
}

bool DeepSSMCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
  // Create a non-gui QApplication instance
  int argc = 0;
  char** argv = nullptr;
  QApplication app(argc, argv);

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

  std::cout << "Prep step:    " << (do_prep ? "on" : "off") << "\n";
  std::cout << "Augment step: " << (do_augment ? "on" : "off") << "\n";
  std::cout << "Train step:   " << (do_train ? "on" : "off") << "\n";
  std::cout << "Test step:    " << (do_test ? "on" : "off") << "\n";

  if (!do_prep && !do_augment && !do_train && !do_test) {
    do_prep = true;
    do_augment = true;
    do_train = true;
    do_test = true;
  }

  ProjectHandle project = std::make_shared<Project>();
  project->load(project_file);

  PythonWorker python_worker;

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

  return false;
}

}  // namespace shapeworks
