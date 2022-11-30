#include "ReconstructSurface.h"
#include "Procrustes3D.h"
#include "VectorImage.h"
#include "ParticleShapeStatistics.h"
#include "StringUtils.h"
#include "Utils.h"

#include <sys/stat.h>
#include <vtkKdTreePointLocator.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

#ifdef _WIN32
  #include <direct.h>
#endif

namespace shapeworks {

template<class TransformType>
ReconstructSurface<TransformType>::ReconstructSurface(const std::string &denseFile, const std::string &sparseFile, const std::string &goodPointsFile)
{
  this->denseMean = Mesh(denseFile).getVTKMesh();
  this->sparseMean = setSparseMean(sparseFile);
  this->goodPoints = setGoodPoints(goodPointsFile);
  this->denseDone = true;
}

template<class TransformType>
vtkSmartPointer<vtkPoints> ReconstructSurface<TransformType>::setSparseMean(const std::string& sparsePath)
{
  int nPoints = 0;
  std::ifstream ptsIn0(sparsePath.c_str());
  vtkSmartPointer<vtkPoints> sparsePoints = vtkSmartPointer<vtkPoints>::New();

  while (ptsIn0.good())
  {
    double x, y, z;
    ptsIn0 >> x >> y >> z;
    if (ptsIn0.good())
    {
      sparsePoints->InsertNextPoint(x, y, z);
      nPoints++;
    }
  }
  ptsIn0.close();

  return sparsePoints;
}

template<class TransformType>
std::vector<bool> ReconstructSurface<TransformType>::setGoodPoints(const std::string& pointsPath)
{
  std::ifstream ptsIn(pointsPath.c_str());
  std::vector<bool> goodPoints;

  if(ptsIn.good())
  {
    while (ptsIn.good())
    {
      int i;
      ptsIn >> i;
      if (ptsIn.good())
      {
        goodPoints.push_back(i == 0 ? false : true);
      }
    }
  }
  else
  {
    // good point file is not given if a template mesh is used instead of a mean just assume all are good
    for(size_t i = 0; i < sparseMean->GetNumberOfPoints(); i++)
      goodPoints.push_back(true);
  }
  ptsIn.close();

  return goodPoints;
}

template<class TransformType>
double ReconstructSurface<TransformType>::computeAverageDistanceToNeighbors(Mesh::MeshPoints points, std::vector<int> particlesIndices)
{
  int K = 6; // hexagonal ring - one jump
  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);

  // Create the tree
  vtkSmartPointer<vtkKdTreePointLocator> kDTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  kDTree->SetDataSet(polydata);
  kDTree->BuildLocator();

  // Find the closest points to each particle
  double avgDist = 0;
  for (unsigned int i = 0; i < particlesIndices.size(); i++)
  {
    double p[3];
    points->GetPoint(particlesIndices[i], p);

    vtkSmartPointer<vtkIdList> result = vtkSmartPointer<vtkIdList>::New();
    kDTree->FindClosestNPoints(K + 1, p, result); // +1 to exclude myself

    double meanDist = 0;
    for (vtkIdType k = 0; k < K + 1; k++)
    {
      vtkIdType pid = result->GetId(k);

      if (pid == particlesIndices[i]) // close to myself
        continue;

      double pk[3];
      points->GetPoint(pid, pk);

      double curDist = sqrt(pow(p[0] - pk[0], 2.0) + pow(p[1] - pk[1], 2.0) + pow(p[2] - pk[2], 2.0));

      meanDist += curDist;
    }
    meanDist /= K;
    avgDist += meanDist;
  }

  avgDist /= particlesIndices.size();
  return avgDist;
}

template<class TransformType>
void ReconstructSurface<TransformType>::checkMapping(TransformTypePtr transform, Mesh::MeshPoints sourcePoints, Mesh::MeshPoints targetPoints)
{
  // source should be warped to the target
  double rms = 0.0;
  double rms_wo_mapping = 0.0;
  Mesh::MeshPoints mappedCorrespondences = Mesh::MeshPoints::New();

  for (unsigned int i = 0; i < sourcePoints->GetNumberOfPoints(); i++)
  {
    double ps[3]; // source
    double pt[3]; // target
    double pw[3]; // warped

    sourcePoints->GetPoint(i, ps);
    targetPoints->GetPoint(i, pt);

    Point3 ps_({ps[0], ps[1], ps[2]});
    Point3 pt_({pt[0], pt[1], pt[2]});
    Point3 pw_({pw[0], pw[1], pw[2]});

    pw_ = transform->TransformPoint(ps_);

    double cur_rms = pw_.EuclideanDistanceTo(pt_);
    double cur_rms_wo_mapping = ps_.EuclideanDistanceTo(pt_);

    rms += cur_rms;
    rms_wo_mapping += cur_rms_wo_mapping;

    pw[0] = pw_[0]; pw[1] = pw_[1]; pw[2] = pw_[2];
    mappedCorrespondences->InsertNextPoint(pw);
  }

  double maxmDist = double(-10000.0f);
  for (unsigned int i = 0; i < mappedCorrespondences->GetNumberOfPoints(); i++)
  {
    double pi[3];
    mappedCorrespondences->GetPoint(i, pi);
    Point3 pi_({pi[0], pi[1], pi[2]});

    for (unsigned int j = 0; j < mappedCorrespondences->GetNumberOfPoints(); j++)
    {
      double pj[3];
      mappedCorrespondences->GetPoint(j, pj);
      Point3 pj_({pj[0], pj[1], pj[2]});

      double dist = pi_.EuclideanDistanceTo(pj_);
      if (dist > maxmDist)
      {
        maxmDist = dist;
      }
    }
  }

  rms /= sourcePoints->GetNumberOfPoints();
  rms_wo_mapping /= sourcePoints->GetNumberOfPoints();
}

