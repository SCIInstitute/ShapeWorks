#include "Commands.h"

// #include <Analyze/Analyze.h>
// #include <Groom/Groom.h>
#include <Logging.h>
#include <Optimize/Optimize.h>
#include <Optimize/OptimizeParameterFile.h>
#include <Optimize/OptimizeParameters.h>
// #include <ShapeworksUtils.h>
#include <Utils/StringUtils.h>
// #include <torch/script.h>


#include <boost/filesystem.hpp>

namespace shapeworks {

// boilerplate for a command. Copy this to start a new command
// // Next, be sure to add the command to shapeworks.cpp!
// #if 0
// ///////////////////////////////////////////////////////////////////////////////
// // Example
// ///////////////////////////////////////////////////////////////////////////////
// void Example::buildParser()
// {
//   const std::string prog = "example";
//   const std::string desc = "brief description of command";
//   parser.prog(prog).description(desc);

//   parser.add_option("--optionname").action("store").type("double").set_default(0.01).help("Description of optionname.");
//   //additional options... 
  
//   Command::buildParser();
// }

// bool Example::execute(const optparse::Values &options, SharedCommandData &sharedData)
// {
//   double optionName = static_cast<double>(options.get("optionname"));
//   //read additional options... 

//   sharedData.image.example(optionName, ...);
//   return true;
// }
// #endif

// ///////////////////////////////////////////////////////////////////////////////
// // Seed
// ///////////////////////////////////////////////////////////////////////////////
// void Seed::buildParser() {
//   const std::string prog = "seed";
//   const std::string desc = "sets the seed for random number generation (useful for debugging)";
//   parser.prog(prog).description(desc);

//   parser.add_option("--value")
//       .action("store")
//       .type("int")
//       .set_default(std::chrono::system_clock::now().time_since_epoch().count())
//       .help("Value of seed.");

//   Command::buildParser();
// }

// bool Seed::execute(const optparse::Values& options, SharedCommandData& sharedData) {
//   int value = static_cast<int>(options.get("value"));

//   ShapeworksUtils::setRngSeed(value);

//   return true;
// }

///////////////////////////////////////////////////////////////////////////////
// Optimize
///////////////////////////////////////////////////////////////////////////////
void OptimizeCommand::buildParser() {
  const std::string prog = "optimize";
  const std::string desc = "generate a particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Path to project file.");

  Command::buildParser();
}

bool OptimizeCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {

  // std::cout << "IN COMMANDS EXECUTE--TRYING AGAIN" << std::endl;
  // //   auto model = torch::nn::Linear(4096, 4096);
  // //   std::cout << "Train Done A "<< std::endl;

  // //   auto optimizer = torch::optim::Adam(model->parameters(), 0.01);

  // //   auto loss = model->forward(torch::zeros({1, 4096})).sum();
  // //   std::cout << "Train Done C "<< std::endl;

  // //   loss.backward();
  // //   optimizer.step();
  // //   optimizer.zero_grad();
  // //   std::cout << "Train Done " << std::endl;

  // //   torch::save(optimizer, "outputs/optimizer.pt");
  // //   while (true) {
  // //       torch::load(optimizer, "outputs/optimizer.pt");
  // //   }
  // // std::cout << "Train Done E " << std::endl;
  
  // // const char* file_name = "/home/sci/nawazish.khan/non-linear-ssm-experiments/Supershapes/pytorch_models_NEW_MODELS/serialized_model.pt";
  // torch::Device m_device = torch::Device(torch::kCUDA, 0);
  // // torch::jit::script::Module module;
  // std::cout << " Initializing ABC  1 " << std::endl;
  // torch::jit::script::Module *module = new torch::jit::script::Module(torch::jit::load("pytorch_models_NEW_MODELS/serialized_model.pt", m_device, false));
  // std::cout << " Initializing 2 " << std::endl;
  // // module.to(m_device);
  // std::cout << " Initializing 3 " << std::endl;
  const std::string& projectFile(static_cast<std::string>(options.get("name")));

  if (projectFile.length() == 0) {
    std::cerr << "Must specify project name with --name <project.xlsx|.swproj>\n";
    return false;
  }

  bool isProject = StringUtils::hasSuffix(projectFile, "xlsx") || StringUtils::hasSuffix(projectFile, "swproj");

  Optimize app;
  if (isProject) {
    try {
      // load spreadsheet project
      ProjectHandle project = std::make_shared<Project>();
      project->load(projectFile);

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

      return success;
    } catch (std::exception& e) {
      std::cerr << "Error: " << e.what() << "\n";
      return false;
    }
  } else {
    OptimizeParameterFile param;
    param.load_parameter_file(projectFile.c_str(), &app);
    return app.Run();
  }
}

// ///////////////////////////////////////////////////////////////////////////////
// // Groom
// ///////////////////////////////////////////////////////////////////////////////
// void GroomCommand::buildParser() {
//   const std::string prog = "groom";
//   const std::string desc = "groom a shapeworks project";
//   parser.prog(prog).description(desc);

