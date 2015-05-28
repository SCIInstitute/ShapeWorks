/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: RiemannianStatistics.txx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
template<class PointType, class TangentType, class ScalarType>
void
RiemannianStatistics<PointType, TangentType, ScalarType>
::computeMean(const PointListType & pointList, PointType & mean) const
{
  const ScalarType EPSILON = 1.0e-12;
  
  TangentType tangent, initialTangent;
  ScalarType lastNormSquared, normSquared, initialNormSquared;
  ScalarType currStepSize = stepSize;
  int i;

  if(pointList.size() == 0)
    return;

  mean = pointList[0];

  tangent = riemannianGeometry->getZeroVector();
  for(i = 0; i < pointList.size(); i++)
    tangent += riemannianGeometry->logMap(mean, pointList[i]);

  tangent = tangent * (1.0 / static_cast<ScalarType>(pointList.size()));
  initialTangent = tangent;

  initialNormSquared = riemannianGeometry->normSquared(mean, tangent);
  lastNormSquared = initialNormSquared;
  while(lastNormSquared >= EPSILON)
  {
    mean = riemannianGeometry->expMap(mean, tangent * currStepSize);

    tangent = riemannianGeometry->getZeroVector();
    for(i = 0; i < pointList.size(); i++)
      tangent += riemannianGeometry->logMap(mean, pointList[i]);

    tangent = tangent * (1.0 / static_cast<ScalarType>(pointList.size()));

    normSquared = riemannianGeometry->normSquared(mean, tangent);
    if(normSquared >= lastNormSquared)
    {
      currStepSize *= 0.5;
      mean = pointList[0];
      lastNormSquared = initialNormSquared;
      tangent = initialTangent;
    }
    else
      lastNormSquared = normSquared;
  }
}

template<class PointType, class TangentType, class ScalarType>
void
RiemannianStatistics<PointType, TangentType, ScalarType>
::computeWeightedAve(const ScalarListType & weightList,
                     const PointListType & pointList,
                     PointType & weightedAve) const
{
  const ScalarType EPSILON = 1.0e-12;
  TangentType tangent, initialTangent;
  ScalarType lastNormSquared, normSquared, initialNormSquared;
  ScalarType currStepSize = stepSize;
  int i;

  if(pointList.size() == 0 ||
     pointList.size() != weightList.size())
    return;

  weightedAve = pointList[0];

  tangent = riemannianGeometry->getZeroVector();
  for(i = 0; i < pointList.size(); i++)
    tangent += weightList[i] *
      riemannianGeometry->logMap(weightedAve, pointList[i]);

  initialTangent = tangent;

  initialNormSquared = riemannianGeometry->normSquared(weightedAve, tangent);
  lastNormSquared = initialNormSquared;
  while(lastNormSquared >= EPSILON)
  {
    weightedAve = riemannianGeometry->expMap(weightedAve, tangent * currStepSize);

    tangent = riemannianGeometry->getZeroVector();
    for(i = 0; i < pointList.size(); i++)
      tangent += weightList[i] *
        riemannianGeometry->logMap(weightedAve, pointList[i]);

    normSquared = riemannianGeometry->normSquared(weightedAve, tangent);
    if(normSquared >= lastNormSquared)
    {
      currStepSize *= 0.5;
      weightedAve = pointList[0];
      lastNormSquared = initialNormSquared;
      tangent = initialTangent;
    }
    else
      lastNormSquared = normSquared;
  }
}

template<class PointType, class TangentType, class ScalarType>
void
RiemannianStatistics<PointType, TangentType, ScalarType>
::computeMeanAndCovariance(const PointListType & pointList,
                           PointType & mean,
                           CovarianceType & covariance) const
{
  PointType logPoint;
  int i, j, k;

  int dimension = riemannianGeometry->getDimension();
  covariance.set_size(dimension);

  computeMean(pointList, mean);

  for(j = 0; j < dimension; j++)
  {
    for(i = j; i < dimension; i++)
      covariance(i, j) = 0.0;
  }

  for(i = 0; i < pointList.size(); i++)
  {
    logPoint = riemannianGeometry->logMap(mean, pointList[i]);

    for(k = 0; k < dimension; k++)
    {
      for(j = k; j < dimension; j++)
        covariance(j, k) += logPoint[j] * logPoint[k];
    }
  }

  covariance *= (1.0 / static_cast<ScalarType>(pointList.size() + 1));
}

template<class PointType, class TangentType, class ScalarType>
void
RiemannianStatistics<PointType, TangentType, ScalarType>
::computeMeanAndPGA(const PointListType & pointList, PointType & mean,
                    PGAVariancesType & pgaVariances,
                    PGAVectorsMatrixType & pgaVectorsMatrix) const
{
  CovarianceType covariance;

  computeMeanAndCovariance(pointList, mean, covariance);

  vnl_symmetric_eigensystem<ScalarType> eig(covariance.as_matrix());
  pgaVariances = eig.D;
  pgaVectorsMatrix = eig.V;
}

template<class PointType, class TangentType, class ScalarType>
PointType
RiemannianStatistics<PointType, TangentType, ScalarType>
::randomGaussianPoint(const PointType & mean, ScalarType sigma) const
{
  ScalarType x, y, r2;
  int i;

  TangentType logResult = riemannianGeometry->getZeroVector();
  int dimension = riemannianGeometry->getDimension();

  // Generate random gaussians with polar Box-Muller method
  for(i = 0; i < dimension; i++)
  {
    r2 = 0;
    while(r2 > 1.0 || r2 == 0)
    {
      x = (2.0 * static_cast<ScalarType>(rand()) /
           static_cast<ScalarType>(RAND_MAX + 1.0)) - 1.0;
      y = (2.0 * static_cast<ScalarType>(rand()) /
           static_cast<ScalarType>(RAND_MAX + 1.0)) - 1.0;

      r2 = x * x + y * y;
    }

    logResult[i] = sigma * y * sqrt(-2.0 * log(r2) / r2);
  }

  return riemannianGeometry->expMap(mean, logResult);
}