template<class TransformType>
void ReconstructSurface<TransformType>::generateWarpedMeshes(TransformTypePtr transform, vtkSmartPointer<vtkPolyData>& outputMesh)
{
  vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
  vertices->DeepCopy(outputMesh->GetPoints());

  unsigned int numPointsToTransform = vertices->GetNumberOfPoints();
  for (unsigned int i = 0; i < numPointsToTransform; i++)
  {
    double meshPoint[3];
    vertices->GetPoint(i, meshPoint);

    Point3 pm_({meshPoint[0], meshPoint[1], meshPoint[2]});

    Point3 pw_;
    pw_ = transform->TransformPoint(pm_);

    vertices->SetPoint(i, pw_[0], pw_[1], pw_[2]);
  }

  outputMesh->SetPoints(vertices);
  outputMesh->Modified();
}

template<class TransformType>
Mesh ReconstructSurface<TransformType>::getMesh(std::vector<Point3> localPoints)
{
  if (!this->denseDone) { return vtkSmartPointer<vtkPolyData>::New(); }

  std::vector<int> particlesIndices;
  for (int i = 0; i < this->goodPoints.size(); i++)
  {
    if (this->goodPoints[i])
      particlesIndices.push_back(i);
  }

  vtkSmartPointer<vtkPoints> subjectPoints = vtkSmartPointer<vtkPoints>::New();
  for (auto &a : localPoints)
    subjectPoints->InsertNextPoint(a[0], a[1], a[2]);

  double sigma = computeAverageDistanceToNeighbors(subjectPoints, particlesIndices);

  PointIdType id;
  id = itk::NumericTraits<PointIdType>::Zero;
  typename PointSetType::Pointer sourceLandMarks = PointSetType::New();
  typename PointSetType::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
  Point3 ps;
  int ns = 0;
  for (int i = 0; i < localPoints.size(); i++)
  {
    if (std::find(particlesIndices.begin(), particlesIndices.end(), i) != particlesIndices.end())
    {
      double p[3];
      this->sparseMean->GetPoint(i, p);
      ps[0] = p[0];
      ps[1] = p[1];
      ps[2] = p[2];
      sourceLandMarkContainer->InsertElement(id++, ps);
      ns++;
    }
  }

  typename TransformType::Pointer transform = TransformType::New();
  transform->SetSigma(sigma);
  transform->SetStiffness(1e-10);
  transform->SetSourceLandmarks(sourceLandMarks);

  id = itk::NumericTraits<PointIdType>::Zero;
  typename PointSetType::Pointer targetLandMarks = PointSetType::New();
  typename PointSetType::PointsContainer::Pointer targetLandMarkContainer = targetLandMarks->GetPoints();
  Point3 pt;
  int nt = 0;
  for (int i = 0; i < localPoints.size(); i++)
  {
    if (std::find(particlesIndices.begin(), particlesIndices.end(), i) != particlesIndices.end())
    {
      double p[3];
      subjectPoints->GetPoint(i, p);
      pt[0] = p[0];
      pt[1] = p[1];
      pt[2] = p[2];
      targetLandMarkContainer->InsertElement(id++, pt);
      nt++;
    }
  }
  transform->SetTargetLandmarks(targetLandMarks);

  checkMapping(transform, this->sparseMean, subjectPoints);

  vtkSmartPointer<vtkPolyData> denseShape = vtkSmartPointer<vtkPolyData>::New();
  denseShape->DeepCopy(this->denseMean);
  generateWarpedMeshes(transform, denseShape);
  return Mesh(denseShape);
}

template<class TransformType>
Mesh::MeshPoints ReconstructSurface<TransformType>::convertToImageCoordinates(Mesh::MeshPoints particles, const Vector& spacing, const Point3& origin)
{
  Mesh::MeshPoints points = Mesh::MeshPoints::New();
  for (unsigned int i = 0; i < particles->GetNumberOfPoints(); i++)
  {
    double p[3];
    particles->GetPoint(i, p);
    points->InsertNextPoint((p[0] - origin[0]) / ((double)spacing[0]),
                            (p[1] - origin[1]) / ((double)spacing[1]),
                            (p[2] - origin[2]) / ((double)spacing[2]));
  }
  return points;
}

template<class TransformType>
int ReconstructSurface<TransformType>::computeMedianShape(std::vector<Eigen::MatrixXd>& shapeList)
{
  int medianShapeIndex = -1;
  double minSum = 1e10;

  for(int i = 0; i < shapeList.size(); i++)
  {
    Eigen::MatrixXd shape_i = shapeList[i];
    double sum = 0.0;

    for(int j = 0; j < shapeList.size(); j++)
    {
      if(i == j)
        continue;

      Eigen::MatrixXd shape_j = shapeList[j];

      for(int k = 0; k < shape_i.rows(); k++)
        sum += (fabs(shape_i(k, 0) - shape_j(k, 0)) +
                fabs(shape_i(k, 1) - shape_j(k, 1)) +
                fabs(shape_i(k, 2) - shape_j(k, 2)));
    }
    sum /= shapeList.size();

    if(sum < minSum)
    {
      minSum = sum;
      medianShapeIndex = i;
    }
  }

  return medianShapeIndex;
}

