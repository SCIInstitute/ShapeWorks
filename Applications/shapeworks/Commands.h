#pragma once

#include "Command.h"

namespace shapeworks {

// Image Commands
COMMAND_DECLARE(ReadImage, ImageCommand);
COMMAND_DECLARE(WriteImage, ImageCommand);
COMMAND_DECLARE(ImageInfo, ImageCommand);
COMMAND_DECLARE(Antialias, ImageCommand);
COMMAND_DECLARE(ResampleImage, ImageCommand);
COMMAND_DECLARE(RecenterImage, ImageCommand);
COMMAND_DECLARE(PadImage, ImageCommand);
COMMAND_DECLARE(Translate, ImageCommand);
COMMAND_DECLARE(Scale, ImageCommand);
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
COMMAND_DECLARE(Blur, ImageCommand);
COMMAND_DECLARE(BoundingBox, ImageCommand);
COMMAND_DECLARE(CropImage, ImageCommand);
COMMAND_DECLARE(ICPRigid, ImageCommand);
COMMAND_DECLARE(ClipVolume, ImageCommand );
COMMAND_DECLARE(ReflectVolume, ImageCommand);
COMMAND_DECLARE(SetOrigin, ImageCommand );
COMMAND_DECLARE(WarpImage, ImageCommand);
COMMAND_DECLARE(Compare, ImageCommand);
COMMAND_DECLARE(NegateImage, ImageCommand);
COMMAND_DECLARE(AddImage, ImageCommand);
COMMAND_DECLARE(SubtractImage, ImageCommand);
COMMAND_DECLARE(MultiplyImage, ImageCommand);
COMMAND_DECLARE(DivideImage, ImageCommand);
COMMAND_DECLARE(ImageToMesh, ImageCommand );

// Particle System Commands
COMMAND_DECLARE(ReadParticleSystem, ParticleSystemCommand);
COMMAND_DECLARE(Compactness, ParticleSystemCommand);
COMMAND_DECLARE(Generalization, ParticleSystemCommand);
COMMAND_DECLARE(Specificity, ParticleSystemCommand);

// Mesh Commands
COMMAND_DECLARE(ReadMesh, MeshCommand);
COMMAND_DECLARE(WriteMesh, MeshCommand);
COMMAND_DECLARE(Coverage, MeshCommand);

} // shapeworks
