 

-------------------------------
ICPRigid3DMeshRegistration 
-------------------------------

Performs iterative closed point (ICP) rigid registration on a pair of vtk meshes.

It uses a parameter file that would enable to specify the source mesh (moving) and the target mesh (fixed)
to be used to estimated the rigid transformation matrix then apply the same transformation on other meshes defined 
in the source mesh domain to be mapped to the target domain

parameter file tags are as follows:
  - source_mesh: vtk filename of the moving mesh
  - target_mesh: vtk filename of the fixed mesh
  - out_mesh : vtk filename of the aligned moving mesh to be save
  - out_transform : txt filename to save the estimated transformation
  - source_meshes: (optional) a list of vtk filenames for meshes defined in the source mesh domain 
		   to be mapped to the target domain using the same transformation matrix estimated.
  - out_meshes : a list vtk filenames to save source_meshes after applying the transformation matrix.
  - mode : Registration mode rigid, similarity, affine (default: similarity)
  - icp_iterations: number of iterations
  - debug: verbose debugging information
  - visualize: display the resulting alignment