template<class TransformType>
void ReconstructSurface<TransformType>::performKMeansClustering(std::vector<std::vector<Point3>> worldPoints, int numberOfParticles, std::vector<int>& centroidIndices)
{
  unsigned int numOfShapes = worldPoints.size();
  if (this->numOfClusters > numOfShapes)
    this->numOfClusters = numOfShapes;

  std::vector<Eigen::MatrixXd> shapeList;
  Eigen::MatrixXd shapeVector(numberOfParticles, 3);
  shapeVector.fill(0.0);

  for (unsigned int shapeNo = 0; shapeNo < numOfShapes; shapeNo++)
  {
    for (unsigned int i = 0; i < numberOfParticles; i++)
    {
      shapeVector(i,0)= worldPoints[shapeNo][i][0];
      shapeVector(i,1) = worldPoints[shapeNo][i][1];
      shapeVector(i,2) = worldPoints[shapeNo][i][2];
    }
    shapeList.push_back(shapeVector);
  }
  std::vector<int> centers(this->numOfClusters, 0);
  if(this->numOfClusters == 1)
  {
    // this should be the median shape rather than a random sample
    centers[0] = computeMedianShape(shapeList);
    centroidIndices = centers;
    std::cout << "One cluster is given ... and the median shape is used ... \n";
    return;
  }

  unsigned int seed = unsigned(std::time(0));
  std::srand(seed);
  centers[0] = rand() % int(numOfShapes);
  std::cout << "Setting center[0] to shape #" << centers[0] << std::endl;
  unsigned int countCenters = 1;

  while (countCenters < this->numOfClusters)
  {
    Eigen::MatrixXd distMat(numOfShapes, countCenters);
    distMat.fill(0.0);
    Eigen::VectorXd minDists(numOfShapes);
    minDists.fill(0.0);
    Eigen::VectorXd probs(numOfShapes);
    probs.fill(0.0);
    for (unsigned int s = 0; s < numOfShapes; s++)
    {
      for (unsigned int c = 0; c < countCenters; c++)
      {
        if (s == int(centers[c]))
        {
          distMat.row(s).fill(0.0);
          break;
        }
        shapeVector = shapeList[s] - shapeList[int(centers[c])];
        distMat(s, c) = shapeVector.norm();
      }
      minDists(s) = distMat.row(s).minCoeff(); // need to find minimum
      probs(s) = minDists(s) * minDists(s);
    }
    probs.operator /= (probs.sum());
    Eigen::VectorXd cumProbs(numOfShapes);
    cumProbs.fill(0.0);

    for (unsigned int s = 0; s < numOfShapes; s++)
      cumProbs[s] = probs.topLeftCorner(s + 1, 0).sum();

    double r = double(rand() % 10000);
    r = r / 10000.0;
    for (unsigned int s = 0; s < numOfShapes; s++)
    {
      if (r < cumProbs[s])
      {
        if (probs[s] == 0.0)
          continue;
        else
        {
          centers[countCenters] = int(s);
          countCenters += 1;
          break;
        }
      }
    }
    std::cout << "Setting center[" << countCenters - 1 << "] to shape #" << centers[countCenters - 1] << std::endl;
  }
  std::cout << "KMeans++ finished...." << std::endl;
  centroidIndices = centers;
}

template<class TransformType>
Eigen::MatrixXd ReconstructSurface<TransformType>::computeParticlesNormals(vtkSmartPointer<vtkPoints> particles, Image dt)
{
  Vector spacing = dt.spacing();
  Point3 origin = dt.origin();

  VectorImage vectorImage(dt);
  dt.applyGradientFilter();

  VectorImage::ImageIterator gradIter = vectorImage.iterator();
  Image::ImageIterator magIter = dt.iterator();

  Image::ImageType::Pointer nxImage = Image::ImageType::New();
  nxImage->SetRegions(dt.getITKImage()->GetLargestPossibleRegion());
  nxImage->Allocate();
  Image::ImageIterator nxIter(nxImage, nxImage->GetRequestedRegion());

  Image::ImageType::Pointer nyImage = Image::ImageType::New();
  nyImage->SetRegions(dt.getITKImage()->GetLargestPossibleRegion());
  nyImage->Allocate();
  Image::ImageIterator nyIter(nyImage, nyImage->GetRequestedRegion());

  Image::ImageType::Pointer nzImage = Image::ImageType::New();
  nzImage->SetRegions(dt.getITKImage()->GetLargestPossibleRegion());
  nzImage->Allocate();
  Image::ImageIterator nzIter(nzImage, nzImage->GetRequestedRegion());

  for (gradIter.GoToBegin(), magIter.GoToBegin(), nxIter.GoToBegin(), nyIter.GoToBegin(), nzIter.GoToBegin();
       !gradIter.IsAtEnd();
       ++gradIter, ++magIter, ++nxIter, ++nyIter, ++nzIter)
  {
    Covariant grad = gradIter.Get();
    float gradMag = magIter.Get();

    float nx = -1.0f*grad[0] / (1e-10f + gradMag);
    float ny = -1.0f*grad[1] / (1e-10f + gradMag);
    float nz = -1.0f*grad[2] / (1e-10f + gradMag);

    nxIter.Set(nx);
    nyIter.Set(ny);
    nzIter.Set(nz);
  }

  vtkSmartPointer<vtkPolyData> polyParticles = vtkSmartPointer<vtkPolyData>::New();
  polyParticles->SetPoints(particles);
  vtkSmartPointer<vtkPoints> pts = convertToImageCoordinates(polyParticles->GetPoints(), spacing, origin);
  polyParticles->SetPoints(pts);

  Mesh particlesData(polyParticles);
  Mesh probeX = particlesData.probeVolume(Image(nxImage));
  Mesh probeY = particlesData.probeVolume(Image(nyImage));
  Mesh probeZ = particlesData.probeVolume(Image(nzImage));

  vtkFloatArray* nx = vtkFloatArray::SafeDownCast(probeX.getVTKMesh()->GetPointData()->GetScalars());
  vtkFloatArray* ny = vtkFloatArray::SafeDownCast(probeY.getVTKMesh()->GetPointData()->GetScalars());
  vtkFloatArray* nz = vtkFloatArray::SafeDownCast(probeZ.getVTKMesh()->GetPointData()->GetScalars());

  Eigen::MatrixXd particlesNormals(particles->GetNumberOfPoints(), 3);
  for (int i = 0; i < particlesData.numPoints(); i++)
  {
    double pN[3];
    pN[0] = nx->GetValue(i);
    pN[1] = ny->GetValue(i);
    pN[2] = nz->GetValue(i);

    double norm = sqrt(pN[0] * pN[0] + pN[1] * pN[1] + pN[2] * pN[2]);
    pN[0] /= norm;
    pN[1] /= norm;
    pN[2] /= norm;

    particlesNormals(i, 0) = pN[0];
    particlesNormals(i, 1) = pN[1];
    particlesNormals(i, 2) = pN[2];
  }

  return particlesNormals;
}

