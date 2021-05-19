#pragma once

#include "Command.h"

namespace shapeworks {

// Image Commands
COMMAND_DECLARE(ReadImage, ImageCommand);
COMMAND_DECLARE(WriteImage, ImageCommand);
COMMAND_DECLARE(ImageInfo, ImageCommand);
COMMAND_DECLARE(Antialias, ImageCommand);
COMMAND_DECLARE(ResampleImage, ImageCommand);
COMMAND_DECLARE(ResizeImage, ImageCommand);
COMMAND_DECLARE(RecenterImage, ImageCommand);
COMMAND_DECLARE(PadImage, ImageCommand);
COMMAND_DECLARE(TranslateImage, ImageCommand);
COMMAND_DECLARE(ScaleImage, ImageCommand);
COMMAND_DECLARE(Rotate, ImageCommand);
COMMAND_DECLARE(ExtractLabel, ImageCommand);
COMMAND_DECLARE(CloseHoles, ImageCommand);
COMMAND_DECLARE(Binarize, ImageCommand);
COMMAND_DECLARE(ComputeDT, ImageCommand);
COMMAND_DECLARE(CurvatureFilter, ImageCommand);
COMMAND_DECLARE(GradientFilter, ImageCommand);
COMMAND_DECLARE(SigmoidFilter, ImageCommand);
COMMAND_DECLARE(TPLevelSetFilter, ImageCommand);
COMMAND_DECLARE(TopologyPreservingFilter, ImageCommand);
COMMAND_DECLARE(IntensityFilter, ImageCommand);
COMMAND_DECLARE(Blur, ImageCommand);
COMMAND_DECLARE(BoundingBoxImage, ImageCommand);
COMMAND_DECLARE(ImageBounds, ImageCommand);
COMMAND_DECLARE(CropImage, ImageCommand);
COMMAND_DECLARE(ICPRigid, ImageCommand);
COMMAND_DECLARE(ClipImage, ImageCommand);
COMMAND_DECLARE(ReflectImage, ImageCommand);
COMMAND_DECLARE(SetOrigin, ImageCommand);
COMMAND_DECLARE(SetSpacing, ImageCommand);
COMMAND_DECLARE(WarpImage, ImageCommand);
COMMAND_DECLARE(CompareImage, ImageCommand);
COMMAND_DECLARE(NegateImage, ImageCommand);
COMMAND_DECLARE(AddImage, ImageCommand);
COMMAND_DECLARE(SubtractImage, ImageCommand);
COMMAND_DECLARE(MultiplyImage, ImageCommand);
COMMAND_DECLARE(DivideImage, ImageCommand);
COMMAND_DECLARE(ImageToMesh, ImageCommand);
COMMAND_DECLARE(SetRegion, ImageCommand);

// Particle System Commands
COMMAND_DECLARE(ReadParticleSystem, ParticleSystemCommand);
COMMAND_DECLARE(Compactness, ParticleSystemCommand);
COMMAND_DECLARE(Generalization, ParticleSystemCommand);
COMMAND_DECLARE(Specificity, ParticleSystemCommand);

// Mesh Commands
COMMAND_DECLARE(ReadMesh, MeshCommand);
COMMAND_DECLARE(WriteMesh, MeshCommand);
COMMAND_DECLARE(MeshInfo, MeshCommand);
COMMAND_DECLARE(Coverage, MeshCommand);
COMMAND_DECLARE(Smooth, MeshCommand);
COMMAND_DECLARE(Decimate, MeshCommand);
COMMAND_DECLARE(InvertNormals, MeshCommand);
COMMAND_DECLARE(ReflectMesh, MeshCommand);
COMMAND_DECLARE(TransformMesh, MeshCommand);
COMMAND_DECLARE(FillHoles, MeshCommand);
COMMAND_DECLARE(ProbeVolume, MeshCommand);
COMMAND_DECLARE(ClipMesh, MeshCommand);
COMMAND_DECLARE(TranslateMesh, MeshCommand);
COMMAND_DECLARE(ScaleMesh, MeshCommand);
COMMAND_DECLARE(BoundingBoxMesh, MeshCommand);
COMMAND_DECLARE(MeshBounds, MeshCommand);
COMMAND_DECLARE(Distance, MeshCommand);
COMMAND_DECLARE(GenerateNormals, MeshCommand);
COMMAND_DECLARE(SetFieldValue, MeshCommand);
COMMAND_DECLARE(GetFieldValue, MeshCommand);
COMMAND_DECLARE(FieldRange, MeshCommand);
COMMAND_DECLARE(FieldMean, MeshCommand);
COMMAND_DECLARE(FieldStd, MeshCommand);
COMMAND_DECLARE(FieldNames, MeshCommand);
COMMAND_DECLARE(FixMesh, MeshCommand);
COMMAND_DECLARE(ClipClosedSurface, MeshCommand);
COMMAND_DECLARE(MeshToImage, MeshCommand);
COMMAND_DECLARE(MeshToDT, MeshCommand);
COMMAND_DECLARE(CompareMesh, MeshCommand);

// Optimize Commands
COMMAND_DECLARE(OptimizeCommand, OptimizeCommandGroup);

// Groom Commands
COMMAND_DECLARE(GroomCommand, GroomCommandGroup);

} // shapeworks
