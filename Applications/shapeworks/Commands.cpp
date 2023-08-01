#include "Commands.h"

#include <Analyze/Analyze.h>
#include <Groom/Groom.h>
#include <Logging.h>
#include <Optimize/Optimize.h>
#include <Optimize/OptimizeParameterFile.h>
#include <Optimize/OptimizeParameters.h>
#include <ShapeworksUtils.h>
#include <Utils/StringUtils.h>

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

static void setup_callbacks(bool show_progress, bool xml_status) {
  if (show_progress) {
    auto progress_callback = [](double progress, std::string message) {
      // show status message and percentage complete
      std::cout << fmt::format("{} ({:.1f}%)        \r", message, progress);
      std::cout.flush();
    };
    Logging::Instance().set_progress_callback(progress_callback);
  }

  if (xml_status) {
    auto progress_callback = [](double progress, std::string message) {
      // print status message and percentage complete
      std::cout << fmt::format("<xml><status>{}</status><progress>{:.1f}</progress></xml>\n", message, progress);
      std::cout.flush();
    };
    Logging::Instance().set_progress_callback(progress_callback);

    auto error_callback = [](std::string message) {
      std::cout << fmt::format("<xml><error>{}</error></xml>\n", message);
      std::cout.flush();
    };
    Logging::Instance().set_error_callback(error_callback);
  }
}

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

  ShapeworksUtils::setRngSeed(value);

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
  parser.add_option("--xmlconsole").action("store_true").set_default(false).help("XML console output [default: false].");

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
  setup_callbacks(show_progress, xml_status);

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

      // set up Optimize class based on project parameters
      OptimizeParameters params(project);
      params.set_up_optimize(&app);
      app.SetProject(project);

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
      SW_ERROR(e.what());
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
  parser.add_option("--xmlconsole").action("store_true").set_default(false).help("XML console output [default: false].");

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

  setup_callbacks(show_progress, xml_status);

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
    SW_ERROR(e.what());
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
  parser.add_option("--range").action("store").type("float").set_default(3.0f).help("Standard deviation range for PCA [default: 3.0].");
  parser.add_option("--steps").action("store").type("int").set_default(21).help("Number of steps to use for PCA [default: 21].");

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
}  // namespace shapeworks