template<class TransformType>
void ReconstructSurface<TransformType>::writeMeanInfo()
{
  Mesh denseMesh(this->denseMean);
  denseMesh.write(this->outPath + "/" + "_dense_rcout.vtk");

  std::ofstream sparsePtsOut((this->outPath + "/" +" _sparse.particles").c_str());
  auto sparsePts = this->sparseMean;
  for (int i = 0; i < this->goodPoints.size(); i++)
  {
    auto pt = sparsePts->GetPoint(i);
    sparsePtsOut << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
  }
  sparsePtsOut.close();

  std::ofstream goodPtsOut((this->outPath + "/" +"_goodPoints.txt").c_str());
  auto goodPts = this->goodPoints;
  for (auto a : goodPts)
    goodPtsOut << a << std::endl;
  goodPtsOut.close();

  std::string outfilenameGood = this->outPath + "/" + "_good-sparse.particles";
  std::string outfilenameBad  = this->outPath + "/" + "_bad-sparse.particles";
  std::ofstream ofsG, ofsB;
  ofsG.open(outfilenameGood.c_str());
  ofsB.open(outfilenameBad.c_str());
  for (int i = 0; i < this->goodPoints.size(); i++)
  {
    auto pt = sparsePts->GetPoint(i);
    if(this->goodPoints[i])
      ofsG << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
    else
      ofsB << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
  }
  ofsG.close();
  ofsB.close();
}

template<class TransformType>
vtkSmartPointer<vtkPolyData> ReconstructSurface<TransformType>::getDenseMean(std::vector<std::vector<Point3>> localPoints, std::vector<std::vector<Point3>> worldPoints, std::vector<std::string> distanceTransform)
{
  if (!this->denseDone || !localPoints.empty() || !distanceTransform.empty() || !worldPoints.empty())
  {
    this->denseDone = false;
    if (localPoints.empty() || distanceTransform.empty() || worldPoints.empty() || localPoints.size() != distanceTransform.size())
    {
      throw std::runtime_error("Invalid input for reconstruction!");
    }
    this->computeDenseMean(localPoints, worldPoints, distanceTransform);
  }

  return this->denseMean;
}

