-------------------------------
ProbeNormals
-------------------------------

Usage: ProbeNormals DTfilename pointsFilename outFileName

-------------------------------
ComputeMeanNormals
-------------------------------

Compute mean normals using spherical coordinates for given normals for a set of shapes

It uses a parameter file with the following tags
- normals:     a list of files containing normals at a set of points on each shape (output files from ProbeNormals)
- pointsCount: number of points in every shape file
- outFileName: full filename (with path) to save resulting file (default: mean.normals.txt)


-------------------------------
GenerateNormalFeaFiles
-------------------------------

Probe normals at vertices and save as fea files

It uses a parameter file with the following tags
- DT:      a list of DT file names to be processed
- mesh:    a list of mesh file names to be processed



