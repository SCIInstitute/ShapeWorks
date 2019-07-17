 

-------------------------------
GenerateFidsFilesFromMeshes 
-------------------------------

Compute face index map of a given set of meshes (ply format) along with its signed distance transfrom using spatial hashing

It uses a parameter file with the following tags"
  - mesh: a list of ply file names for meshes to be processed
  - origin_x, origin_y, origin_z: the origin in physical units of the resulting distance transform
  - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
  - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform
  - number_of_subvoxels: : number of subvoxels to divid each voxel (higher improve subvoxel accuarcy esp for meshes with high curvature regions)
  - number_of_voxels: number of voxels to construct a supervoxel
  - narrow_band: a narrow band defined in phyical units to limit the supvoxel-accuarate distance transform computation, 
	         distance values of voxels outside this band will be inferred using fids
  - ball_radius_factor: to reduce the radius(b) at each super-voxel. (At times b is too big and contains the whole mesh. Use < 1)
  - num_threads: number of thread to be spawned
