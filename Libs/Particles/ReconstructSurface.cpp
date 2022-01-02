#include "ReconstructSurface.h"
#include "StringUtils.h" 
#include "ParticleShapeStatistics.h"
#include "Utils.h"

#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkKdTreePointLocator.h>

namespace shapeworks {

// ReconstructSurface::ReconstructSurface(TransformType transform, InterpType interp)
// {
//   if (transform == ThinPlateSplineTransform)
//   {
//     this->ifTransform1 = true;
//   }
//   else if (transform == RBFSSparseTransform)
//   {
//     this->ifTransform2 = true;
//   }
//   else
//     std::invalid_argument("Incorrect transform type");
// }

vtkSmartPointer<vtkPoints> ReconstructSurface::setSparseMean(const std::string& sparsePath)
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

std::vector<bool> ReconstructSurface::setGoodPoints(const std::string& pointsPath)
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

double ReconstructSurface::computeAverageDistanceToNeighbors(Mesh::MeshPoints points, std::vector<int> particlesIndices)
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

template<class T>
void ReconstructSurface::CheckMapping(Mesh::MeshPoints sourcePoints, Mesh::MeshPoints targetPoints, typename T::Pointer transform, Mesh::MeshPoints& mappedCorrespondences,
                                      double& rms, double& rms_wo_mapping, double& maxmDist)
{
  // source should be warped to the target
  rms = 0.0;
  rms_wo_mapping = 0.0;

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

  maxmDist = double(-10000.0f);
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

template<class T>
void ReconstructSurface::generateWarpedMeshes(typename T::Pointer transform, vtkSmartPointer<vtkPolyData>& outputMesh)
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

template<class T>
Mesh ReconstructSurface::getMesh(typename T::Pointer transform, std::vector<Point3> localPoints)
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
  PointSetType::Pointer sourceLandMarks = PointSetType::New();
  PointSetType::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
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

  id = itk::NumericTraits<PointIdType>::Zero;
  PointSetType::Pointer targetLandMarks = PointSetType::New();
  PointSetType::PointsContainer::Pointer targetLandMarkContainer = targetLandMarks->GetPoints();
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

  vtkSmartPointer<vtkPoints> mappedCorrespondences = vtkSmartPointer<vtkPoints>::New();
  double rms, rms_wo_mapping, maxmDist;
  CheckMapping(this->sparseMean, subjectPoints, transform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);

  vtkSmartPointer<vtkPolyData> denseShape = vtkSmartPointer<vtkPolyData>::New();
  denseShape->DeepCopy(this->denseMean.getVTKMesh());
  generateWarpedMeshes(transform, denseShape);
  return Mesh(denseShape);
}

void ReconstructSurface::setDistanceTransformFiles(const std::vector<std::string> dtFiles)
{
  this->distanceTransformFiles = dtFiles;
}

std::vector<std::vector<Point3>> ReconstructSurface::setLocalPointsFiles(const std::vector<std::string> localPointsFiles)
{
  std::vector<std::vector<Point3>> localPoints;
  this->localPointsFiles = localPointsFiles;

  for (int i=0; i<this->localPointsFiles.size(); i++)
  {
    std::vector<Point3> curShape;
    Utils::readSparseShape(curShape, const_cast<char*>(this->localPointsFiles[i].c_str()));

    localPoints.push_back(curShape);
  }

  return localPoints;
}

std::vector<std::vector<Point3>> ReconstructSurface::setWorldPointsFiles(const std::vector<std::string> worldPointsFiles)
{
  std::vector<std::vector<Point3>> worldPoints;
  this->worldPointsFiles = worldPointsFiles;

  for (int i=0; i<this->worldPointsFiles.size(); i++)
  {
    std::vector<Point3> curShape;
    Utils::readSparseShape(curShape, const_cast<char*>(this->worldPointsFiles[i].c_str()));

    worldPoints.push_back(curShape);
  }

  return worldPoints;
}

void ReconstructSurface::meanSurface(const std::vector<std::string> distanceTransformFiles, const std::vector<std::string> localPointsFiles, const std::vector<std::string> worldPointsFiles)
{
  double maxAngleDegrees = normalAngle * (180.0 / Pi);
  std::vector<std::vector<Point3>> worldPoints;
  setDistanceTransformFiles(distanceTransformFiles);
  std::vector<std::vector<Point3>> localPoints = setLocalPointsFiles(localPointsFiles);

  if(this->worldPointsFiles.size() == 0)
  {
    // TODO
  }

  else
  {
    worldPoints = setWorldPointsFiles(worldPointsFiles);

    // finding image origin that is consistent with the given world coordinates and adjusted using the origin of images and point clouds in the local space
    Point3 originLocal;
    Point3 originWorld;

    Point3 minLocal;
    Point3 maxLocal;

    // the bounding box of the global points
    Point3 minWorld;
    Point3 maxWorld;

    // compute the center of mass for both local and global points
    Utils::computeCenterOfMassForShapeEnsemble(localPoints,  originLocal);
    Utils::computeCenterOfMassForShapeEnsemble(worldPoints, originWorld);

    // find the bounding box of both local and global points
    Utils::getBoundingBoxForShapeEnsemble(localPoints,
                                          minLocal[0], minLocal[1], minLocal[2],
                                          maxLocal[0], maxLocal[1], maxLocal[2]);
    Utils::getBoundingBoxForShapeEnsemble(worldPoints,
                                          minWorld[0], minWorld[1], minWorld[2],
                                          maxWorld[0], maxWorld[1], maxWorld[2]);

    // compute the image origin (corner) based on the center of mass
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

  // write global points to be use for pca modes and also local points
  int mkdirStatus;
  std::string worldPointsPath = this->outPath + "/global_particles";
  std::string localPointsPath = this->outPath + "/local_particles";

#ifdef WIN32
  mkdirStatus = _mkdir(worldPointsPath.c_str());
  mkdirStatus = _mkdir(localPointsPath.c_str());
#else
  // mkdirStatus = mkdir(worldPointsPath.c_str(), S_IRWXU); // check on this
  // mkdirStatus = mkdir(localPointsPath.c_str(), S_IRWXU); // check on this
#endif

  for (int i=0; i<this->worldPointsFiles.size(); i++)
  {
    std::string curfilename = worldPointsPath + "/" + StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[i])) + "_global.particles";
    Utils::writeSparseShape((char*)curfilename.c_str(), worldPoints[i]);

    std::string curfilename_local = localPointsPath + "/" + StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[i])) + "_local.particles";
    Utils::writeSparseShape((char*)curfilename_local.c_str(), localPoints[i]);
  }

  vtkSmartPointer<vtkPolyData> denseMean = getDenseMean(localPoints, worldPoints, this->distanceTransformFiles);
}

