#include "ReconstructSurface.h"
#include "StringUtils.h" 
#include "ParticleShapeStatistics.h"
#include "Utils.h"

#include <vtkKdTreePointLocator.h>

namespace shapeworks {

template<class TransformType>
ReconstructSurface<TransformType>::ReconstructSurface(const std::vector<std::string> &localPointsFiles)
{
  this->localPointsFiles = localPointsFiles;
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
void ReconstructSurface<TransformType>::CheckMapping(TransformType transform, Mesh::MeshPoints sourcePoints, Mesh::MeshPoints targetPoints,
                                                     Mesh::MeshPoints& mappedCorrespondences, double& rms, double& rms_wo_mapping, double& maxmDist)
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

template<class TransformType>
void ReconstructSurface<TransformType>::generateWarpedMeshes(TransformType transform, vtkSmartPointer<vtkPolyData>& outputMesh)
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
Mesh ReconstructSurface<TransformType>::getMesh(TransformType transform, std::vector<Point3> localPoints)
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
  denseShape->DeepCopy(this->denseMean);
  generateWarpedMeshes(transform, denseShape);
  return Mesh(denseShape);
}

template<class TransformType>
std::vector<std::vector<Point3>> ReconstructSurface<TransformType>::setLocalPointsFiles(const std::vector<std::string> localPointsFiles)
{
  std::vector<std::vector<Point3>> localPoints;

  for (int i=0; i<this->localPointsFiles.size(); i++)
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

  for (int i=0; i<this->worldPointsFiles.size(); i++)
  {
    std::vector<Point3> curShape;
    Utils::readSparseShape(curShape, const_cast<char*>(this->worldPointsFiles[i].c_str()));

    worldPoints.push_back(curShape);
  }

  return worldPoints;
}

template<class TransformType>
void ReconstructSurface<TransformType>::surface(TransformType transform, std::string denseFile, std::string sparseFile, std::string goodPointsFile)
{
  double maxAngleDegrees = this->normalAngle * (180.0 / Pi);

  this->denseMean = Mesh(denseFile).getVTKMesh();
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

template<class TransformType>
void ReconstructSurface<TransformType>::samplesAlongPCAModes(TransformType transform, std::string denseFile, std::string sparseFile, std::string goodPointsFile)
{
  int domainsPerShape = 1;
  const unsigned int Dimension = 3;
  ParticleShapeStatistics shapeStats;

  this->denseMean = Mesh(denseFile).getVTKMesh();
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
    // mkdirStatus = mkdir(cur_path.c_str(), S_IRWXU); // TODO: does not built
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
