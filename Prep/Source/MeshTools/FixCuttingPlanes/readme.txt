 
-------------------------------
FixCuttingPlanes 
-------------------------------

Given bunch of curves/landmarks for each shape, fix the orientation of the provided cutting plane to point to the side where the curves/landmarks exist

It uses a parameter file with the following tags
 - mesh:      a list of vtk/ply file names for meshes to be processed
 - input_planes:      a list of txt file names for the planes to be processed
 - output_planes:      a list of txt file names for the fixed planes to be saved
 - curves_prefix: where we can find the point files of the curves, this should be the common part of the filename for all given mesh files
 - curves_suffix: string (optional) that will suffix all curve files
 - curves_ext:    the extension of the curve files
 - curve_labels:  the name of each curve according to the suffix given for each curve pts file
