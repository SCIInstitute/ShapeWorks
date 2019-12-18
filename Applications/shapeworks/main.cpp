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
  shapeworks.addCommand(Newfunction::getCommand());
 //shapeworks.addCommand(AntiAliasing::command());
  //...
  
/* (* means OptionParser already used, otherwise just argv)
 Image commands:
 [x] AntiAliasing*
 [] ClipVolume
 [] CloseHoles*
 [] CropImages*
 [] ExtractGivenLabelImage*
 [] FastMarching*
 [] FindLargestBoundingBox*
 [] PadVolumeWithConstant*
 [] ReflectVolumes*
 [] ResampleVolumesToBeIsotropic*
 [] ThresholdImages*
 [] TopologyPreservingSmoothing
 [] WriteImageInfoToText*
 [] itkTBGACLevelSetImageFilter (also has .txx file, ugh)

 Mesh commands:
 [] smoothmesh
[] ComputeCurvatureAndCoordFiles.cxx
[] ... (some of these may not actually be executables)
 	ComputeGeodesicDistanceFromVerticesToPoints.cxx 	ComputeGeodesicDistanceToCurve.cxx 	ComputeGeodesicDistanceToLandmark.cxx 	ComputeMeanNormals.cxx
ComputeMeshGeodesics.cxx 	ComputeRasterizationVolumeOriginAndSize.cxx 	ExtractVertexFeatureFromMesh.cxx 	FillMeshHoles.cxx 	FixCuttingPlanes.cxx
GenerateBinaryAndDTImagesFromMeshes.cxx 	GenerateFeatureGradientFiles.cxx 	GenerateFidsFiles.cxx 	GenerateFidsFilesFromMeshes.cxx 	GenerateNormalFeaFiles.cxx
GetFeatureVolume.cxx 	GradientTesting.cxx 	Mesh.cpp 	Mesh.cpp~ 	Mesh.h
Mesh.h~ 	ParticleBasedSurfaceReconstruction_InputParams.h 	PreviewCmd.cxx 	PreviewMeshQC/ 	ProbeFeatureVolumesAtMeshVertices.cxx
ProbeNormals.cxx 	ProjectPointsOntoMesh.cxx 	ReconstructMeanSurface.cxx 	ReconstructSamplesAlongPCAModes.cxx 	ReconstructSurface.cxx
ReflectMesh.cxx 	RemoveFidsDTLeakage.cxx 	SmoothMesh.cxx 	WriteFeatureGradientAsText.cxx 	deprecated/
fea2vtk.cxx 	itkMultiplyByConstantImageFilter.h 	meshFIM.cpp 	meshFIM.h
  ...

 Alignment commands:
 [] ReflectMeshes
 [] TranslateShapeToImageOrigin

 Optimize commands:
...

 Analyze commands:
...

 Utils commands:
...

*/

  
  try {
    shapeworks.run(argc, argv);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    return -1;
  }


  //<ctc> this should now work fine, --help's epilog lists all subcommands, and each command handles its own help
  // help: shapeworks <command> options
  //       for help on a specific command: shapeworks <command> --help
  // cmds: antialiasing
  //       etc, 
  //       ...
  //

  // <ctc> would be nice to categorize commands as Image, Analyze, etc (yes it would: todo)

  return 0;
}