//   parser.add_option("--name").action("store").type("string").set_default("").help("Path to project file.");

//   Command::buildParser();
// }

// bool GroomCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
//   const std::string& projectFile(static_cast<std::string>(options.get("name")));

//   if (projectFile.length() == 0) {
//     std::cerr << "Must specify project name with --name <project.xlsx|.swproj>\n";
//     return false;
//   }

//   try {
//     ProjectHandle project = std::make_shared<Project>();
//     project->load(projectFile);

//     const auto oldBasePath = boost::filesystem::current_path();
//     auto base = StringUtils::getPath(projectFile);
//     if (base != projectFile) {
//       boost::filesystem::current_path(base.c_str());
//       project->set_filename(StringUtils::getFilename(projectFile));
//     }

//     Groom app(project);
//     bool success = app.run();

//     boost::filesystem::current_path(oldBasePath);
//     if (success) {
//       project->save(projectFile);
//     }
//     return success;
//   } catch (std::exception& e) {
//     std::cerr << "Error: " << e.what() << "\n";
//     return false;
//   }
// }

// ///////////////////////////////////////////////////////////////////////////////
// // Analyze
// ///////////////////////////////////////////////////////////////////////////////
// void AnalyzeCommand::buildParser() {
//   const std::string prog = "analyze";
//   const std::string desc = "Offline analysis of a shapeworks project, output json and meshes";
//   parser.prog(prog).description(desc);

//   parser.add_option("--name").action("store").type("string").set_default("").help("Path to project file.");
//   parser.add_option("--output").action("store").type("string").set_default("").help("Path to output file.");

//   Command::buildParser();
// }

// bool AnalyzeCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
//   const std::string& projectFile(static_cast<std::string>(options.get("name")));
//   const std::string& outputFile(static_cast<std::string>(options.get("output")));

//   if (projectFile.length() == 0) {
//     std::cerr << "Must specify project name with --name <project.xlsx|.swproj>\n";
//     return false;
//   }

//   if (outputFile.empty()) {
//     std::cerr << "No output file specified, must pass `--output <filename>`\n";
//     return false;
//   }

//   try {
//     ProjectHandle project = std::make_shared<Project>();

//     const auto oldBasePath = boost::filesystem::current_path();
//     auto base = StringUtils::getPath(projectFile);
//     auto filename = StringUtils::getFilename(projectFile);
//     if (base != projectFile) {
//       SW_LOG("Setting path to {}", base);
//       boost::filesystem::current_path(base.c_str());
//       project->set_filename(filename);
//     }

//     SW_LOG("Loading project: {}", filename);
//     project->load(filename);

//     Analyze analyze(project);

//     boost::filesystem::path dir(oldBasePath);
//     boost::filesystem::path file(outputFile);
//     boost::filesystem::path full_output = dir / file;
//     analyze.run_offline_analysis(full_output.string());

//     boost::filesystem::current_path(oldBasePath);

//     return true;
//   } catch (std::exception& e) {
//     std::cerr << "Error: " << e.what() << "\n";
//     return false;
//   }
// }

// ///////////////////////////////////////////////////////////////////////////////
// // Convert Project
// ///////////////////////////////////////////////////////////////////////////////
// void ConvertProjectCommand::buildParser() {
//   const std::string prog = "convert-project";
//   const std::string desc = "convert a shapeworks project (xlsx or swproj)";
//   parser.prog(prog).description(desc);

//   parser.add_option("--name").action("store").type("string").set_default("").help(
//       "Path to input project file (xlsx or swproj).");
//   parser.add_option("--output")
//       .action("store")
//       .type("string")
//       .set_default("")
//       .help("Path to output project file (xlsx or swproj).");

//   Command::buildParser();
// }

// bool ConvertProjectCommand::execute(const optparse::Values& options, SharedCommandData& sharedData) {
//   const std::string& projectFile(static_cast<std::string>(options.get("name")));
//   const std::string& outputFile(static_cast<std::string>(options.get("output")));

//   if (projectFile.length() == 0) {
//     std::cerr << "No input file specified, must pass `--name <filename>`\n";
//     return false;
//   }

//   auto extension = StringUtils::getLowerExtension(projectFile);
//   if (extension != ".xlsx" && extension != ".swproj") {
//     std::cerr << "Input project file must be either .xlsx or .swproj\n";
//     return false;
//   }

//   if (outputFile.empty()) {
//     std::cerr << "No output file specified, must pass `--output <filename>`\n";
//     return false;
//   }

//   extension = StringUtils::getLowerExtension(outputFile);
//   if (extension != ".xlsx" && extension != ".swproj") {
//     std::cerr << "Output project file must be either .xlsx or .swproj\n";
//     return false;
//   }

//   try {
//     ProjectHandle project = std::make_shared<Project>();
//     project->load(projectFile);
//     project->save(outputFile);
//     return true;
//   } catch (std::exception& e) {
//     std::cerr << "Error: " << e.what() << "\n";
//     return false;
//   }
// }
}  // namespace shapeworks
