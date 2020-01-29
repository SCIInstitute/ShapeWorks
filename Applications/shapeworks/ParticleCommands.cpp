#include "ParticleCommands.h"

namespace shapeworks {
///////////////////////////////////////////////////////////////////////////////
// ReadParticleSystem
///////////////////////////////////////////////////////////////////////////////
void ReadParticleSystem::buildParser() {
    const std::string prog = "readparticlesystem";
    const std::string desc = "reads a particle system";
    parser.prog(prog).description(desc);

    parser.add_option("--names").action("append").type("string").set_default("").help("name of file to read");

    Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int ReadParticleSystem::execute(const optparse::Values &options, SharedCommandData &sharedData) {
    std::string filename = options["name"];

    return sharedData.image.read(filename);
}
}
