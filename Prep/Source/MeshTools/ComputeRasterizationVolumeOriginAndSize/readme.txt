 

-------------------------------
ComputeRasterizationVolumeOriginAndSize 
-------------------------------

Given a set of meshes (vtk), compute the origin and size of a volume that would contain the rasterization of each mesh 
so that all meshes will be embedded within the same volume characteristics

It uses a parameter file with the following tags"
  - mesh: a list of vtk file names for meshes to be processed
  - centering: a flag to indicate whether build the to-be built rasterization is centered on the shape's center
  - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
  - pad_pixels: number of pixels to be padded in each dimension
  - out_origin_filename: the filename to save the origin information
  - out_size_filename: the filename to save the size information
  