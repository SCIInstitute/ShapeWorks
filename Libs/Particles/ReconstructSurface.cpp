#include "ReconstructSurface.h"
#include "StringUtils.h" 
#include "ParticleShapeStatistics.h"
#include "Utils.h"

namespace shapeworks {

// ReconstructSurface::ReconstructSurface(const std::string& densePath, const std::string& sparsePath, const std::string& pointsPath)
// {
//   this->denseMean = Mesh(densePath);
//   this->sparseMean = getSparseMean(sparsePath);
//   this->goodPoints = getGoodPoints(pointsPath);
// }

vtkSmartPointer<vtkPoints> ReconstructSurface::getSparseMean(const std::string& sparsePath)
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

std::vector<bool> ReconstructSurface::getGoodPoints(const std::string& pointsPath)
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

Mesh ReconstructSurface::getMesh(std::vector<Point3> local_pts)
{
  //default reconstruction if no warping to dense mean has occurred yet
  // if (!this->denseDone_) {
  //     return vtkSmartPointer<vtkPolyData>::New();
  // }
  // //we have a dense mean, so lets warp to subject space
  // std::vector<int> particles_indices;
  // for (int i = 0; i < this->goodPoints_.size(); i++) {
  //     if (this->goodPoints_[i]) {
  //         particles_indices.push_back(i);
  //     }
  // }
  // vtkSmartPointer< vtkPoints > subjectPts = vtkSmartPointer<vtkPoints>::New();
  // for (auto &a : local_pts) {
  //     subjectPts->InsertNextPoint(a[0], a[1], a[2]);
  // }
  // double sigma = this->computeAverageDistanceToNeighbors(subjectPts,
  //                                                         particles_indices);
  // // (3) set up the common source points for all warps
  // PointIdType id;
  // // NOTE that, since we are not resampling images, this warping is a
  // // forward warping from mean space to subject space
  // // Define container for source landmarks that corresponds to the mean space, this is
  // // the moving mesh which will be warped to each individual subject
  // typename PointSetType::Pointer sourceLandMarks = PointSetType::New();
  // typename PointSetType::PointsContainer::Pointer sourceLandMarkContainer =
  //         sourceLandMarks->GetPoints();
  // PointType ps;
  // id = itk::NumericTraits< PointIdType >::Zero;
  // int ns = 0;
  // for (unsigned int ii = 0; ii < local_pts.size(); ii++) {
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
  // typename TransformType::Pointer transform = TransformType::New();
  // transform->SetSigma(sigma); // smaller means more sparse
  // transform->SetStiffness(1e-10);
  // transform->SetSourceLandmarks(sourceLandMarks);
  // // Define container for target landmarks corresponds to the subject shape
  // typename PointSetType::Pointer targetLandMarks = PointSetType::New();
  // PointType pt;
  // typename PointSetType::PointsContainer::Pointer targetLandMarkContainer =
  //         targetLandMarks->GetPoints();
  // id = itk::NumericTraits< PointIdType >::Zero;
  // int nt = 0;
  // for (unsigned int ii = 0; ii < local_pts.size(); ii++) {
  //     if (std::find(particles_indices.begin(),
  //                   particles_indices.end(), ii) != particles_indices.end()) {
  //         double p[3];
  //         subjectPts->GetPoint(ii, p);
  //         pt[0] = p[0];
  //         pt[1] = p[1];
  //         pt[2] = p[2];
  //         targetLandMarkContainer->InsertElement(id++, pt);
  //         nt++;
  //     }
  // }
  // transform->SetTargetLandmarks(targetLandMarks);
  // // check the mapping (inverse here)
  // // this means source points (current sample's space) should
  // //  be warped to the target (mean space)
  // vtkSmartPointer<vtkPoints> mappedCorrespondences =
  //         vtkSmartPointer<vtkPoints>::New();
  // double rms;
  // double rms_wo_mapping;
  // double maxmDist;
  // this->CheckMapping(this->sparseMean_, subjectPts,
  //                     transform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);
  // vtkSmartPointer<vtkPolyData> denseShape = vtkSmartPointer<vtkPolyData>::New();
  // denseShape->DeepCopy(this->denseMean_);
  // this->generateWarpedMeshes(transform, denseShape);
  // return denseShape;
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

  setDistanceTransformFiles(distanceTransformFiles);
  std::vector<std::vector<Point3>> localPoints = setLocalPointsFiles(localPointsFiles);

  if(this->worldPointsFiles.size() == 0)
  {
    // TODO
  }

  else
  {
    std::vector<std::vector<Point3>> worldPoints = setWorldPointsFiles(worldPointsFiles);

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

}

void ReconstructSurface::surface()
{
  // reconstructor_.readMeanInfo(this->denseFile, this->sparseFile, this->goodPointsFile);

  for (unsigned int shapeNo = 0; shapeNo < this->localPointsFiles.size(); shapeNo++)
  {
    std::string basename = StringUtils::getFilename(this->localPointsFiles[shapeNo]);
    std::cout << "Processing: " << this->localPointsFiles[shapeNo].c_str() << std::endl;

    std::vector<Point3> curSparse;
    Utils::readSparseShape(curSparse, const_cast<char*> (this->localPointsFiles[shapeNo].c_str()));

    Mesh curDense = getMesh(curSparse);

    std::string outfilename = this->out_prefix + StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[shapeNo])) + "_dense.vtk";
    std::cout << "Writing: " << outfilename << std::endl;
    curDense.write(outfilename);

    std::string ptsfilename = this->out_prefix + '/'+ StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[shapeNo])) + "_dense.particles";
    Utils::writeSparseShape((char*) ptsfilename.c_str(), curDense.getVTKMesh()->GetPoints());

    vtkSmartPointer<vtkPoints> curSparse_ = vtkSmartPointer<vtkPoints>::New();
    Utils::readSparseShape(curSparse_, const_cast<char*> (this->localPointsFiles[shapeNo].c_str()));

    ptsfilename = this->out_prefix + '/'+ StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFiles[shapeNo])) + "_sparse.particles";
    Utils::writeSparseShape((char*) ptsfilename.c_str(), curSparse_);
  }
}

