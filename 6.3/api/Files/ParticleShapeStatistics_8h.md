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

class Project;

class ParticleShapeStatistics {

public:

  constexpr static int VDimension = 3;

  ParticleShapeStatistics() {};
  ParticleShapeStatistics(std::shared_ptr<Project> project);
  ~ParticleShapeStatistics() {};

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

  int SampleSize() const { return m_numSamples; }
  int Group1SampleSize() const { return m_numSamples1; }
  int Group2SampleSize() const { return m_numSamples2; }

  int NumberOfDimensions() const { return m_numDimensions; }

  int GroupID(unsigned int i) const { return m_groupIDs[i]; }
  const std::vector<int> &GroupID() const { return m_groupIDs; }

  const Eigen::MatrixXd &Eigenvectors() const { return m_eigenvectors; }
  const std::vector<double> &Eigenvalues() const { return m_eigenvalues; }

  const Eigen::VectorXd &Mean() const { return m_mean; }
  const Eigen::VectorXd &Group1Mean() const { return m_mean1; }
  const Eigen::VectorXd &Group2Mean() const { return m_mean2; }

  const Eigen::VectorXd &NormalizedGroupDifference() const { return m_groupdiffnorm; }
  const Eigen::VectorXd &GroupDifference() const { return m_groupdiff; }

  int ComputeMedianShape(const int ID);

  double L1Norm(unsigned int a, unsigned int b);

  Eigen::MatrixXd &PCALoadings() { return m_principals; }

  const Eigen::VectorXd &FishersLDA() const { return m_fishersLD; }

  const Eigen::MatrixXd &ShapeMatrix() const { return m_shapes; }

  const Eigen::MatrixXd &RecenteredShape() const { return m_pointsMinusMean; }

  const std::vector<double>& PercentVarByMode() const { return m_percentVarByMode; }

  int SimpleLinearRegression(const std::vector<double> &y,
                             const std::vector<double> &x,
                             double &a, double &b) const;

  Eigen::VectorXd get_compactness(std::function<void(float)> progress_callback);
  Eigen::VectorXd get_specificity(std::function<void (float)> progress_callback);
  Eigen::VectorXd get_generalization(std::function<void (float)> progress_callback);

  Eigen::MatrixXd get_group1_matrix();
  Eigen::MatrixXd get_group2_matrix();

private:

  void compute_good_bad_points();

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

  // 0 = bad, 1 = good
  std::vector<bool> m_goodPoints;
  std::vector<Eigen::VectorXd> points_;
};

} // shapeworks
```


-------------------------------

Updated on 2022-05-14 at 00:09:10 +0000
