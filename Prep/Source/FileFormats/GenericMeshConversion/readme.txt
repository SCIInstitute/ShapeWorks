-------------------------------
GENERIC MESH FORMAT CONVERSION
-------------------------------
Converts different mesh formats (vtk, ply, stl, obj) to other formats

It uses a parameter file with the following tags
- input_format: integer value specifying the input mesh file format
	 (1) .vtk 
	 (2) .ply 
	 (3) .stl 
	 (4) .obj 
- output_format: integer value specifying the output mesh file format
	 (1) .vtk 
	 (2) .ply 
	 (3) .stl 
- input_mesh: paths of the input meshes
- output_mesh: paths of the output meshes

Usage: 
./GenericMeshConversion paramfile 