Mesh::MeshPoints ReconstructSurface::convertToImageCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin)
{
  Mesh::MeshPoints points = Mesh::MeshPoints::New();
  for (unsigned int i = 0; i < numParticles; i++)
  {
    double p[3];
    particles->GetPoint(i, p);
    points->InsertNextPoint((p[0] - origin[0]) / ((double)spacing[0]),
                            (p[1] - origin[1]) / ((double)spacing[1]),
                            (p[2] - origin[2]) / ((double)spacing[2]));
  }
  return points;
}

Mesh::MeshPoints ReconstructSurface::convertToPhysicalCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin)
{
  Mesh::MeshPoints points = Mesh::MeshPoints::New();
  for (unsigned int i = 0; i < numParticles; i++)
  {
    double p[3];
    particles->GetPoint(i, p);
    points->InsertNextPoint(p[0] * spacing[0] + origin[0],
                            p[1] * spacing[1] + origin[1],
                            p[2] * spacing[2] + origin[2]);
  }
  return points;
}

Eigen::MatrixXd ReconstructSurface::computeParticlesNormals(vtkSmartPointer<vtkPoints> particles, Image dt)
{
  Vector spacing = dt.spacing();
  Point3 origin = dt.origin();

  Image gradientImage = dt.applyGradientFilter();
  Image gradientMagImage = dt.applyGradientMagFilter();

  Image::CovariantImageIterator gradIter = gradientImage.setIterator();
  Image::ImageIterator magIter = gradientMagImage.setIterator();

  Image nxImage(dt);
  Image::ImageIterator nxIter = nxImage.setIterator();
  Image nyImage(dt);
  Image::ImageIterator nyIter = nyImage.setIterator();
  Image nzImage(dt);
  Image::ImageIterator nzIter = nzImage.setIterator();

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

  // going to vtk for probing ...
  Image Nx(nxImage.getVTKImage());
  Image Ny(nyImage.getVTKImage());
  Image Nz(nzImage.getVTKImage());

  vtkSmartPointer<vtkPoints> pts = convertToImageCoordinates(particles, particles->GetNumberOfPoints(), spacing, origin);
  vtkSmartPointer<vtkPolyData> polyParticles = vtkSmartPointer<vtkPolyData>::New();
  polyParticles->SetPoints(pts);

  Mesh particlesData(polyParticles);
  Mesh probeX = particlesData.probeVolume(Nx);
  Mesh probeY = particlesData.probeVolume(Ny);
  Mesh probeZ = particlesData.probeVolume(Nz);

  vtkFloatArray* nx = vtkFloatArray::SafeDownCast(probeX.getVTKMesh()->GetPointData()->GetScalars());
  vtkFloatArray* ny = vtkFloatArray::SafeDownCast(probeY.getVTKMesh()->GetPointData()->GetScalars());
  vtkFloatArray* nz = vtkFloatArray::SafeDownCast(probeZ.getVTKMesh()->GetPointData()->GetScalars());

  vtkSmartPointer<vtkDoubleArray> pointNormalsArray = vtkSmartPointer<vtkDoubleArray>::New();
  pointNormalsArray->SetNumberOfComponents(3);
  pointNormalsArray->SetNumberOfTuples(particlesData.numPoints());

  Eigen::MatrixXd particlesNormals(particles->GetNumberOfPoints(), 3);
  for (int i=0; i<particlesData.numPoints(); i++)
  {
    Point3 pN({nx->GetValue(i), ny->GetValue(i), nz->GetValue(i)});

    double norm = sqrt(pN[0] * pN[0] + pN[1] * pN[1] + pN[2] * pN[2]);
    pN /= norm;

    particlesNormals(i, 0) = pN[0];
    particlesNormals(i, 1) = pN[1];
    particlesNormals(i, 2) = pN[2];

    pointNormalsArray->SetTuple(i, pN);
  }

  particlesData->GetPointData()->SetNormals(pointNormalsArray);

  vtkSmartPointer<vtkPoints> pts2 = convertToPhysicalCoordinates(polyParticles->GetPoints(), particlesData->GetPoints()->GetNumberOfPoints(),
                                                                 spacing, origin);
  polyParticles->SetPoints(pts2);

  return particlesNormals;
}

