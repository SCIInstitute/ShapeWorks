 

-------------------------------
SmoothMesh 
-------------------------------

Given a set of meshes (vtk), this tool laplacian smooth the mesh

It uses a parameter file with the following tags:
  - inputs: a list of vtk file names for meshes to be processed
  - outputs: the vtk filenames of the output to be produced 
  - iterations: number of smoothing iterations
  - relaxation_factor: amount of vertex displacement in each iteration
