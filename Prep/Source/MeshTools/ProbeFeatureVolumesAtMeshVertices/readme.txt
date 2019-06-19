 

-------------------------------
ProbeFeatureVolumesAtMeshVertices 
-------------------------------

Given a set of meshes (vtk) and corresponding feature volumes where these meshes live, this tool probe the feature volumes at each mesh vertex and 
output vtk meshes with scalar field defined based on such probing process (report the feature values from the feature volumes at the mesh vertices)

It uses a parameter file with the following tags"
  - input_meshes: a list of vtk file names for meshes to be processed
  - feature_volumes: a list of image files (3D) to be probed corresponding to the given set of meshes
  - output_meshes: the vtk filenames of the output to be produced 