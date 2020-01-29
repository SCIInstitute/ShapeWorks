#ifndef SHAPEWORKS_SHAPEEVALUATION_H
#define SHAPEWORKS_SHAPEEVALUATION_H

#include <string>
#include <Eigen/Core>
#include "itkParticleShapeStatistics.h"

namespace shapeworks {
class ShapeEvaluation {
public:
    ShapeEvaluation();

    void ReadPointFiles(const std::string &flistPath);
    void Evaluate(); //TODO: Remove
    int N() const { AssertLoaded(); return P.cols(); }
    int D() const { AssertLoaded(); return P.rows(); }

private:
    Eigen::MatrixXd P;
    bool isLoaded = false;

    double ComputeCompactness(const int nModes) const;
    double ComputeGeneralizability(const int nModes) const;
    double ComputeSpecificity(const int nModes) const;
    void AssertLoaded() const { assert(isLoaded); }
};
}


#endif //SHAPEWORKS_SHAPEEVALUATION_H
