/*
 * The unified shapeworks executable for all operations of the ShapeWorks Framework.
 */

#include <exception>
#include "Executable.h"
#include "Commands.h"

using namespace shapeworks;

int main(int argc, char *argv[])
{
  Executable shapeworks;

  // Image Commands
  shapeworks.addCommand(ReadImage::getCommand());
  shapeworks.addCommand(WriteImage::getCommand());
  shapeworks.addCommand(ImageInfo::getCommand());
  shapeworks.addCommand(Antialias::getCommand());
  shapeworks.addCommand(ResampleImage::getCommand());
  shapeworks.addCommand(ResizeImage::getCommand());
  shapeworks.addCommand(RecenterImage::getCommand());
  shapeworks.addCommand(PadImage::getCommand());
  shapeworks.addCommand(TranslateImage::getCommand());
  shapeworks.addCommand(ScaleImage::getCommand());
  shapeworks.addCommand(Rotate::getCommand());
  shapeworks.addCommand(ExtractLabel::getCommand());
  shapeworks.addCommand(CloseHoles::getCommand());
  shapeworks.addCommand(Binarize::getCommand());
  shapeworks.addCommand(ComputeDT::getCommand());
  shapeworks.addCommand(CurvatureFilter::getCommand());
  shapeworks.addCommand(GradientFilter::getCommand());
  shapeworks.addCommand(SigmoidFilter::getCommand());
  shapeworks.addCommand(TPLevelSetFilter::getCommand());
  shapeworks.addCommand(IntensityFilter::getCommand());
  shapeworks.addCommand(TopologyPreservingFilter::getCommand());
  shapeworks.addCommand(Blur::getCommand());
  shapeworks.addCommand(BoundingBoxImage::getCommand());
  shapeworks.addCommand(ImageBounds::getCommand());
  shapeworks.addCommand(CropImage::getCommand());
  shapeworks.addCommand(ICPRigid::getCommand());
  shapeworks.addCommand(ClipImage::getCommand());
  shapeworks.addCommand(ReflectImage::getCommand());
  shapeworks.addCommand(SetOrigin::getCommand());
  shapeworks.addCommand(SetSpacing::getCommand());
  shapeworks.addCommand(WarpImage::getCommand());
  shapeworks.addCommand(ImageToMesh::getCommand());
  shapeworks.addCommand(NegateImage::getCommand());
  shapeworks.addCommand(AddImage::getCommand());
  shapeworks.addCommand(SubtractImage::getCommand());
  shapeworks.addCommand(MultiplyImage::getCommand());
  shapeworks.addCommand(DivideImage::getCommand());
  shapeworks.addCommand(CompareImage::getCommand());
  shapeworks.addCommand(SetRegion::getCommand());

  // Particle System Commands
  shapeworks.addCommand(ReadParticleSystem::getCommand());
  shapeworks.addCommand(Compactness::getCommand());
  shapeworks.addCommand(Generalization::getCommand());
  shapeworks.addCommand(Specificity::getCommand());

  // Mesh Commands
  shapeworks.addCommand(ReadMesh::getCommand());
  shapeworks.addCommand(WriteMesh::getCommand());
  shapeworks.addCommand(MeshInfo::getCommand());
  shapeworks.addCommand(Coverage::getCommand());
  shapeworks.addCommand(Smooth::getCommand());
  shapeworks.addCommand(SmoothSinc::getCommand());
  shapeworks.addCommand(Decimate::getCommand());
  shapeworks.addCommand(CVDDecimate::getCommand());
  shapeworks.addCommand(InvertNormals::getCommand());
  shapeworks.addCommand(ReflectMesh::getCommand());
  shapeworks.addCommand(TransformMesh::getCommand());
  shapeworks.addCommand(FillHoles::getCommand());
  shapeworks.addCommand(ProbeVolume::getCommand());
  shapeworks.addCommand(ClipMesh::getCommand());
  shapeworks.addCommand(TranslateMesh::getCommand());
  shapeworks.addCommand(ScaleMesh::getCommand());
  shapeworks.addCommand(BoundingBoxMesh::getCommand());
  shapeworks.addCommand(MeshBounds::getCommand());
  shapeworks.addCommand(Distance::getCommand());
  shapeworks.addCommand(FixElement::getCommand());
  shapeworks.addCommand(ClipClosedSurface::getCommand());
  shapeworks.addCommand(ComputeNormals::getCommand());
  shapeworks.addCommand(ClosestPoint::getCommand());
  shapeworks.addCommand(GeodesicDistance::getCommand());
  shapeworks.addCommand(GeodesicDistanceToLandmark::getCommand());
  shapeworks.addCommand(MeanNormals::getCommand());
  shapeworks.addCommand(SetField::getCommand());
  shapeworks.addCommand(GetField::getCommand());
  shapeworks.addCommand(SetFieldValue::getCommand());
  shapeworks.addCommand(GetFieldValue::getCommand());
  shapeworks.addCommand(FieldRange::getCommand());
  shapeworks.addCommand(FieldMean::getCommand());
  shapeworks.addCommand(FieldStd::getCommand());
  shapeworks.addCommand(FieldNames::getCommand());
  shapeworks.addCommand(MeshToImage::getCommand());
  shapeworks.addCommand(MeshToDT::getCommand());
  shapeworks.addCommand(CompareMesh::getCommand());
  shapeworks.addCommand(WarpMesh::getCommand());

  // Misc Commands
  shapeworks.addCommand(Seed::getCommand());
  shapeworks.addCommand(OptimizeCommand::getCommand());
  shapeworks.addCommand(GroomCommand::getCommand());

  try {
    return shapeworks.run(argc, argv);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
