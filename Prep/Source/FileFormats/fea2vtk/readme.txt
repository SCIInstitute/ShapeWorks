 

-------------------------------
fea2vtk 
-------------------------------

Read in feature files (.fea) and their corresponding triangular mesh (ply) and output a vtk file containing the feature values as scalar onto the mesh (colormap)

It uses a parameter file with the following tags"
  - mesh:      a list of ply file names for meshes to be processed
  - fea_per_mesh:      number of fea files to be processed per mesh
  - fea:      a list of fea files to be processed such that the first fea_per_mesh fea files correspond to the first mesh and so on.