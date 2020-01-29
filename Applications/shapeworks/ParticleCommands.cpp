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
    for(int i=0; i<filenames.size(); i++) {
        std::cout << "Filenames: " << filenames[i] << std::endl;
    }


    return 0;
}
}
