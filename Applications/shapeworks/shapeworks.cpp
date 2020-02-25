/*
 * The unified shapeworks executable for all operations of the ShapeWorks Framework.
 */

#include <exception>
#include "Executable.h"
#include "Commands.h"
#include "ParticleCommands.h"

using namespace shapeworks;

int main(int argc, char *argv[])
{
  Executable shapeworks;

  shapeworks.addCommand(ReadImage::getCommand());
  shapeworks.addCommand(WriteImage::getCommand());
  shapeworks.addCommand(ReadMesh::getCommand());
  shapeworks.addCommand(WriteMesh::getCommand());
  shapeworks.addCommand(Antialias::getCommand());
  shapeworks.addCommand(SmoothMesh::getCommand());
  shapeworks.addCommand(ResampleImage::getCommand());
  shapeworks.addCommand(Binarize::getCommand());
  shapeworks.addCommand(RecenterImage::getCommand());
  shapeworks.addCommand(PadImage::getCommand());
  shapeworks.addCommand(Coverage::getCommand());
  shapeworks.addCommand(CenterOfMassAlign::getCommand());
  shapeworks.addCommand(ReadParticleSystem::getCommand());
  shapeworks.addCommand(Compactness::getCommand());
  shapeworks.addCommand(Generalization::getCommand());
  shapeworks.addCommand(Specificity::getCommand());
  shapeworks.addCommand(Resample::getCommand());

  try {
    shapeworks.run(argc, argv);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  return 0;
}