// TODO : FINISH CONSOLIDATING THIS
vtkSmartPointer<vtkPolyData> ReconstructSurface::getDenseMean(std::vector<std::vector<Point>> localPts, std::vector<std::vector<Point>> worldPts, std::vector<std::string> distance_transform)
{
  // if (!this->denseDone || !local_pts.empty() || !distance_transform.empty() || !global_pts.empty()) 
  // {
  //   this->denseDone = false;
  //   if (local_pts.empty() || distance_transform.empty() || global_pts.empty() || local_pts.size() != distance_transform.size())
  //   {
  //     throw std::runtime_error("Invalid input for reconstruction!");
  //   }
  //   this->computeDenseMean(local_pts, global_pts, distance_transform);
  // }

  // return this->denseMean_;
}

void ReconstructSurface::computeDenseMean(std::vector<std::vector<Point>> localPts, std::vector<std::vector<Point>> worldPts, std::vector<std::string> distanceTransform)
{
  try {

    // turn the sets of global points to one sparse global mean.
    Point init({0.f, 0.f, 0.f});
    std::vector<Point> sparseMeanPoint = std::vector<Point>(worldPts[0]);
    for (auto &a : worldPts)
    {
      for (size_t i = 0; i < a.size(); i++)
      {
        init[0] = a[i][0]; init[1] = a[i][1]; init[2] = a[i][2];
        Vector3 vec = toVector(init);
        sparseMeanPoint[i] = sparseMeanPoint[i] + vec;
      }
    }

    auto div = static_cast<float>(worldPts.size());
    for (size_t i = 0; i < sparseMeanPoint.size(); i++)
    {
      init[0] = sparseMeanPoint[i][0] / div;
      init[1] = sparseMeanPoint[i][1] / div;
      init[2] = sparseMeanPoint[i][2] / div;
      sparseMeanPoint[i] = itk::Point<float>(init);
    }

    std::vector<Eigen::MatrixXd> normals;
    std::vector<vtkSmartPointer< vtkPoints > > subjectPts;
    this->sparseMean = vtkSmartPointer<vtkPoints>::New();
    for (auto &a : sparseMeanPoint)
    {
      this->sparseMean->InsertNextPoint(a[0], a[1], a[2]);
    }
    for (size_t shape = 0; shape < localPts.size(); shape++)
    {
      subjectPts.push_back(vtkSmartPointer<vtkPoints>::New());
      for (auto &a : localPts[shape])
      {
        subjectPts[shape]->InsertNextPoint(a[0], a[1], a[2]);
      }
      //calculate the normals from the DT
      normals.push_back(this->computeParticlesNormals(subjectPts[shape], Image(distanceTransform[shape])));
    }

      // now decide whether each particle is a good based on dispersion from mean
      // (it normals are in the same direction accross shapes) or
      // bad (there is discrepency in the normal directions across shapes)
      // this->goodPoints_.resize(local_pts[0].size(), true);
      // if(usePairwiseNormalsDifferencesForGoodBad_){
      //     for (unsigned int ii = 0; ii < local_pts[0].size(); ii++) {
      //         bool isGood = true;

      //         // the normal of the first shape
      //         double nx_jj = normals[0](ii, 0);
      //         double ny_jj = normals[0](ii, 1);
      //         double nz_jj = normals[0](ii, 2);

      //         // start from the second
      //         for (unsigned int shapeNo_kk = 1; shapeNo_kk <
      //               local_pts.size(); shapeNo_kk++) {
      //             double nx_kk = normals[shapeNo_kk](ii, 0);
      //             double ny_kk = normals[shapeNo_kk](ii, 1);
      //             double nz_kk = normals[shapeNo_kk](ii, 2);

      //             this->goodPoints_[ii] = this->goodPoints_[ii] &&
      //                     ((nx_jj*nx_kk + ny_jj*ny_kk + nz_jj*nz_kk) >
      //                       std::cos(this->maxAngleDegrees_ * M_PI / 180.));
      //         }
      //     }
      // }
      // else {
      //     // here use the angle to the average normal
      //     // spherical coordinates of normal vector per particle per shape sample to compute average normals
      //     std::vector< std::vector< double > > thetas ; thetas.clear();
      //     std::vector< std::vector< double > > phis;    phis.clear();

      //     thetas.resize(sparseMean.size());
      //     phis.resize(sparseMean.size());
      //     for (size_t j = 0; j < sparseMean.size(); j++) {
      //         thetas[j].resize(local_pts.size());
      //         phis[j].resize(local_pts.size());
      //     }
      //     for (int i = 0; i < local_pts.size(); i++){
      //         for (size_t j = 0; j < sparseMean.size(); j++) {
      //             double curNormal[3];
      //             double curNormalSph[3];

      //             curNormal[0] = normals[i](j,0);
      //             curNormal[1] = normals[i](j,1);
      //             curNormal[2] = normals[i](j,2);

      //             Utils::cartesian2spherical(curNormal, curNormalSph);
      //             phis[j][i]   = curNormalSph[1];
      //             thetas[j][i] = curNormalSph[2];
      //         }
      //     }

      //     // compute mean normal for every particle
      //     vnl_matrix<double> average_normals(sparseMean.size(), 3);
      //     for (size_t j = 0; j < sparseMean.size(); j++) {
      //         double avgNormal_sph[3];
      //         double avgNormal_cart[3];
      //         avgNormal_sph[0] = 1;
      //         avgNormal_sph[1] = Utils::averageThetaArc(phis[j]);
      //         avgNormal_sph[2] = Utils::averageThetaArc(thetas[j]);
      //         Utils::spherical2cartesian(avgNormal_sph, avgNormal_cart);

      //         average_normals(j,0) = avgNormal_cart[0];
      //         average_normals(j,1) = avgNormal_cart[1];
      //         average_normals(j,2) = avgNormal_cart[2];
      //     }

      //     for (size_t j = 0; j < sparseMean.size(); j++) {

      //         double cur_cos_appex = 0;
      //         // the mean normal of the current particle index
      //         double nx_jj = average_normals(j,0);
      //         double ny_jj = average_normals(j,1);
      //         double nz_jj = average_normals(j,2);

      //         for (unsigned int shapeNo_kk = 0; shapeNo_kk < local_pts.size(); shapeNo_kk++) {
      //             double nx_kk = normals[shapeNo_kk](j, 0);
      //             double ny_kk = normals[shapeNo_kk](j, 1);
      //             double nz_kk = normals[shapeNo_kk](j, 2);

      //             cur_cos_appex += (nx_jj*nx_kk + ny_jj*ny_kk + nz_jj*nz_kk);
      //         }

      //         cur_cos_appex /= local_pts.size();
      //         cur_cos_appex *= 2.0; // due to symmetry about the mean normal

      //         this->goodPoints_[j] = cur_cos_appex > std::cos(this->maxAngleDegrees_ * M_PI / 180.);
      //     }
      // }

      // decide which correspondences will be used to build the warp
      // std::vector<int> particles_indices;
      // particles_indices.clear();
      // for (unsigned int kk = 0; kk < this->goodPoints_.size(); kk++) {
      //     if (this->goodPoints_[kk]) {
      //         particles_indices.push_back(int(kk));
      //     }
      // }
      // std::cout << "There are " << particles_indices.size() << " / " << this->goodPoints_.size() <<
      //               " good points." << std::endl;

      // The parameters of the output image are taken from the input image.
      // NOTE: all distance transforms were generated throughout shapeworks pipeline
      // as such they have the same parameters
      // typename ImageType::Pointer image = loadImage(distance_transform[0]);
      // const typename ImageType::SpacingType& spacing = image->GetSpacing();
      // const typename ImageType::PointType& origin = image->GetOrigin();
      // const typename ImageType::DirectionType& direction = image->GetDirection();
      // typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
      // typename ImageType::RegionType region = image->GetBufferedRegion();

      // define the mean dense shape (mean distance transform)
      // typename ImageType::Pointer meanDistanceTransform = ImageType::New();
      // if(use_origin)
      //     meanDistanceTransform->SetOrigin(origin_);
      // else
      //     meanDistanceTransform->SetOrigin(origin);
      // meanDistanceTransform->SetSpacing(spacing);
      // meanDistanceTransform->SetDirection(direction);
      // meanDistanceTransform->SetLargestPossibleRegion(size);

      // typename ImageType::Pointer meanDistanceTransformBeforeWarp = ImageType::New();
      // if(use_origin)
      //     meanDistanceTransformBeforeWarp->SetOrigin(origin_);
      // else
      //     meanDistanceTransformBeforeWarp->SetOrigin(origin);
      // meanDistanceTransformBeforeWarp->SetSpacing(spacing);
      // meanDistanceTransformBeforeWarp->SetDirection(direction);
      // meanDistanceTransformBeforeWarp->SetLargestPossibleRegion(size);

      // typename AddImageFilterType::Pointer sumfilter = AddImageFilterType::New();
      // typename AddImageFilterType::Pointer sumfilterBeforeWarp = AddImageFilterType::New();

      // Define container for source landmarks that corresponds to the mean space, this is
      // fixed where the target (each individual shape) will be warped to
      // NOTE that this is inverse warping to avoid holes in the warped distance transforms
      // typename PointSetType::Pointer sourceLandMarks = PointSetType::New();
      // typename PointSetType::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
      // PointType ps;
      // PointIdType id = itk::NumericTraits< PointIdType >::Zero;
      // int ns = 0;
      // for (unsigned int ii = 0; ii < local_pts[0].size(); ii++) {
      //     if (std::find(particles_indices.begin(),
      //                   particles_indices.end(), ii) != particles_indices.end()) {
      //         double p[3];
      //         this->sparseMean_->GetPoint(ii, p);
      //         ps[0] = p[0];
      //         ps[1] = p[1];
      //         ps[2] = p[2];
      //         sourceLandMarkContainer->InsertElement(id++, ps);
      //         ns++;
      //     }
      // }

      // double sigma = computeAverageDistanceToNeighbors(
      //             this->sparseMean_, particles_indices);

      // // the roles of the source and target are reversed to simulate a reverse warping
      // // without explicitly invert the warp in order to avoid holes in the warping result
      // typename TransformType::Pointer transform = TransformType::New();
      // transform->SetSigma(sigma); // smaller means more sparse
      // //transform->SetStiffness(0.25*sigma);
      // transform->SetStiffness(1e-10);

      // transform->SetSourceLandmarks(sourceLandMarks);

      // //////////////////////////////////////////////////////////////////
      // //Praful - get the shape indices corresponding to cetroids of
      // //kmeans clusters and run the following loop on only those shapes
      // // Read input shapes from file list
      // std::vector<int> centroidIndices;
      // if (this->numClusters_ > 0 && this->numClusters_ < global_pts.size()) {
      //         this->performKMeansClustering(global_pts, global_pts[0].size(), centroidIndices);
      // } else {
      //     this->numClusters_ = distance_transform.size();
      //     centroidIndices.resize(distance_transform.size());
      //     for (size_t shapeNo = 0; shapeNo < distance_transform.size(); shapeNo++) {
      //         centroidIndices[shapeNo] = int(shapeNo);
      //         std::cout << centroidIndices[shapeNo] << std::endl;
      //     }
      // }
      // //////////////////////////////////////////////////////////////////
      // //Praful - clustering
      // for (unsigned int cnt = 0; cnt < centroidIndices.size(); cnt++) {
      //     size_t shape = size_t(centroidIndices[cnt]);
      //     typename ImageType::Pointer dt = loadImage(distance_transform[shape]);
      //     typename PointSetType::Pointer targetLandMarks = PointSetType::New();
      //     PointType pt;
      //     typename PointSetType::PointsContainer::Pointer
      //             targetLandMarkContainer = targetLandMarks->GetPoints();
      //     id = itk::NumericTraits< PointIdType >::Zero;

      //     int nt = 0;
      //     for (unsigned int ii = 0; ii < local_pts[0].size(); ii++) {
      //         if (std::find(particles_indices.begin(),
      //                       particles_indices.end(), ii) != particles_indices.end()) {
      //             double p[3];
      //             subjectPts[shape]->GetPoint(ii, p);
      //             pt[0] = p[0];
      //             pt[1] = p[1];
      //             pt[2] = p[2];
      //             targetLandMarkContainer->InsertElement(id++, pt);
      //             nt++;
      //         }
      //     }
      //     transform->SetTargetLandmarks(targetLandMarks);
      //     // check the mapping (inverse here)
      //     // this mean source points (mean space) should
      //     // be warped to the target (current sample's space)
      //     vtkSmartPointer<vtkPoints> mappedCorrespondences = vtkSmartPointer<vtkPoints>::New();
      //     double rms;
      //     double rms_wo_mapping;
      //     double maxmDist;
      //     this->CheckMapping(this->sparseMean_, subjectPts[shape], transform,
      //                         mappedCorrespondences, rms, rms_wo_mapping, maxmDist);

      //     // Set the resampler params
      //     typename ResampleFilterType::Pointer   resampler = ResampleFilterType::New();
      //     typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
      //     //interpolator->SetSplineOrder(3); // itk has a default bspline order = 3

      //     resampler->SetInterpolator(interpolator);

      //     resampler->SetOutputSpacing(spacing);
      //     resampler->SetOutputDirection(direction);
      //     if(use_origin)
      //         resampler->SetOutputOrigin(origin_);
      //     else
      //         resampler->SetOutputOrigin(origin);
      //     resampler->SetSize(size);
      //     resampler->SetTransform(transform);
      //     resampler->SetDefaultPixelValue((PixelType)-100.0);
      //     resampler->SetOutputStartIndex(region.GetIndex());
      //     resampler->SetInput(dt);
      //     resampler->Update();

      //     if (cnt == 0) {
      //         // after warp
      //         typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
      //         duplicator->SetInputImage(resampler->GetOutput());
      //         duplicator->Update();
      //         meanDistanceTransform = duplicator->GetOutput();
      //         // before warp
      //         typename DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
      //         duplicator2->SetInputImage(dt);
      //         duplicator2->Update();
      //         meanDistanceTransformBeforeWarp = duplicator2->GetOutput();
      //     } else {
      //         // after warp
      //         sumfilter->SetInput1(meanDistanceTransform);
      //         sumfilter->SetInput2(resampler->GetOutput());
      //         sumfilter->Update();

      //         typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
      //         duplicator->SetInputImage(sumfilter->GetOutput());
      //         duplicator->Update();
      //         meanDistanceTransform = duplicator->GetOutput();

      //         if (this->mean_before_warp_enabled_) {
      //           // before warp
      //           sumfilterBeforeWarp->SetInput1(meanDistanceTransformBeforeWarp);
      //           sumfilterBeforeWarp->SetInput2(dt);
      //           sumfilterBeforeWarp->Update();

      //           typename DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
      //           duplicator2->SetInputImage(sumfilterBeforeWarp->GetOutput());
      //           duplicator2->Update();
      //           meanDistanceTransformBeforeWarp = duplicator2->GetOutput();
      //         }
      //     }

      // }
      // typename MultiplyByConstantImageFilterType::Pointer multiplyImageFilter =
      //         MultiplyByConstantImageFilterType::New();
      // multiplyImageFilter->SetInput(meanDistanceTransform);
      // multiplyImageFilter->SetConstant(1.0 /
      //                                   static_cast<double>(this->numClusters_));
      // multiplyImageFilter->Update();
      // typename MultiplyByConstantImageFilterType::Pointer multiplyImageFilterBeforeWarp =
      //         MultiplyByConstantImageFilterType::New();
      // multiplyImageFilterBeforeWarp->SetInput(meanDistanceTransformBeforeWarp);
      // multiplyImageFilterBeforeWarp->SetConstant(1.0 /
      //                                             static_cast<double>(this->numClusters_));
      // multiplyImageFilterBeforeWarp->Update();

      // std::string meanDT_filename           = out_prefix_ + "_meanDT.nrrd" ;;
      // std::string meanDTBeforeWarp_filename = out_prefix_ + "_meanDT_beforeWarp.nrrd" ;;

      // if (this->output_enabled_)
      // {
      //     typename WriterType::Pointer writer = WriterType::New();
      //     writer->SetFileName( meanDT_filename.c_str());
      //     writer->SetInput( multiplyImageFilter->GetOutput() );
      //     writer->Update();

      //     if (this->mean_before_warp_enabled_) {
      //       writer->SetFileName(meanDTBeforeWarp_filename.c_str());
      //       writer->SetInput(multiplyImageFilterBeforeWarp->GetOutput());
      //       writer->Update();
      //     }
      // }

      // going to vtk to extract the template mesh (mean dense shape)
      // to be deformed for each sparse shape
      // typename ITK2VTKConnectorType::Pointer itk2vtkConnector = ITK2VTKConnectorType::New();
      // itk2vtkConnector->SetInput(multiplyImageFilter->GetOutput());
      // itk2vtkConnector->Update();
      // this->denseMean_ = this->extractIsosurface(itk2vtkConnector->GetOutput());
      // this->denseMean_ = this->MeshQC(this->denseMean_);
  }
  catch (std::runtime_error e)
  {
    if (this->denseMean_ != NULL)
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

template<class T>
void ReconstructSurface::surface(typename T::Pointer transform, std::string denseFile, std::string sparseFile, std::string goodPointsFile)
{
  double maxAngleDegrees = this->normalAngle * (180.0 / Pi);
  
  this->denseMean = Mesh(denseFile);
  this->sparseMean = setSparseMean(sparseFile);
  this->goodPoints = setGoodPoints(goodPointsFile);

  for (unsigned int i = 0; i < this->localPointsFiles.size(); i++)
  {
    std::string basename = StringUtils::getFilename(this->localPointsFiles[i]);
    std::cout << "Processing: " << this->localPointsFiles[i].c_str() << std::endl;

    std::vector<Point3> curSparse;
    Utils::readSparseShape(curSparse, const_cast<char*> (this->localPointsFiles[i].c_str()));

    Mesh curDense = getMesh(transform, curSparse);

    std::string outfilename = this->outPrefix + StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[i])) + "_dense.vtk";
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

template<class T>
void ReconstructSurface::samplesAlongPCAModes(typename T::Pointer transform)
{
  int domainsPerShape = 1;
  const unsigned int Dimension = 3;
  ParticleShapeStatistics shapeStats;

  this->denseMean = Mesh(denseFile);
  this->sparseMean = setSparseMean(sparseFile);
  this->goodPoints = setGoodPoints(goodPointsFile);

  std::vector<std::vector<Point3>> globalPoints;
  for (unsigned int i = 0; i < this->worldPointsFiles.size(); i++)
  {
    std::vector<Point3> curShape; curShape.clear();

    std::string curfilename = this->worldPointsFiles[i];
    std::cout << "curfilename: " << curfilename << std::endl;

    Utils::readSparseShape(curShape, (char*)curfilename.c_str());

    std::cout << "curShape.size() = " << curShape.size() << "-------------\n";
    globalPoints.push_back(curShape);
  }

  // perform PCA on the global points that were used to compute the dense mean mesh
  shapeStats.DoPCA(globalPoints, domainsPerShape);

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
    if (this->numberOfModes > 0)
    {
      numberOfModes = std::min(this->numberOfModes, totalNumberOfModes);
      std::cout << numberOfModes << " dominant modes are requested to be generated  ..." << std::endl;
    }
    else
    {
      // detect number of modes
      bool found = false;
      for (int n = totalNumberOfModes-1; n >=0; n--)
      {
        if (percentVarByMode[n] >= this->maximumVarianceCaptured && found==false)
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
    std::cout << numberOfModes << " dominant modes is found to capture " << this->maximumVarianceCaptured*100 << "% of total variation ..." << std::endl;
  }

  // start sampling along each mode
  std::vector<double> eigenValues = shapeStats.Eigenvalues();
  vnl_matrix<double> eigenVectors = shapeStats.Eigenvectors();
  vnl_vector<double> mean         = shapeStats.Mean();

  Mesh::MeshPoints meanPoints = Mesh::MeshPoints::New();
  for(unsigned int i = 0; i < this->numberOfParticles; i++)
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
    // mkdirStatus = mkdir(cur_path.c_str(), S_IRWXU);
#endif

    double sqrt_eigenValue = sqrt(eigenValues[totalNumberOfModes - modeId - 1]);
    double min_std_factor = -1 * this->maxStdDev;
    double max_std_factor = +1 * this->maxStdDev;
    std::vector<double> std_factor_store = Utils::linspace(min_std_factor, max_std_factor, this->numberOfSamplesPerMode);

    vnl_vector<double> curMode = eigenVectors.get_column(totalNumberOfModes - modeId - 1);

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
    ofstream ofs(stdfilename.c_str());

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

      vnl_vector<double> curSample = mean + cur_std * curMode;

      // fill-in the vtkpoints structure to perform warping for dense reconstruction of the current sample
      vtkSmartPointer< vtkPoints > curSamplePts = vtkSmartPointer< vtkPoints >::New();
      std::vector<Point3> curSparse;
      for(unsigned int i = 0; i < this->numberOfParticles; i++)
      {
        double pt[3];
        for (unsigned int d = 0 ; d < Dimension; d++)
            pt[d] = curSample(i*Dimension + d);

        Point3 p({pt[0], pt[1], pt[2]});

        curSamplePts->InsertNextPoint(pt);
        curSparse.push_back(p);
      }

      Mesh curDense = getMesh(transform, curSparse);

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

} // shapeworks
