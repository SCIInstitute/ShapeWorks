#ifndef SHAPEWORKS_PARTICLESYSTEM_H
#define SHAPEWORKS_PARTICLESYSTEM_H

#include <vector>
#include <Eigen/Core>
#include "itkParticlePositionReader.h"

class ParticleSystem {
public:
    ParticleSystem();
    bool LoadParticles(const std::vector<std::string> &filepaths);

    const Eigen::MatrixXd &Particles() const { return P; };
    int N() const { AssertLoaded(); return P.cols(); }
    int D() const { AssertLoaded(); return P.rows(); }

private:
    Eigen::MatrixXd P;
    bool isLoaded = false;

    void AssertLoaded() const { assert(isLoaded); }
};


#endif //SHAPEWORKS_PARTICLESYSTEM_H
