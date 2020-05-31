#pragma once

#include <random>
#include <Eigen/Core>
#include <Eigen/Dense>

namespace shapeworks {
struct MultiVariateNormalRandom
{
  Eigen::VectorXd mean;
  Eigen::MatrixXd transform;

  MultiVariateNormalRandom(Eigen::MatrixXd const &covar)
          : MultiVariateNormalRandom(Eigen::VectorXd::Zero(covar.rows()), covar)
  {}

  MultiVariateNormalRandom(Eigen::VectorXd const &mean, Eigen::MatrixXd const &covar)
          : mean(mean)
  {
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(covar);
    transform = eigenSolver.eigenvectors() * eigenSolver.eigenvalues().cwiseSqrt().asDiagonal();
  }

  Eigen::MatrixXd operator()() const
  {
    //static std::mt19937 gen{ std::random_device{}() };

    // seed set as constant 1 for test repeatability
    static std::mt19937 gen{1};

    static std::normal_distribution<> dist;

    return mean + transform * Eigen::VectorXd{mean.size()}.unaryExpr([&](double x) { return dist(gen); });
  }
};

struct Reconstruction
{
  double dist;
  int shapeIdx;
  Eigen::MatrixXd rec;
};

// Sorts the reconstructions in place according to `dist` and saves them to the specified folder.
// This generates XML files which could be opened in ShapeWorksView2
void SaveReconstructions(std::vector<Reconstruction> &reconstructions, const std::vector<std::string> &srcPaths,
                         const std::string &saveTo)
{
  std::sort(reconstructions.begin(), reconstructions.end(),
            [](const Reconstruction &l, const Reconstruction &r) { return l.dist < r.dist; });

  for (int i = 0; i < reconstructions.size(); i++) {
    const int percentile =
            i == reconstructions.size() - 1 ? 100 : std::floor(((double) i / reconstructions.size()) * 100.0);

    // Save the reconstruction
    const std::string recPath = saveTo + "/" + std::to_string(percentile) + "perc.particles";
    std::ofstream recOF(recPath);
    if (!recOF) { throw std::runtime_error("Unable to open file: " + recPath); }
    recOF << reconstructions[i].rec << std::endl;
    recOF.close();

    // Create an XML file
    const std::string xmlPath = saveTo + "/" + std::to_string(percentile) + "perc.xml";
    std::ofstream xmlOF(xmlPath);
    if (!xmlOF) { throw std::runtime_error("Unable to open file: " + xmlPath); }

    xmlOF << "<point_files>"
          << srcPaths[reconstructions[i].shapeIdx] << std::endl << recPath
          << "</point_files>" << std::endl
          << "<group_ids>"
          << 1 << std::endl << 2
          << "</group_ids>";
    xmlOF.close();
  }
}
}
