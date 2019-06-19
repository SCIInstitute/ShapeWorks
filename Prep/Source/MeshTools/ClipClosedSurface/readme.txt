 

-------------------------------
ClipClosedSurface 
-------------------------------

Given a set of meshes (vtk) and corresponding cutting planes, this tool clip each mesh with its cutting plane and result in a 
closed surface. A buffer distance can be used to move the cutting plane in the opposite direction of its normal to leave a buffer 
region below the cutting plane

It uses a parameter file with the following tags:
  - input_meshes: a list of vtk file names for meshes to be processed
  - output_meshes: the vtk filenames of the output to be produced 
  - cutting_planes: a list of cutting planes in three-point form ( x1 y1 z1 x2 y2 z2 x3 y3 z3 )
  - buffer_distance: phyical distance of the buffer region
