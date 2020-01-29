#ifndef SHAPEWORKS_PARTICLESYSTEMREADER_H
#define SHAPEWORKS_PARTICLESYSTEMREADER_H

#include <vector>
#include <Eigen/Core>
#include "itkParticlePositionReader.h"

//TODO: Get rid of itkParticleShapeStatistics,
class ParticleSystemReader {
public:
    static Eigen::MatrixXd LoadParticles(const std::vector<std::string> &filepaths);
};


#endif //SHAPEWORKS_PARTICLESYSTEMREADER_H