template<class TransformType>
void ReconstructSurface<TransformType>::computeDenseMean(std::vector<std::vector<Point3>> localPoints, std::vector<std::vector<Point3>> worldPoints, std::vector<std::string> distanceTransform)
{
  try
  {
    // turn the sets of global points to one sparse global mean.
    std::vector<Point3> sparseMeanPoint = std::vector<Point3>(worldPoints[0].size(), Point3({0.f, 0.f, 0.f}));
    for (auto &a : worldPoints)
    {
      for (int i = 0; i < a.size(); i++)
        sparseMeanPoint[i] += Point3({a[i][0], a[i][1], a[i][2]});
    }

    for (int i = 0; i < sparseMeanPoint.size(); i++)
      sparseMeanPoint[i] /= static_cast<float>(worldPoints.size());

    this->sparseMean = Mesh::MeshPoints::New();
    for (auto &a : sparseMeanPoint)
      this->sparseMean->InsertNextPoint(a[0], a[1], a[2]);

    std::vector<Eigen::MatrixXd> normals;
    std::vector<Mesh::MeshPoints> subjectPoints;
    for (int shape = 0; shape < localPoints.size(); shape++)
    {
      subjectPoints.push_back(Mesh::MeshPoints::New());
      for (auto &a : localPoints[shape])
        subjectPoints[shape]->InsertNextPoint(a[0], a[1], a[2]);

      normals.push_back(this->computeParticlesNormals(subjectPoints[shape], Image(distanceTransform[shape])));
    }

    // now decide whether each particle is a good based on dispersion from mean
    // (it normals are in the same direction accross shapes) or
    // bad (there is discrepency in the normal directions across shapes)
    this->goodPoints.resize(localPoints[0].size(), true);
    if(pairwiseNormalsDiffForGoodBad)
    {
      for (int i = 0; i < localPoints[0].size(); i++)
      {
        // the normal of the first shape
        double nx_i = normals[0](i, 0);
        double ny_i = normals[0](i, 1);
        double nz_i = normals[0](i, 2);

        // start from the second
        for (int j = 1; j < localPoints.size(); j++)
        {
          double nx_j = normals[j](i, 0);
          double ny_j = normals[j](i, 1);
          double nz_j = normals[j](i, 2);

          this->goodPoints[i] = this->goodPoints[i] && ((nx_i * nx_j + ny_i * ny_j + nz_i * nz_j) >
                                                         std::cos(this->maxAngleDegrees * Pi / 180.));
        }
      }
    }
    else
    {
      // here use the angle to the average normal
      // spherical coordinates of normal vector per particle per shape sample to compute average normals
      std::vector<std::vector<double>> thetas; thetas.clear();
      std::vector<std::vector<double>> phis; phis.clear();

      thetas.resize(sparseMeanPoint.size());
      phis.resize(sparseMeanPoint.size());
      for (int i = 0; i < sparseMeanPoint.size(); i++)
      {
        thetas[i].resize(localPoints.size());
        phis[i].resize(localPoints.size());
      }

      for (int i = 0; i < localPoints.size(); i++)
      {
        for (int j = 0; j < sparseMeanPoint.size(); j++)
        {
          double curNormal[3];
          double curNormalSph[3];

          curNormal[0] = normals[i](j, 0);
          curNormal[1] = normals[i](j, 1);
          curNormal[2] = normals[i](j, 2);

          Utils::cartesian2spherical(curNormal, curNormalSph);
          phis[j][i]   = curNormalSph[1];
          thetas[j][i] = curNormalSph[2];
        }
      }

      // compute mean normal for every particle
      Eigen::MatrixXd averageNormals(sparseMeanPoint.size(), 3);
      for (int i = 0; i < sparseMeanPoint.size(); i++)
      {
        double avgNormal_sph[3];
        double avgNormal_cart[3];
        avgNormal_sph[0] = 1;
        avgNormal_sph[1] = Utils::averageThetaArc(phis[i]);
        avgNormal_sph[2] = Utils::averageThetaArc(thetas[i]);
        Utils::spherical2cartesian(avgNormal_sph, avgNormal_cart);

        averageNormals(i, 0) = avgNormal_cart[0];
        averageNormals(i, 1) = avgNormal_cart[1];
        averageNormals(i, 2) = avgNormal_cart[2];
      }

      for (int i = 0; i < sparseMeanPoint.size(); i++)
      {
        double cur_cos_appex = 0;
        // the mean normal of the current particle index
        double nx_i = averageNormals(i, 0);
        double ny_i = averageNormals(i, 1);
        double nz_i = averageNormals(i, 2);

        for (int j = 0; j < localPoints.size(); j++)
        {
          double nx_j = normals[j](i, 0);
          double ny_j = normals[j](i, 1);
          double nz_j = normals[j](i, 2);

          cur_cos_appex += (nx_i * nx_j + ny_i * ny_j + nz_i * nz_j);
        }

        cur_cos_appex /= localPoints.size();
        cur_cos_appex *= 2.0; // due to symmetry about the mean normal

        this->goodPoints[i] = cur_cos_appex > std::cos(this->maxAngleDegrees * Pi / 180.);
      }
    }

    // decide which correspondences will be used to build the warp
    std::vector<int> particlesIndices; particlesIndices.clear();
    for (int i = 0; i < this->goodPoints.size(); i++)
    {
      if (this->goodPoints[i])
        particlesIndices.push_back(i);
    }
    std::cout << "There are " << particlesIndices.size() << " / " << this->goodPoints.size() << " good points." << std::endl;

    Image dt(distanceTransform[0]);
    Image meanDistanceTransform(dt);
    Image meanDistanceTransformBeforeWarp(dt);

    typename PointSetType::Pointer sourceLandMarks = PointSetType::New();
    typename PointSetType::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
    Point3 ps;
    PointIdType id = itk::NumericTraits<PointIdType>::Zero;
    int ns = 0;
    for (unsigned int i = 0; i < localPoints[0].size(); i++)
    {
      if (std::find(particlesIndices.begin(), particlesIndices.end(), i) != particlesIndices.end())
      {
        double p[3];
        this->sparseMean->GetPoint(i, p);
        ps[0] = p[0];
        ps[1] = p[1];
        ps[2] = p[2];
        sourceLandMarkContainer->InsertElement(id++, ps);
        ns++;
      }
    }

    double sigma = computeAverageDistanceToNeighbors(this->sparseMean, particlesIndices);

    typename TransformType::Pointer transform = TransformType::New();
    transform->SetSigma(sigma); // smaller means more sparse
    transform->SetStiffness(1e-10);
    transform->SetSourceLandmarks(sourceLandMarks);

    std::vector<int> centroidIndices;
    if (this->numOfClusters > 0 && this->numOfClusters < worldPoints.size())
      this->performKMeansClustering(worldPoints, worldPoints[0].size(), centroidIndices);
    else
    {
      this->numOfClusters = distanceTransform.size();
      centroidIndices.resize(distanceTransform.size());
      for (int shapeNo = 0; shapeNo < distanceTransform.size(); shapeNo++)
        centroidIndices[shapeNo] = int(shapeNo);
    }

    for (int cnt = 0; cnt < centroidIndices.size(); cnt++)
    {
      int shape = centroidIndices[cnt];
      Image dt(distanceTransform[shape]);
      typename PointSetType::Pointer targetLandMarks = PointSetType::New();
      Point3 pt;
      typename PointSetType::PointsContainer::Pointer targetLandMarkContainer = targetLandMarks->GetPoints();
      id = itk::NumericTraits<PointIdType>::Zero;

      int nt = 0;
      for (unsigned int i = 0; i < localPoints[0].size(); i++)
      {
        if (std::find(particlesIndices.begin(), particlesIndices.end(), i) != particlesIndices.end())
        {
          double p[3];
          subjectPoints[shape]->GetPoint(i, p);
          pt[0] = p[0];
          pt[1] = p[1];
          pt[2] = p[2];
          targetLandMarkContainer->InsertElement(id++, pt);
          nt++;
        }
      }
      transform->SetTargetLandmarks(targetLandMarks);

      if (cnt == 0)
      {
        meanDistanceTransformBeforeWarp = Image(dt);
        meanDistanceTransform = dt.resample(transform, dt.origin(), dt.dims(), dt.spacing(), dt.coordsys());
      }
      else
      {
        meanDistanceTransform += dt.resample(transform, dt.origin(), dt.dims(), dt.spacing(), dt.coordsys());
        if (this->meanBeforeWarp)
          meanDistanceTransformBeforeWarp += dt;
      }
    }

    Image multiplyImage = meanDistanceTransform * (1.0 / this->numOfClusters);
    Image multiplyImageBeforeWarp = meanDistanceTransformBeforeWarp * (1.0 / this->numOfClusters);

    if (this->enableOutput)
    {
      std::string meanDT_filename = this->outPrefix + "/" + "_meanDT.nrrd";
      multiplyImage.write(meanDT_filename);
      if (this->meanBeforeWarp)
      {
        std::string meanDTBeforeWarp_filename = this->outPrefix + "/" + "_meanDT_beforeWarp.nrrd";
        multiplyImageBeforeWarp.write(meanDTBeforeWarp_filename);
      }
    }

    Mesh mesh(multiplyImage.toMesh(0.0f));
    mesh.smooth(2);
    this->denseMean = mesh.getVTKMesh();
  }
  catch (std::runtime_error e)
  {
    if (this->denseMean != NULL)
    {
      this->denseDone = true;
      throw std::runtime_error("Warning! MeshQC failed, but a dense mean was computed by VTK.");
    }
  }
  catch (itk::ExceptionObject& excep)
  {
    throw std::runtime_error(excep.what());
  }
  catch (...)
  {
    throw std::runtime_error("Reconstruction failed!");
  }
  this->denseDone = true;
}

