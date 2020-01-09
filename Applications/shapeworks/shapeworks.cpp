/*
 * The unified shapeworks executable for all operations of the ShapeWorks Framework.
 */

#include <exception>
#include "Executable.h"
#include "Commands.h"

using namespace Shapeworks;

int main(int argc, char *argv[])
{
  Executable shapeworks;

  shapeworks.addCommand(Antialias::getCommand());
  shapeworks.addCommand(Smoothmesh::getCommand()); 
  shapeworks.addCommand(ResampleImage::getCommand());
  shapeworks.addCommand(Binarize::getCommand());
  shapeworks.addCommand(RecenterImage::getCommand());
  //...
  
  try {
    shapeworks.run(argc, argv);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  // <ctc> would be nice to categorize commands listed in help as Image, Analyze, etc (yes it would: todo)
  // [] this is difficult on multiple levels, both in terms of categorization and listing... maybe don't worry about it
  
  return 0;
}
