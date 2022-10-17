#pragma once

namespace shapeworks {

  enum class CorrespondenceMode : int {
    MeanEnergy = 0,
    EnsembleEntropy = 1,
    EnsembleRegressionEntropy = 3,
    EnsembleMixedEffectsEntropy = 4,
    MeshBasedGeneralEntropy = 5,
    MeshBasedGeneralMeanEnergy = 6,
    Ssm4dEnsembleEntropy = 7,
    Ssm4dEnsembleEntropyMean = 8,
    EnsemblePolynomialRegressionEntropy = 9
  };
}