template<class TransformType>
std::vector<std::vector<Point3>> ReconstructSurface<TransformType>::computeSparseMean(std::vector<std::vector<Point3>> localPoints, Point3 commonCenter)
{
  // run generalized procrustes on the local points to align all shapes to a common coordinate frame
  Procrustes3D::ShapeListType shapelist;
  Procrustes3D::ShapeType shapevector;
  Procrustes3D::ShapeType meanSparseShape;
  Procrustes3D::PointType point;

  // fill the shape list
  for (int shapeNo = 0; shapeNo < localPoints.size(); shapeNo++)
  {
    shapevector.clear();
    std::vector<Point3> curShape = localPoints[shapeNo];
    for(int i = 0 ; i < curShape.size(); i++)
    {
      Point3 p = curShape[i];

      point(0) = double(p[0]);
      point(1) = double(p[1]);
      point(2) = double(p[2]);

      shapevector.push_back(point);
    }
    shapelist.push_back(shapevector);
  }

  Procrustes3D procrustes;
  Procrustes3D::PointType commonCenter_;
  Procrustes3D::SimilarityTransformListType transforms;

  if(this->doProcrustesScaling)
    procrustes.ScalingOn();
  else
    procrustes.ScalingOff();

  if(this->doProcrustes)
    procrustes.AlignShapes(transforms, shapelist); // shapes are actually aligned (modified) and transforms are returned
  else
    procrustes.RemoveTranslation(transforms, shapelist); // remove translation to compute the common center, shapes are actually aligned (modified) and transforms are returned

  // this is the center which needed for translation of the shapes to coincide on the image origin
  // so that the whole object is in the image and won't go outside
  procrustes.ComputeCommonCenter(transforms, commonCenter_);

  commonCenter[0] = commonCenter_[0];
  commonCenter[1] = commonCenter_[1];
  commonCenter[2] = commonCenter_[2];

  // compute the average sparse shape
  procrustes.ComputeMeanShape(meanSparseShape, shapelist);
  int medianShapeIndex = procrustes.ComputeMedianShape(shapelist);

  this->sparseMean = Mesh::MeshPoints::New(); // for visualization and estimating kernel support
  Point3 center({0,0,0});
  for(unsigned int i = 0; i < meanSparseShape.size(); i++)
  {
    Point3 pt;

    pt[0] = meanSparseShape[i](0) - commonCenter_(0);
    pt[1] = meanSparseShape[i](1) - commonCenter_(1);
    pt[2] = meanSparseShape[i](2) - commonCenter_(2);

    center += pt;
    this->sparseMean->InsertNextPoint(pt[0], pt[1], pt[2]);
  }

  center /= meanSparseShape.size();

  std::vector<std::vector<Point3>> globalPoints; globalPoints.clear();

  // prep aligned shapes for subsequent statistical analysis
  for (int shapeNo = 0; shapeNo < localPoints.size(); shapeNo++)
  {
    shapevector = shapelist[shapeNo];
    std::vector<Point3> curShape;
    curShape.clear();
    for(int i = 0 ; i < shapevector.size(); i++)
    {
      Point3 pt;

      pt[0] = shapevector[i](0) - commonCenter_(0);
      pt[1] = shapevector[i](1) - commonCenter_(1);
      pt[2] = shapevector[i](2) - commonCenter_(2);

      curShape.push_back(pt);
    }

    globalPoints.push_back(curShape);
  }

  return globalPoints;
}

template<class TransformType>
std::vector<std::vector<Point3>> ReconstructSurface<TransformType>::setLocalPointsFiles(const std::vector<std::string> localPointsFiles)
{
  std::vector<std::vector<Point3>> localPoints;
  this->localPointsFiles = localPointsFiles;

  for (int i = 0; i < this->localPointsFiles.size(); i++)
  {
    std::vector<Point3> curShape;
    Utils::readSparseShape(curShape, const_cast<char*>(this->localPointsFiles[i].c_str()));

    localPoints.push_back(curShape);
  }

  return localPoints;
}

template<class TransformType>
std::vector<std::vector<Point3>> ReconstructSurface<TransformType>::setWorldPointsFiles(const std::vector<std::string> worldPointsFiles)
{
  std::vector<std::vector<Point3>> worldPoints;
  this->worldPointsFiles = worldPointsFiles;

  for (int i = 0; i < this->worldPointsFiles.size(); i++)
  {
    std::vector<Point3> curShape;
    Utils::readSparseShape(curShape, const_cast<char*>(this->worldPointsFiles[i].c_str()));

    worldPoints.push_back(curShape);
  }

  return worldPoints;
}

template<class TransformType>
void ReconstructSurface<TransformType>::surface(const std::vector<std::string> localPointsFiles)
{
  this->localPointsFiles = localPointsFiles;

  for (unsigned int i = 0; i < this->localPointsFiles.size(); i++)
  {
    std::string basename = StringUtils::getFilename(this->localPointsFiles[i]);
    std::cout << "Processing: " << this->localPointsFiles[i].c_str() << std::endl;

    std::vector<Point3> curSparse;
    Utils::readSparseShape(curSparse, const_cast<char*> (this->localPointsFiles[i].c_str()));

    Mesh curDense = getMesh(curSparse);

    std::string outfilename = this->outPrefix + '/' + StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[i])) + "_dense.vtk";
    std::cout << "Writing: " << outfilename << std::endl;
    curDense.write(outfilename);

    std::string ptsfilename = this->outPrefix + '/'+ StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[i])) + "_dense.particles";
    Utils::writeSparseShape((char*) ptsfilename.c_str(), curDense.getVTKMesh()->GetPoints());
    vtkSmartPointer<vtkPoints> curSparse_ = vtkSmartPointer<vtkPoints>::New();
    Utils::readSparseShape(curSparse_, const_cast<char*> (this->localPointsFiles[i].c_str()));

    ptsfilename = this->outPrefix + '/'+ StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[i])) + "_sparse.particles";
    Utils::writeSparseShape((char*) ptsfilename.c_str(), curSparse_);
  }
}

