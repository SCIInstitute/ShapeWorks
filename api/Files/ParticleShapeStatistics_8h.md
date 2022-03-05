---
title: Libs/Particles/ParticleShapeStatistics.h

---

# Libs/Particles/ParticleShapeStatistics.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md)**  |




## Source code

```cpp
#pragma once

#include <Eigen/Eigen>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>

#include "itkParticlePositionReader.h"
#include "itkParticlePositionWriter.h"
#include "Shapeworks.h"
#include "ParticleSystem.h"

namespace shapeworks {
class ParticleShapeStatistics
{
public:

  constexpr static int VDimension = 3;

  ParticleShapeStatistics() {}
  ~ParticleShapeStatistics() {}

  int DoPCA(std::vector<std::vector<Point>> global_pts, int domainsPerShape = 1);

  int DoPCA(ParticleSystem particleSystem, int domainsPerShape = 1);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  int ImportPoints(std::vector<Eigen::VectorXd> points, std::vector<int> group_ids);

  int ReadPointFiles(const std::string &s);

  int ReloadPointFiles();

  int WriteCSVFile(const std::string &s);
  int WriteCSVFile2(const std::string &s);

  int ComputeModes();

  int PrincipalComponentProjections();

  int FisherLinearDiscriminant(unsigned int numModes);

  const int SampleSize() { return m_numSamples; }
  const int Group1SampleSize() { return m_numSamples1; }
  const int Group2SampleSize() { return m_numSamples2; }

  const int NumberOfDimensions() { return m_numDimensions; }

  const int GroupID(unsigned int i) { return m_groupIDs[i]; }
  const std::vector<int> &GroupID() { return m_groupIDs;}

  const Eigen::MatrixXd &Eigenvectors() { return m_eigenvectors; }
  const std::vector<double> &Eigenvalues() { return m_eigenvalues; }

  const Eigen::VectorXd &Mean() { return m_mean; }
  const Eigen::VectorXd &Group1Mean() { return m_mean1; }
  const Eigen::VectorXd &Group2Mean() { return m_mean2; }

  // Returns group2 - group1 mean
  const Eigen::VectorXd &NormalizedGroupDifference() { return m_groupdiffnorm; }
  const Eigen::VectorXd &GroupDifference() { return m_groupdiff; }

  int ComputeMedianShape(const int ID);

  double L1Norm(unsigned int a, unsigned int b);

  Eigen::MatrixXd &PCALoadings() { return m_principals; }

  const Eigen::VectorXd &FishersLDA() { return m_fishersLD; }

  const Eigen::MatrixXd &ShapeMatrix() { return m_shapes; }

  const Eigen::MatrixXd &RecenteredShape() { return m_pointsMinusMean; }

  std::vector<double> PercentVarByMode() { return m_percentVarByMode; }

  int SimpleLinearRegression(const std::vector<double> &y,
                             const std::vector<double> &x,
                             double &a, double &b) const;

  Eigen::VectorXd get_compactness(std::function<void(float)> progress_callback);
  Eigen::VectorXd get_specificity(std::function<void (float)> progress_callback);
  Eigen::VectorXd get_generalization(std::function<void (float)> progress_callback);

  Eigen::MatrixXd get_group1_matrix();
  Eigen::MatrixXd get_group2_matrix();

private:

  unsigned int m_numSamples1;
  unsigned int m_numSamples2;
  unsigned int m_numSamples;
  unsigned int m_domainsPerShape;
  unsigned int m_numDimensions;
  std::vector<int> m_groupIDs;

  Eigen::MatrixXd m_eigenvectors;
  std::vector<double> m_eigenvalues;
  Eigen::VectorXd m_mean;
  Eigen::VectorXd m_mean1;
  Eigen::VectorXd m_mean2;
  Eigen::MatrixXd m_pointsMinusMean;
  Eigen::MatrixXd m_shapes;
  Eigen::MatrixXd m_projectedPMM1;
  Eigen::MatrixXd m_projectedPMM2;
  Eigen::VectorXd m_projectedMean1;
  Eigen::VectorXd m_projectedMean2;
  std::vector<double> m_fishersProjection;
  std::vector<double> m_percentVarByMode;
  Eigen::VectorXd m_fishersLD;
  Eigen::MatrixXd m_principals;

  Eigen::VectorXd m_groupdiff;
  Eigen::VectorXd m_groupdiffnorm;

  // used to keep the points' files that needs to be reloaded when new updates come in.
  std::vector<std::string> m_pointsfiles;

  Eigen::MatrixXd m_Matrix;

  Eigen::MatrixXd m_group_1_matrix;
  Eigen::MatrixXd m_group_2_matrix;
};

} // shapeworks
```


-------------------------------

Updated on 2022-03-05 at 23:20:35 +0000
