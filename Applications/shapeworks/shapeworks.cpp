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

  shapeworks.addCommand(ReadImage::getCommand());
  shapeworks.addCommand(WriteImage::getCommand());
  shapeworks.addCommand(Antialias::getCommand());
  shapeworks.addCommand(ResampleImage::getCommand());
  shapeworks.addCommand(IsoResampleImage::getCommand());
  shapeworks.addCommand(RecenterImage::getCommand());
  shapeworks.addCommand(PadImage::getCommand());
  shapeworks.addCommand(ExtractLabel::getCommand());
  shapeworks.addCommand(Translate::getCommand());
  shapeworks.addCommand(Specificity::getCommand());
  shapeworks.addCommand(CloseHoles::getCommand());
  shapeworks.addCommand(Threshold::getCommand());
  shapeworks.addCommand(ComputeDT::getCommand());
  shapeworks.addCommand(CurvatureFilter::getCommand());
  shapeworks.addCommand(GradientFilter::getCommand());
  shapeworks.addCommand(SigmoidFilter::getCommand());
  shapeworks.addCommand(TPLevelSetFilter::getCommand());
  shapeworks.addCommand(TopologyPreservingFilter::getCommand());
  shapeworks.addCommand(Blur::getCommand());
  shapeworks.addCommand(BoundingBox::getCommand());
  shapeworks.addCommand(CropImage::getCommand());
  shapeworks.addCommand(ICPRigid::getCommand());
  shapeworks.addCommand(ClipVolume::getCommand());
  shapeworks.addCommand(ReflectVolume::getCommand());
  shapeworks.addCommand(ChangeOrigin::getCommand());
  shapeworks.addCommand(ReadParticleSystem::getCommand());
  shapeworks.addCommand(Compactness::getCommand());
  shapeworks.addCommand(Generalization::getCommand());
  shapeworks.addCommand(ReadMesh::getCommand());
  shapeworks.addCommand(WriteMesh::getCommand());
  shapeworks.addCommand(Coverage::getCommand());

  try {
    shapeworks.run(argc, argv);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  return 0;
}
