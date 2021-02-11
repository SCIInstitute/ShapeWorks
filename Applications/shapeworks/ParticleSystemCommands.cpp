#include "Commands.h"
#include "ShapeEvaluation.h"

namespace shapeworks {

///////////////////////////////////////////////////////////////////////////////
// ReadParticleSystem
///////////////////////////////////////////////////////////////////////////////
void ReadParticleSystem::buildParser()
{
  const std::string prog = "read-particle-system";
  const std::string desc = "reads a particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--names").action("store").type("multistring").set_default("").help("Paths to .particle files (must be followed by `--`), ex: \"--names *.particle -- next-command...\")");

  Command::buildParser();
}

bool ReadParticleSystem::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");
  try {
    sharedData.particleSystem = ParticleSystem(filenames);
    return true;
  } catch(...) {
    std::cerr << "exception while reading particle system" << std::endl;
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Compactness
///////////////////////////////////////////////////////////////////////////////
void Compactness::buildParser()
{
  const std::string prog = "compactness";
  const std::string desc = "Compute compactness of a loaded particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use [default: %default].");
  parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the scree plots for all modes to a file.");

  Command::buildParser();
}

bool Compactness::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validParticleSystem())
  {
    std::cerr << "No valid particle system to operate on\n";
    return false;
  }

  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation::ComputeCompactness(sharedData.particleSystem, nModes, saveTo);
  std::cout << "Particle system compactness: " << r << std::endl;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Generalization
///////////////////////////////////////////////////////////////////////////////
void Generalization::buildParser()
{
  const std::string prog = "generalization";
  const std::string desc = "compute generalization of a loaded particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use [default: %default].");
  parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the reconstructions sorted by generalization along with the mapping to the original shape.");

  Command::buildParser();
}

bool Generalization::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validParticleSystem())
  {
    std::cerr << "No valid particle system to operate on\n";
    return false;
  }

  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation::ComputeGeneralization(sharedData.particleSystem, nModes, saveTo);
  std::cout << "Particle system generalization: " << r << std::endl;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Specificity
///////////////////////////////////////////////////////////////////////////////
void Specificity::buildParser()
{
  const std::string prog = "specificity";
  const std::string desc = "compute specificity of a loaded particle system";
  parser.prog(prog).description(desc);

  parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use [default: %default].");
  parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the reconstructions sorted by specificity along with the mapping to the original shape.");

  Command::buildParser();
}

bool Specificity::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validParticleSystem())
  {
    std::cerr << "No valid particle system to operate on\n";
    return false;
  }

  const int nModes = static_cast<int>(options.get("nmodes"));
  const std::string saveTo = static_cast<std::string>(options.get("saveto"));
  const double r = ShapeEvaluation::ComputeSpecificity(sharedData.particleSystem, nModes, saveTo);
  std::cout << "Particle system specificity: " << r << std::endl;

  return true;
}

} //shapeworks
