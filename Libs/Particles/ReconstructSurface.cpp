#include "ReconstructSurface.h"
// #include "ParticleBasedSurfaceReconstruction_InputParams.h"

namespace shapeworks {

ReconstructSurface::ReconstructSurface()
{
  // InputParams params;
  // ReconstructionType reconstructor;
}

void ReconstructSurface::surface()
{
  ReconstructionType reconstructor;
  reconstructor.readMeanInfo(this->denseFile, this->sparseFile, this->goodPointsFile);

  for (unsigned int shapeNo = 0; shapeNo < this->localPointsFile.size(); shapeNo++)
  {
    std::string basename = shapeworks::StringUtils::getFilename(params.localPointsFilenames[shapeNo]);
    std::cout << "Processing: " << params.localPointsFilenames[shapeNo].c_str() << std::endl;

    PointArrayType curSparse;
    Utils::readSparseShape(curSparse, const_cast<char*> (params.localPointsFilenames[shapeNo].c_str()));

    vtkSmartPointer<vtkPolyData> curDense = reconstructor.getMesh(curSparse);

    std::string outfilename = params.out_prefix + shapeworks::StringUtils::removeExtension(shapeworks::StringUtils::getFilename(params.localPointsFilenames[shapeNo])) + "_dense.vtk";
    std::cout << "Writing: " << outfilename << std::endl;

    vtkSmartPointer<vtkPolyDataWriter> writer = vtkPolyDataWriter::New();
    writer->SetFileName(outfilename.c_str());
    writer->SetInputData(curDense);
    writer->Update();

    vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
    vertices->DeepCopy( curDense->GetPoints() );

    std::string ptsfilename = params.out_prefix + '/'+ shapeworks::StringUtils::removeExtension(shapeworks::StringUtils::getFilename(params.localPointsFilenames[shapeNo])) + "_dense.particles";
    Utils::writeSparseShape((char*) ptsfilename.c_str(), vertices);

    vtkSmartPointer<vtkPoints> curSparse_ = vtkSmartPointer<vtkPoints>::New();
    Utils::readSparseShape(curSparse_, const_cast<char*> (params.localPointsFilenames[shapeNo].c_str()));

    ptsfilename = params.out_prefix + '/'+ shapeworks::StringUtils::removeExtension(shapeworks::StringUtils::getFilename(params.localPointsFilenames[shapeNo])) + "_sparse.particles";
    Utils::writeSparseShape((char*) ptsfilename.c_str(), curSparse_);
  }
}

} // shapeworks