template<class TransformType>
void ReconstructSurface<TransformType>::samplesAlongPCAModes(const std::vector<std::string> worldPointsFiles)
{
  int domainsPerShape = 1;
  const int Dimension = 3;
  ParticleShapeStatistics shapeStats;

  std::vector<std::vector<Point3>> worldPoints = setWorldPointsFiles(worldPointsFiles);

  // perform PCA on the global points that were used to compute the dense mean mesh
  shapeStats.DoPCA(worldPoints, domainsPerShape);

  std::vector<double> percentVarByMode = shapeStats.PercentVarByMode();
  int totalNumberOfModes = percentVarByMode.size();

  int numberOfModes = 0;
  bool singleModeToBeGen = false;
  if ((this->modeIndex >= 0) && (this->modeIndex < totalNumberOfModes))
  {
    numberOfModes = 1; singleModeToBeGen = true;
    std::cout << "Mode #" << this->modeIndex << " is requested to be generated  ..." << std::endl;
  }
  else
  {
    if (this->numOfModes > 0)
    {
      numberOfModes = std::min(this->numOfModes, totalNumberOfModes);
      std::cout << numberOfModes << " dominant modes are requested to be generated  ..." << std::endl;
    }
    else
    {
      // detect number of modes
      bool found = false;
      for (int n = totalNumberOfModes-1; n >=0; n--)
      {
        if (percentVarByMode[n] >= this->maxVarianceCaptured && found==false)
        {
          numberOfModes = n;
          found = true;
        }
      }

      if(!found)
        numberOfModes = percentVarByMode.size();

      if (numberOfModes == 0)
      {
        std::invalid_argument("No dominant modes detected");
      }
    }
    std::cout << numberOfModes << " dominant modes is found to capture " << this->maxVarianceCaptured*100 << "% of total variation ..." << std::endl;
  }

  // start sampling along each mode
  std::vector<double> eigenValues = shapeStats.Eigenvalues();
  Eigen::MatrixXd eigenVectors = shapeStats.Eigenvectors();
  Eigen::VectorXd mean = shapeStats.Mean();

  Mesh::MeshPoints meanPoints = Mesh::MeshPoints::New();
  for(unsigned int i = 0; i < this->numOfParticles; i++)
  {
    double pt[3];
    for (unsigned int d = 0; d < Dimension; d++)
      pt[d] = mean(i*Dimension + d);

    meanPoints->InsertNextPoint(pt);
  }

  std::string prefix = shapeworks::StringUtils::getFilename(this->outPrefix);
  if(!prefix.empty())
    prefix = prefix + "_";

  for (int modeId = 0; modeId < totalNumberOfModes; modeId++)
  {
    if (singleModeToBeGen && (modeId != this->modeIndex))
      continue;

    if ((!singleModeToBeGen) && (modeId >= numberOfModes))
      break;

    std::string modeStr  = Utils::int2str(modeId, 2);
    std::string cur_path = this->outPath + "/mode-" + modeStr;
    int mkdirStatus;
#ifdef WIN32
    mkdirStatus = _mkdir(cur_path.c_str());
#else
    mkdirStatus = mkdir(cur_path.c_str(), S_IRWXU);
#endif

    double sqrt_eigenValue = sqrt(eigenValues[totalNumberOfModes - modeId - 1]);
    double min_std_factor = -1 * this->maxStdDev;
    double max_std_factor = +1 * this->maxStdDev;
    std::vector<double> std_factor_store = Utils::linspace(min_std_factor, max_std_factor, this->numOfSamplesPerMode);

    Eigen::VectorXd curMode = eigenVectors.col(totalNumberOfModes - modeId - 1);

    std::vector<double> std_store;
    std::cout << "std_store: ";
    for (unsigned int sid = 0; sid < std_factor_store.size(); sid++)
    {
      std_store.push_back(std_factor_store[sid]*sqrt_eigenValue);
      std::cout << std_store[sid] << ", " ;
    }
    std::cout << std::endl;

    // writing stds on file
    std::string stdfilename = cur_path + "/" + prefix + "mode-" + modeStr + "_stds.txt";
    std::ofstream ofs(stdfilename.c_str());

    if (!ofs)
      throw std::runtime_error("Could not open file for output: " + stdfilename);

    for (unsigned int sid = 0; sid < std_factor_store.size(); sid++)
      ofs << std_factor_store[sid] << "\n" ;
    ofs.close();

    for(unsigned int sampleId = 0; sampleId < std_store.size(); sampleId++)
    {
      std::string sampleStr = Utils::int2str(int(sampleId), 3);
      std::string basename = prefix + "mode-" + modeStr + "_sample-" + sampleStr ;

      std::cout << "generating mode #" + Utils::num2str((float)modeId) + ", sample #" + Utils::num2str((float)sampleId) << std::endl;

      // generate the sparse shape of the current sample
      double cur_std = std_store[sampleId];

      std::cout << "cur_std: " << cur_std << std::endl;

      Eigen::VectorXd curSample = mean + cur_std * curMode;

      // fill-in the vtkpoints structure to perform warping for dense reconstruction of the current sample
      vtkSmartPointer< vtkPoints > curSamplePts = vtkSmartPointer< vtkPoints >::New();
      std::vector<Point3> curSparse;
      for(unsigned int i = 0; i < this->numOfParticles; i++)
      {
        double pt[3];
        for (unsigned int d = 0 ; d < Dimension; d++)
            pt[d] = curSample(i*Dimension + d);

        Point3 p({pt[0], pt[1], pt[2]});

        curSamplePts->InsertNextPoint(pt);
        curSparse.push_back(p);
      }

      Mesh curDense = getMesh(curSparse);

      std::string outfilename = cur_path + "/" + basename + "_dense.vtk";
      std::cout << "Writing: " << outfilename << std::endl;
      curDense.write(outfilename);

      vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
      vertices->DeepCopy(curDense.getVTKMesh()->GetPoints());

      std::string ptsfilename = cur_path + "/" + basename + "_dense.particles";
      Utils::writeSparseShape((char*) ptsfilename.c_str(), vertices);

      ptsfilename = cur_path + "/" + basename + "_sparse.particles";
      Utils::writeSparseShape((char*) ptsfilename.c_str(), curSamplePts);
    }
  }
}

