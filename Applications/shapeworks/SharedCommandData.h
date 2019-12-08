#pragma once

#include "Image.h"
#include "Mesh.h"
#include <string>

namespace Shapeworks {

// Most commands can be executed one file at a time, so this class stores the shared data to enable successive operations on the results of these commands.
// TODO: commands like Clip should use this method rather than independently modifying each element of a group of files.

// Some commands (like FindLargestBoundingBox) require a set of files to be loaded to produce a single result. These take a list of files that are loaded one at a time (which is the best way since if files are huge it might not be possible to read them all at once). SharedCommandData does not help with these.

// The individal commands can be either Mesh or Image (or point cloud? or...?), and they are specialized to load/save the proper data type, but all of these data types are stored in this struct in case a command requires more than one type, or reads one and produces another.

// Items used for successive operations by the various commands, including:
// - images
// - meshes
// - point clouds
// - ... ?
struct SharedCommandData
{
  Image image;
  Mesh mesh;

  itk::TranslationTransform<double, 3/*dimension*/>::Pointer imageTransform;
};

} // Shapeworks
