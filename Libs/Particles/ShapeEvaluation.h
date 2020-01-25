#ifndef SHAPEWORKS_SHAPEEVALUATION_H
#define SHAPEWORKS_SHAPEEVALUATION_H

#include <string>
#include "itkParticleShapeStatistics.h"

namespace shapeworks {
class ShapeEvaluation {
public:
    ShapeEvaluation();

    void ReadPointFiles(const std::string &flistPath);
    void Evaluate();

private:
    ParticleShapeStatistics<3> particles;

    double ComputeCompactness(const int nModes);
    double ComputeGeneralizability(const int nModes);
    double ComputeSpecificity(const int nModes);
};
}


#endif //SHAPEWORKS_SHAPEEVALUATION_H
