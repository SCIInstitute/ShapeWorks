#ifndef SHAPEWORKS_PARTICLECOMMANDS_H
#define SHAPEWORKS_PARTICLECOMMANDS_H

#include "Command.h"

namespace shapeworks {
///////////////////////////////////////////////////////////////////////////////
class ReadParticleSystem : public ParticleSystemCommand {
public:
    static ReadParticleSystem &getCommand() {
        static ReadParticleSystem instance;
        return instance;
    }

private:
    ReadParticleSystem() { buildParser(); }

    void buildParser() override;

    int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};
}
#endif //SHAPEWORKS_PARTICLECOMMANDS_H
