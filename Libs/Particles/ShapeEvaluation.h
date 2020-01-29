#ifndef SHAPEWORKS_SHAPEEVALUATION_H
#define SHAPEWORKS_SHAPEEVALUATION_H

#include <string>
#include <Eigen/Core>
#include "itkParticleShapeStatistics.h"
#include "ParticleSystem.h"

namespace shapeworks {
class ShapeEvaluation {
public:
    ShapeEvaluation();

    void ReadPointFiles(const std::string &flistPath);
    void Evaluate(); //TODO: Remove

private:
    ParticleSystem particleSystem;

    double ComputeCompactness(const int nModes, const std::string &saveScreePlotTo="") const;
    double ComputeGeneralizability(const int nModes) const;
    double ComputeSpecificity(const int nModes) const;
};
}


#endif //SHAPEWORKS_SHAPEEVALUATION_H
