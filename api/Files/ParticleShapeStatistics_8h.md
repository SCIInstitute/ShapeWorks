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

#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/algo/vnl_matrix_inverse.h"
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

  int ImportPoints(std::vector<vnl_vector<double>> points, std::vector<int> group_ids);

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

  const vnl_matrix<double> &Eigenvectors() { return m_eigenvectors; }
  const std::vector<double> &Eigenvalues() { return m_eigenvalues; }

  const vnl_vector<double> &Mean() { return m_mean; }
  const vnl_vector<double> &Group1Mean() { return m_mean1; }
  const vnl_vector<double> &Group2Mean() { return m_mean2; }

  // Returns group2 - group1 mean
  const vnl_vector<double> &NormalizedGroupDifference() { return m_groupdiffnorm; }
  const vnl_vector<double> &GroupDifference() { return m_groupdiff; }

  int ComputeMedianShape(const int ID);

  double L1Norm(unsigned int a, unsigned int b);

  Eigen::MatrixXd &PCALoadings() { return m_principals; }

  const vnl_vector<double> &FishersLDA() { return m_fishersLD; }

  const vnl_matrix<double> &ShapeMatrix() { return m_shapes; }

  const vnl_matrix<double> &RecenteredShape() { return m_pointsMinusMean; }

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

  vnl_matrix<double> m_pooled_covariance;
  vnl_matrix<double> m_eigenvectors;
  std::vector<double> m_eigenvalues;
  vnl_vector<double> m_mean;
  vnl_vector<double> m_mean1;
  vnl_vector<double> m_mean2;
  vnl_matrix<double> m_pointsMinusMean;
  vnl_matrix<double> m_shapes;
  vnl_matrix<double> m_projectedPMM1;
  vnl_matrix<double> m_projectedPMM2;
  vnl_vector<double> m_projectedMean1;
  vnl_vector<double> m_projectedMean2;
  std::vector<double> m_fishersProjection;
  std::vector<double> m_percentVarByMode;
  vnl_vector<double> m_fishersLD;
  Eigen::MatrixXd m_principals;

  vnl_vector<double> m_groupdiff;
  vnl_vector<double> m_groupdiffnorm;

  // used to keep the points' files that needs to be reloaded when new updates come in.
  std::vector<std::string> m_pointsfiles;

  Eigen::MatrixXd m_Matrix;

  Eigen::MatrixXd m_group_1_matrix;
  Eigen::MatrixXd m_group_2_matrix;
};

} // shapeworks
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000
