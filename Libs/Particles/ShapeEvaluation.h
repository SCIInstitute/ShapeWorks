#ifndef SHAPEWORKS_SHAPEEVALUATION_H
#define SHAPEWORKS_SHAPEEVALUATION_H

#include <string>
#include <Eigen/Core>
#include "itkParticleShapeStatistics.h"
#include "ParticleSystem.h"

namespace shapeworks {

template <unsigned int VDimension=3>
class ShapeEvaluation {
public:
    ShapeEvaluation();

    static double ComputeCompactness(const ParticleSystem &particleSystem, const int nModes, const std::string &saveScreePlotTo="");
    static double ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo="");
    static double ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo="");
};
}


#endif //SHAPEWORKS_SHAPEEVALUATION_H
