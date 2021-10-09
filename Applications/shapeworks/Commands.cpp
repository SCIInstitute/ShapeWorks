#include "Commands.h"
#include <Libs/Optimize/Optimize.h>
#include <Libs/Optimize/OptimizeParameters.h>
#include <Libs/Optimize/OptimizeParameterFile.h>
#include <Libs/Groom/Groom.h>
#include <Libs/Utils/StringUtils.h>
#include <ShapeworksUtils.h>
#include <boost/filesystem.hpp>
#include <limits>

#ifdef _WIN32
#include <direct.h>
  #define chdir _chdir
#else
  #include <unistd.h>
#endif

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
void Seed::buildParser()
{
  const std::string prog = "seed";
  const std::string desc = "sets the seed for random number generation (useful for debugging)";
  parser.prog(prog).description(desc);

  parser.add_option("--value").action("store").type("int").set_default(std::chrono::system_clock::now().time_since_epoch().count()).help("Value of seed.");

  Command::buildParser();
}

bool Seed::execute(const optparse::Values& options, SharedCommandData& sharedData)
{
  int value = static_cast<int>(options.get("value"));

  ShapeworksUtils::setRngSeed(value);

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Optimize
///////////////////////////////////////////////////////////////////////////////
void OptimizeCommand::buildParser()
{
  const std::string prog = "optimize";
  const std::string desc = "generate a particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Path to parameter file.");

  Command::buildParser();
}

bool OptimizeCommand::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  const std::string& projectFile(static_cast<std::string>(options.get("name")));

  if (projectFile.length() == 0)
  {
    std::cerr << "Must specify project name\n";
    return false;
  }

  bool isProject = StringUtils::hasSuffix(projectFile, "xlsx");

  Optimize app;
  if (isProject) {
    try {
      // load spreadsheet project
      ProjectHandle project = std::make_shared<Project>();
      project->load(projectFile);

      const auto oldBasePath = boost::filesystem::current_path();
      auto base = StringUtils::getPath(projectFile);
      if (base != projectFile) {
        chdir(base.c_str());
        project->set_filename(StringUtils::getFilename(projectFile));
      }

      // set up Optimize class based on project parameters
      OptimizeParameters params(project);
      params.set_up_optimize(&app);

      bool success = app.Run();

      chdir(reinterpret_cast<const char*>(oldBasePath.c_str()));
      if (success) {
        project->save(projectFile);
      }

      return success;
    }
    catch (std::exception& e) {
      std::cerr << "Error: " << e.what() << "\n";
      return false;
    }
  }
  else {
    OptimizeParameterFile param;
    param.load_parameter_file(projectFile.c_str(), &app);
    return app.Run();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Groom
///////////////////////////////////////////////////////////////////////////////
void GroomCommand::buildParser()
{
  const std::string prog = "groom";
  const std::string desc = "groom a shapeworks project";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Path to parameter file.");

  Command::buildParser();
}

bool GroomCommand::execute(const optparse::Values& options, SharedCommandData& sharedData)
{
  const std::string& projectFile(static_cast<std::string>(options.get("name")));

  if (projectFile.length() == 0) {
    std::cerr << "Must specify project name\n";
    return false;
  }

  try {
    ProjectHandle project = std::make_shared<Project>();
    project->load(projectFile);

    auto base = StringUtils::getPath(projectFile);
    if (base != projectFile) {
      chdir(base.c_str());
      project->set_filename(StringUtils::getFilename(projectFile));
    }

    Groom app(project);
    bool success = app.run();
    if (success) {
      project->save(projectFile);
    }
    return success;
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return false;
  }
}

} // shapeworks
