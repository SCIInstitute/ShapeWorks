#include "ReconstructSurface.h"
#include "StringUtils.h"
#include "Mesh.h"
// #include "ParticleBasedSurfaceReconstruction_InputParams.h"

namespace shapeworks {

ReconstructSurface::ReconstructSurface(TransformType transform, InterpType interp)
{
  // switch (transform) {
  //   case ThinPlateSplineTransform:
  //     switch(interp) {
  //       case LinearInterpolation:
  //         // default
  //         break;
  //       case BSplineInterpolation:
  //         this->reconstructor_ = Reconstruction <itk::ThinPlateSplineKernelTransform2,
  //                                                    itk::BSplineInterpolateImageFunctionWithDoubleCoefficents,
  //                                                    TCoordRep, Image::PixelType, Image::ImageType>();
  //         break;
  //     }
  //     break;

  //   case RBFSSparseTransform:
  //     switch(interp) {
  //       case LinearInterpolation:
  //         using ReconstructionType = Reconstruction <itk::CompactlySupportedRBFSparseKernelTransform,
  //                                                    itk::LinearInterpolateImageFunction,
  //                                                    TCoordRep, Image::PixelType, Image::ImageType>;
  //         break;
  //       case BSplineInterpolation:
  //         using ReconstructionType = Reconstruction <itk::CompactlySupportedRBFSparseKernelTransform,
  //                                                    itk::BSplineInterpolateImageFunctionWithDoubleCoefficents,
  //                                                    TCoordRep, Image::PixelType, Image::ImageType>;
  //         break;
  //     }
  //     break;
  // }
}

void ReconstructSurface::surface()
{
  reconstructor_.readMeanInfo(this->denseFile, this->sparseFile, this->goodPointsFile);

  for (unsigned int shapeNo = 0; shapeNo < this->localPointsFile.size(); shapeNo++)
  {
    std::string basename = StringUtils::getFilename(this->localPointsFile[shapeNo]);
    std::cout << "Processing: " << this->localPointsFile[shapeNo].c_str() << std::endl;

    PointArrayType curSparse;
    Utils::readSparseShape(curSparse, const_cast<char*> (this->localPointsFile[shapeNo].c_str()));

    Mesh curDense = reconstructor_.getMesh(curSparse);

    std::string outfilename = this->out_prefix + StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFile[shapeNo])) + "_dense.vtk";
    std::cout << "Writing: " << outfilename << std::endl;
    curDense.write(outfilename);

    std::string ptsfilename = this->out_prefix + '/'+ StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFile[shapeNo])) + "_dense.particles";
    Utils::writeSparseShape((char*) ptsfilename.c_str(), curDense.getVTKMesh()->GetPoints());

    vtkSmartPointer<vtkPoints> curSparse_ = vtkSmartPointer<vtkPoints>::New();
    Utils::readSparseShape(curSparse_, const_cast<char*> (this->localPointsFile[shapeNo].c_str()));

    ptsfilename = this->out_prefix + '/'+ StringUtils::removeExtension(StringUtils::getFilename(this->localPointsFile[shapeNo])) + "_sparse.particles";
    Utils::writeSparseShape((char*) ptsfilename.c_str(), curSparse_);
  }
}

} // shapeworks
