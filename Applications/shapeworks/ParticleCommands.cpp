#include <ShapeEvaluation.h>
#include "ParticleCommands.h"

namespace shapeworks {
///////////////////////////////////////////////////////////////////////////////
// ReadParticleSystem
///////////////////////////////////////////////////////////////////////////////
void ReadParticleSystem::buildParser() {
    const std::string prog = "readparticlesystem";
    const std::string desc = "reads a particle system";
    parser.prog(prog).description(desc);

    parser.add_option("--names").action("store").type("multistring").set_default("").help("name of file to read");

    Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int ReadParticleSystem::execute(const optparse::Values &options, SharedCommandData &sharedData) {
    std::vector<std::string> filenames = options.get("names");

    return sharedData.particleSystem.LoadParticles(filenames);
}

///////////////////////////////////////////////////////////////////////////////
// Compactness
///////////////////////////////////////////////////////////////////////////////
void Compactness::buildParser() {
    const std::string prog = "compactness";
    const std::string desc = "Compute compactness of a loaded particle system";
    parser.prog(prog).description(desc);

    parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use");
    parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the scree plots for all modes to a file");

    Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Compactness::execute(const optparse::Values &options, SharedCommandData &sharedData) {
    const int nModes = static_cast<int>(options.get("nmodes"));
    const std::string saveTo = static_cast<std::string>(options.get("saveto"));
    const double r = ShapeEvaluation<3>::ComputeCompactness(sharedData.particleSystem, nModes, saveTo);
    std::cout << r << std::endl;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Generalization
///////////////////////////////////////////////////////////////////////////////
void Generalization::buildParser() {
    const std::string prog = "generalization";
    const std::string desc = "Compute generalization of a loaded particle system";
    parser.prog(prog).description(desc);

    parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use");
    parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the reconstructions sorted by generalization along with the mapping to the original shape");

    Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Generalization::execute(const optparse::Values &options, SharedCommandData &sharedData) {
    const int nModes = static_cast<int>(options.get("nmodes"));
    const std::string saveTo = static_cast<std::string>(options.get("saveto"));
    const double r = ShapeEvaluation<3>::ComputeGeneralization(sharedData.particleSystem, nModes, saveTo);
    std::cout << r << std::endl;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Specificity
///////////////////////////////////////////////////////////////////////////////
void Specificity::buildParser() {
    const std::string prog = "specificity";
    const std::string desc = "Compute specificity of a loaded particle system";
    parser.prog(prog).description(desc);

    parser.add_option("--nmodes").action("store").type("int").set_default("1").help("Number of modes to use");
    parser.add_option("--saveto").action("store").type("string").set_default("").help("Save the reconstructions sorted by generalization along with the mapping to the original shape");

    Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Specificity::execute(const optparse::Values &options, SharedCommandData &sharedData) {
    const int nModes = static_cast<int>(options.get("nmodes"));
    const std::string saveTo = static_cast<std::string>(options.get("saveto"));
    const double r = ShapeEvaluation<3>::ComputeSpecificity(sharedData.particleSystem, nModes, saveTo);
    std::cout << r << std::endl;

    return true;
}
}