// TODO: FINISH CONSOLIDATING THIS
// int ReconstructSurface::samplesAlongPCAModes(int mode_index, int number_of_modes, float maximum_variance_captured, int number_of_samples_per_mode)
// {
//   ParticleShapeStatistics shapeStats;
//   int domainsPerShape = 1;

//   reconstructor_.readMeanInfo(this->denseFile, this->sparseFile, this->goodPointsFile);

//   // read-in the world/global coordinates of the given sparse correspondence model (using the output of ReconstructMeanSurface)
//   std::vector<std::vector<Point3>> global_pts;
//   for (unsigned int shapeNo = 0; shapeNo < this->worldPointsFile.size(); shapeNo++)
//   {
//     std::vector<Point3> curShape;
//     curShape.clear();

//     std::string curfilename = this->worldPointsFile[shapeNo];
//     std::cout << "curfilename: " << curfilename << std::endl;

//     Utils::readSparseShape(curShape,(char*)curfilename.c_str());

//     std::cout << "curShape.size() = " << curShape.size() << "-------------\n";
//     global_pts.push_back(curShape);
//   }

//   // perform PCA on the global points that were used to compute the dense mean mesh
//   shapeStats.DoPCA(global_pts, domainsPerShape);

//   std::vector<double> percentVarByMode = shapeStats.PercentVarByMode();
//   int TotalNumberOfModes = percentVarByMode.size();

//   int NumberOfModes = 0;
//   bool singleModeToBeGen = false;

//   if ((mode_index >= 0) && (mode_index < TotalNumberOfModes))
//   {
//     NumberOfModes = 1;
//     singleModeToBeGen = true;
//     std::cout << "Mode #" << mode_index << " is requested to be generated  ..." << std::endl;
//   }
//   else 
//   {
//     if (number_of_modes > 0)
//     {
//       NumberOfModes = std::min(number_of_modes, TotalNumberOfModes);
//       std::cout << NumberOfModes << " dominant modes are requested to be generated  ..." << std::endl;
//     }
//     else 
//     {
//       // detect number of modes
//       bool found = false;
//       for (int n = TotalNumberOfModes-1; n >=0; n--)
//       {
//         if (percentVarByMode[n] >= maximum_variance_captured && found == false)
//         {
//           NumberOfModes = n;
//           found = true;
//         }
//       }

//       if(!found)
//         NumberOfModes = percentVarByMode.size();

//       if (NumberOfModes == 0)
//       {
//         std::cerr << "No dominant modes detected!!!!!" << std::endl;
//         return EXIT_FAILURE;
//       }
//       }
//     std::cout << NumberOfModes << " dominant modes is found to capture " << maximum_variance_captured * 100 << "% of total variation ..." << std::endl;
//   }

//   // start sampling along each mode
//   std::vector<double> eigenValues  = shapeStats.Eigenvalues();
//   vnl_matrix<double> eigenVectors = shapeStats.Eigenvectors();
//   vnl_vector<double> mean         = shapeStats.Mean();

//   vtkSmartPointer< vtkPoints > meanPts = vtkSmartPointer< vtkPoints >::New();
//   for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
//   {
//       double pt[3];
//       for (unsigned int d = 0 ; d < Dimension; d++)
//           pt[d] = mean(ii*Dimension + d);

//       meanPts->InsertNextPoint(pt);
//   }

