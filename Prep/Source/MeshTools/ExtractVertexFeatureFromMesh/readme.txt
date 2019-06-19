 

-------------------------------
ExtractVertexFeatureFromMesh 
-------------------------------

Extract scalar values associated to mesh vertices to txt file

It uses a parameter file with the following tags:
  - inputs: a list of vtk file names for meshes to be processed
  - out_path: a directory (otional) to save output files, it not indicated, out files will be saved in the same directory as the mesh files
  - vtk_feature_name: feature name in the vtk file
  - out_feature_name: feature name to be used in the output file names, if not indicated, mesh filenames will be used as is
  - read_as_ply: to read ply rather than vtk files
