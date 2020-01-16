#ifndef SHAPEWORKS_EVALUATESSM_H
#define SHAPEWORKS_EVALUATESSM_H

#include "Command.h"

namespace shapeworks {

//TODO: find out how to organize this from Cam/Archna
class EvaluateSSM : public Command
{
public:
    static EvaluateSSM& getCommand() { static EvaluateSSM instance; return instance; }

private:
    EvaluateSSM() { buildParser(); }
    void buildParser() override;
    int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

}

#endif //SHAPEWORKS_EVALUATESSM_H