//   std::cout << "eigenValues: " ;
//   for (unsigned int sid = 0 ; sid < eigenValues.size(); sid++)
//       std::cout << eigenValues[sid] << ", " ;
//   std::cout << std::endl;

//   std::string prefix = shapeworks::StringUtils::getFilename(this->out_prefix);

//   for (int modeId = 0 ; modeId < TotalNumberOfModes; modeId ++)
//   {
//     if (singleModeToBeGen && (modeId != mode_index))
//       continue;

//     if ((!singleModeToBeGen) && (modeId >= NumberOfModes))
//         break;

//     std::string modeStr  = Utils::int2str(modeId, 2);
//     std::string cur_path = params.out_path + "/mode-" + modeStr;
//     int mkdirStatus;

// #ifdef WIN32
//     mkdirStatus = _mkdir(cur_path.c_str());
// #else
//     mkdirStatus = mkdir(cur_path.c_str(), S_IRWXU);
// #endif

//     double sqrt_eigenValue = sqrt(eigenValues[TotalNumberOfModes - modeId - 1]);

//     double min_std_factor = -1 * params.maximum_std_dev;
//     double max_std_factor = +1 * params.maximum_std_dev;
//     std::vector<double> std_factor_store = Utils::linspace(min_std_factor, max_std_factor, number_of_samples_per_mode);

//     vnl_vector<double> curMode = eigenVectors.get_column(TotalNumberOfModes - modeId - 1);

//     std::vector<double> std_store;
//     std::cout << "std_store: " ;
//     for (unsigned int sid = 0 ; sid < std_factor_store.size(); sid++)
//     {
//       std_store.push_back(std_factor_store[sid]*sqrt_eigenValue);
//       std::cout << std_store[sid] << ", " ;
//     }
//     std::cout << std::endl;

//     // writing stds on file
//     std::string stdfilename = cur_path + "/" + prefix + "mode-" + modeStr + "_stds.txt";
//     ofstream ofs(stdfilename.c_str());

//     if ( !ofs )
//       throw std::runtime_error("Could not open file for output: " + stdfilename);

//     for (unsigned int sid = 0; sid < std_factor_store.size(); sid++)
//       ofs << std_factor_store[sid] << "\n" ;
//     ofs.close();

//         for(unsigned int sampleId = 0 ; sampleId < std_store.size(); sampleId++)
//         {
//             std::string sampleStr = Utils::int2str(int(sampleId), 3);

//             std::string basename = prefix + "mode-" + modeStr + "_sample-" + sampleStr ;
//             //std::string basename =  "_mode-" + modeStr + "_sample-" + sampleStr ;

//             std::cout << "generating mode #" + Utils::num2str((float)modeId) + ", sample #" + Utils::num2str((float)sampleId) << std::endl;

//             // generate the sparse shape of the current sample
//             double cur_std = std_store[sampleId];

//             std::cout << "cur_std: " << cur_std << std::endl;

//             vnl_vector<double> curSample = mean + cur_std * curMode;

//             // fill-in the vtkpoints structure to perform warping for dense reconstruction of the current sample
//             vtkSmartPointer< vtkPoints > curSamplePts = vtkSmartPointer< vtkPoints >::New();
//             PointArrayType curSparse;
//             for(unsigned int ii = 0 ; ii < params.number_of_particles; ii++)
//             {
//                 double pt[3];
//                 for (unsigned int d = 0 ; d < Dimension; d++)
//                     pt[d] = curSample(ii*Dimension + d);

//                 PointType p;
//                 p[0] = pt[0]; p[1] = pt[1]; p[2] = pt[2];

//                 curSamplePts->InsertNextPoint(pt);
//                 curSparse.push_back(p);
//             }
  
// }

// TODO: FINISH CONSOLIDATING THIS
void ReconstructSurface::writeOutFiles()
{
  // Mesh curDense = reconstructor_.getMesh(curSparse);

  // std::string outfilename = this->out_prefix + StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFile[shapeNo])) + "_dense.vtk";
  // std::cout << "Writing: " << outfilename << std::endl;
  // curDense.write(outfilename);

  // std::string ptsfilename = this->out_prefix + '/'+ StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFile[shapeNo])) + "_dense.particles";
  // Utils::writeSparseShape((char*) ptsfilename.c_str(), curDense.getVTKMesh()->GetPoints());

  // vtkSmartPointer<vtkPoints> curSparse_ = vtkSmartPointer<vtkPoints>::New();
  // Utils::readSparseShape(curSparse_, const_cast<char*> (this->localPointsFile[shapeNo].c_str()));

  // ptsfilename = this->out_prefix + '/'+ StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFile[shapeNo])) + "_sparse.particles";
  // Utils::writeSparseShape((char*) ptsfilename.c_str(), curSparse_);
}

} // shapeworks