template<class TransformType>
void ReconstructSurface<TransformType>::meanSurface(const std::vector<std::string> distanceTransformFiles, const std::vector<std::string> localPointsFiles, const std::vector<std::string> worldPointsFiles)
{
  this->maxAngleDegrees = this->normalAngle * (180.0 / Pi);
  std::vector<std::vector<Point3>> worldPoints;
  this->distanceTransformFiles = distanceTransformFiles;
  std::vector<std::vector<Point3>> localPoints = setLocalPointsFiles(localPointsFiles);

  if(worldPointsFiles.size() == 0)
  {
    // if no world points are given, they will be estimated using procrustes
    // define mean sparse shape -- this is considered as target points in the warp
    std::cout << "Computing mean sparse shape .... \n ";

    Point3 commonCenter;
    worldPoints = computeSparseMean(localPoints, commonCenter);
  }

  else
  {
    worldPoints = setWorldPointsFiles(worldPointsFiles);

    // finding image origin that is consistent with the given world coordinates
    // and adjusted using the origin of images and point clouds in the local space
    Point3 originLocal, originWorld;
    Point3 minLocal, maxLocal;
    Point3 minWorld, maxWorld; // bounding box of the global points

    // compute center of mass for both local and global points
    Utils::computeCenterOfMassForShapeEnsemble(localPoints,  originLocal);
    Utils::computeCenterOfMassForShapeEnsemble(worldPoints, originWorld);

    // find the bounding box of both local and global points
    Utils::getBoundingBoxForShapeEnsemble(localPoints,
                                          minLocal[0], minLocal[1], minLocal[2],
                                          maxLocal[0], maxLocal[1], maxLocal[2]);
    Utils::getBoundingBoxForShapeEnsemble(worldPoints,
                                          minWorld[0], minWorld[1], minWorld[2],
                                          maxWorld[0], maxWorld[1], maxWorld[2]);

    // compute image origin (corner) based on center of mass
    Point3 widthLocal = maxLocal - minLocal;
    Point3 widthWorld = maxWorld - minWorld;
    originLocal -= widthLocal/2.0;
    originWorld -= widthWorld/2.0;

    Image dt(this->distanceTransformFiles[0]);
    Point3 origin_dt = dt.origin();
    Point3 offset = origin_dt - originLocal;

    // adjust global origin based on local offset
    originWorld += offset;

    dt.setOrigin(originWorld);
  }

  // write global points to be used for pca modes and also local points
  int mkdirStatus;
  std::string worldPointsPath = this->outPath + "/global_particles";
  std::string localPointsPath = this->outPath + "/local_particles";

#ifdef WIN32
  mkdirStatus = _mkdir(worldPointsPath.c_str());
  mkdirStatus = _mkdir(localPointsPath.c_str());
#else
  mkdirStatus = mkdir(worldPointsPath.c_str(), S_IRWXU); // check on this
  mkdirStatus = mkdir(localPointsPath.c_str(), S_IRWXU); // check on this
#endif

  for (int i = 0; i < this->worldPointsFiles.size(); i++)
  {
    std::string curfilename = worldPointsPath + "/" + StringUtils::removeExtension(StringUtils::getFilename(localPointsFiles[i])) + "_global.particles";
    Utils::writeSparseShape((char*)curfilename.c_str(), worldPoints[i]);

    std::string curfilename_local = localPointsPath + "/" + StringUtils::removeExtension(StringUtils::getFilename(localPointsFiles[i])) + "_local.particles";
    Utils::writeSparseShape((char*)curfilename_local.c_str(), localPoints[i]);
  }

  vtkSmartPointer<vtkPolyData> denseMean = getDenseMean(localPoints, worldPoints, this->distanceTransformFiles);

  // write output
  writeMeanInfo();

  // write out good and bad particles for each subject file
  std::string localGoodBadPath = this->outPath + "/local_good_bad";
  std::string worldGoodBadPath = this->outPath + "/global_good_bad";
#ifdef WIN32
  mkdirStatus = _mkdir(localGoodBadPath.c_str());
  mkdirStatus = _mkdir(worldGoodBadPath.c_str());
#else
  mkdirStatus = mkdir(localGoodBadPath.c_str(), S_IRWXU);
  mkdirStatus = mkdir(worldGoodBadPath.c_str(), S_IRWXU);
#endif

  for (unsigned int shapeNo = 0; shapeNo < this->worldPointsFiles.size(); shapeNo++)
  {
    std::string outfilenameGood;
    std::string outfilenameBad;
    std::ofstream ofsG, ofsB;

    outfilenameGood = localGoodBadPath + "/" + shapeworks::StringUtils::removeExtension(shapeworks::StringUtils::getFilename(this->localPointsFiles[shapeNo])) + "_local-good.particles";
    outfilenameBad  = localGoodBadPath + "/" + shapeworks::StringUtils::removeExtension(shapeworks::StringUtils::getFilename(this->localPointsFiles[shapeNo])) + "_local-bad.particles";

    ofsG.open(outfilenameGood.c_str());
    ofsB.open(outfilenameBad.c_str());

    std::vector<Point3> currShapeLocal = localPoints[shapeNo];
    for (unsigned int i = 0 ; i < this->numOfParticles; i++)
    {
      auto pt = currShapeLocal[i];
      if(this->goodPoints[i])
        ofsG << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
      else
        ofsB << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
    }
    ofsG.close();
    ofsB.close();

    outfilenameGood = worldGoodBadPath + "/" + shapeworks::StringUtils::removeExtension(shapeworks::StringUtils::getFilename(this->localPointsFiles[shapeNo])) + "_global-good.particles";
    outfilenameBad  = worldGoodBadPath + "/" + shapeworks::StringUtils::removeExtension(shapeworks::StringUtils::getFilename(this->localPointsFiles[shapeNo])) + "_global-bad.particles";

    ofsG.open(outfilenameGood.c_str());
    ofsB.open(outfilenameBad.c_str());

    std::vector<Point3> currShapeWorld = worldPoints[shapeNo];
    for (unsigned int i = 0; i < this->numOfParticles; i++)
    {
      auto pt = currShapeWorld[i];
      if(this->goodPoints[i])
        ofsG << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
      else
        ofsB << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
    }
    ofsG.close();
    ofsB.close();
  }
}

template class ReconstructSurface<ThinPlateSplineTransform>;
template class ReconstructSurface<RBFSSparseTransform>;

} // shapeworks
