#
# ShapeWorks Tools
#
The `shapeworks` executable has a variety of tools for data alignment, processing of images and meshes, reading and converting various file formats, optimization, and analysis. These tasks are described below.
#
[Image Tools]
[Mesh Tools]
[Alignment]
[Analysis]
[Optimization]
[File Utilities]

# Image Tools

Image manipulation is used to blah blah blah...

[AntiAliasing] (#anti-aliasing)
[ClipVolume]
[CloseHoles]
[Cropping Images]
[ExtractGivenLabelImage]
[FastMarching]
[FindLargestBoundingBox]
[PadVolumeWithConstant]
[ReflectVolumes]
[ResampleVolumesToBeIsotropic]
[ThresholdImages]
[TopologyPreservingSmoothing]
[WriteImageInfoToText]

## Read

This tool reads an image.

Command Line Name: read-image

It uses the following input arguments:

  --name = Name of file to read.

## Write

This tool writes an image.

Command Line Name: write-image

It uses the following input arguments:

  --name = Name of file to write.

## <a name="anti-aliasing"></a> AntiAliasing

This tool antialiases binary volumes.

Command Line Name: antialias

It uses the following input arguments:  

  --maxrmserror = The maximum RMS error determines how fast the solver converges. (Range [0.0, 1.0]) [default 0.01].  
  --numiterations = Number of iterations [default 50].  
  --numlayers = Number of layers around a 3d pixel to use for this computation [default image dims].

## PadVolumeWithConstant 

This tool pads a contant value in the x-, y-, and z- directions of a given volume.

Command Line Name: pad

It uses the following input arguments:

  --padding = Number of voxels to be padded in each direction.  
  --value = Value to be used to fill padded voxels.  

## ResampleVolumesToBeIsotropic 

This tool resamples given mri/binary volumes to have isotropic voxel spacing.

Command Line Name: isoresample

It uses the following input arguments:

  --isospacing = The isotropic spacing in all dimensions.  
  --sizez = Image size in x-direction [ize is autmatically estimated from input image].  
  --sizey = Image size in y-direction [size is autmatically estimated from input image].  
  --sizez = Image size in z-direction [size is autmatically estimated from input image].

## Binarize

This tool binarizes an image at some given threshold.

Command Line Name: binarize

It uses the following input arguments:

  --threshold = Resulting image has two values for pixels: > threshold set to inside value, <= threshold set to outside value [default epsilon].  
  --inside = Value of pixels > threshold [default 1.0].  
  --outside = Value of pixels <= threshold [default 0.0].  

## Recenter

This tool recenters an image by changing its origin in image header to the physical coordinates of the center of the image.

Command Line Name: recenter-image

## ReflectVolumes 

A command line tool that reflect 3d volume images with respect to image center and specific axis.

It uses the following input arguments:
-inFilename - Image file name which needs to be reflected.
- outFilename - Output file name for the reflected image
- paddingSize - Axis along which it needs to be reflected
- centerFilename - The filename where the image center information will be stored. 

## ClipVolume

Slice 3D Volume Using Cutting Planes

Takes set of .nrrd volumes and corresponding cutting planes, and chops the volume accordingly 
It uses a parameter file with the following tags 
    - num_shapes: number of volumes to be processed (put this correctly) 
    - inputs: paths to input files 
    - outputs: paths to output files 
    - cutting_planes : Set of cutting planes, expressed as 3 3D points separated by spaces 
    example : 1 2 -3 0 34 102 -23.4 2 0.44 

## CloseHoles

TODO

## Cropping Images

This tool is to crop the images given the starting index and bounding box size
Provide the smallest index and bounding box values from the above tool to this tool as inputs.

Inputs:

-inFilename - Input image file name
-outFilename - Output image file name
-MRIinFilename - MRI image file name
-MRIoutFilename - MRI output image file name
-bbX - bounding box value in X direction 
-bbY - bounding box value in Y direction 
-bbZ - bounding box value in Z direction
-startingIndexX - starting index in X direction
-startingIndexY - starting index in Y direction
-startingIndexZ - starting index in Z direction

## ExtractGivenLabelImage 

A command line tool that extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image.

It uses the following input arguments:
  -inFilename               The filename of the input image from which label has to be extracted.
  -labelVal                 The label value which has to be extracted.
  -outFilename              The filename of the output image.

## FastMarching

A command line tool that computes distance transform volume from a binary (antialiased) image ...

--inFilename  = The filename of the input image to be processed.
--outFilename = The filename of the output distance transform image.
--isoValue    = The level set value that defines the interface between foreground and background.
 
## FindLargestBoundingBox 

A command line tool that o compute largest bounding box size given a set of images.

It uses the following input arguments:
 -inFilename - A text file with the file names for which the largest size has to be computed.
- outPrefix - output prefix to be used to save the parameters for the estimated bounding box
- paddingSize - number of extra voxels in each direction to pad the largest bounding box, checks agains min image size is performed to make sure that this padding won't get out of bounds for the smallest image in the file names provides

## ThresholdImages

A command line tool that threholds a given image into a binary label based on upper and lower intensity bounds given by the user.....

    --inFilename = Input image file path.
    --outFilename = The filename of the output threshold image.
    --lowerThresholdLevel = The lower threshold level (optional, default = FLT_MIN)
    --upperThresholdLevel = The upper threshold level (optional, default = FLT_MAX)
    --insideValue = The inside pixel value after threshold
    --outsideValue = The outside pixel value after threshold 

## TopologyPreservingSmoothing 

A parameter file based tool that smoothes distance transforms while preserving shape's topology.
[Here is an example of an input xml file that can be used.](TopologyPreservingSmoothingExample.xml)

parameter file tags are as follows:
 -inputs               The filenames of the input distance transforms to be smoothed.
 -dtFiles              The filenames of the output smoothed distance transforms.
 -outputs              The filenames of the output smoothed isosurface images.
 -verbose                  Show each intermediate step [default 0].
 -isoValue                 Isovalue to be used to define the surface in the input distance transform [default 0.0].
 -smoothing_iterations      Number of iterations to perform smoothing [default 10].
 -alpha                    Smoothing parameter in I' = (max-min). \frac{1}{1+exp(-\frac{1-\beta}{\alpha)} + min [default 10.5].
 -beta                     Smoothing parameter in I' = (max-min). \frac{1}{1+exp(-\frac{1-\beta}{\alpha)} + min [default 10.0].
 -propagationScale         The PropagationScaling parameter can be used to switch from propagation outwards (POSITIVE) versus propagating inwards (NEGATIVE). [default 20.0].

## WriteImageInfoToText 

A command line tool that extracts header information from a nrrd file and write it to a text file

It uses the following input arguments:
 -inFilename               The filename of the input image to extract its header information (origin, size, spacing).
 -outPrefix                The output prefix to be used to save header info to <outPrefix>_<info>.txt where info is origin, size, and spacing.


### Mesh Tools

Mesh manipulation is used to blah blah blah...

## ClipClosedSurface 
## ComputeCurvatureAndCoordFiles 
## ComputeGeodesicDistanceToCurve 
## ComputeGeodesicDistanceToLandmark 
## ComputeGeodesicDistanceFromVerticesToPoints 
## ComputeMeshGeodesics 
## ComputeRasterizationVolumeOriginAndSize 
## ExtractVertexFeatureFromMesh 
## FillMeshHoles 
## FixCuttingPlanes 
## GenerateBinaryAndDTImagesFromMeshes 
## GenerateFeatureGradientFiles
## GenerateFidsFiles
## GenerateFidsFilesFromMeshes 
## GetFeatureVolume 
## ParticleBasedSurfaceReconstruction
## PreviewCmd 
## ProbeFeatureVolumesAtMeshVertices 
## ProbeNormals
## ProjectPointsOntoMesh
## ComputeMeanNormals
## GenerateNormalFeaFiles
## Reflect Meshes 
## RemoveFidsDTLeakage 
## SmoothMesh 

## ClipClosedSurface 

Given a set of meshes (vtk) and corresponding cutting planes, this tool clip each mesh with its cutting plane and result in a 
closed surface. A buffer distance can be used to move the cutting plane in the opposite direction of its normal to leave a buffer 
region below the cutting plane

It uses a parameter file with the following tags:
  - input_meshes: a list of vtk file names for meshes to be processed
  - output_meshes: the vtk filenames of the output to be produced 
  - cutting_planes: a list of cutting planes in three-point form ( x1 y1 z1 x2 y2 z2 x3 y3 z3 )
  - buffer_distance: phyical distance of the buffer region

## ComputeCurvatureAndCoordFiles 

Compute curvature and x,y,z on vertices on given triangular meshes (ply format) 

It uses a parameter file with the following tags
- mesh:      a list of ply file names for meshes to be processed
- outPath:   path to save feature files 
 
## ComputeGeodesicDistanceToCurve 

Compute geodesic distances on given triangular meshes (ply format) to a set of given curves 

It uses a parameter file with the following tags
 - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead (filenames will be affected), if true, we need to provide
              the spacing, size and origin of their distance transforms that generated those fids files
 - mesh:      a list of ply file names for meshes to be processed
 - fids:      a list of corresponding fids files (in case with_fids = 1)
 - origin_x, origin_t, origin_z: the origin in physical units of the resulting distance transform
 - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
 - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform
 - curves_prefix: where we can find the point files of the curves, this should be the common part of the filename for all given mesh files
 - curves_suffix: string (optional) that will suffix all curve files
 - curves_ext:    the extension of the curve files
 - curve_labels:  the name of each curve according to the suffix given for each curve pts file, this will also be used in the naming of the output feature files (fea)
 
This tool uses fids to project a given landmark onto the closest VERTEX on the mesh then use this vertex as a seed for geodesics computation. It will be more accurate to project the point onto 
a triangle and use its vertices as seeds to initiate geodesics to the rest of the mesh then use geodesic approximation scheme to obtain the geodesic from the projected point to all the mesh vertices.

## ComputeGeodesicDistanceToLandmark 

Compute geodesic distances on given triangular meshes (ply format) to a set of given landmarks 

It uses a parameter file with the following tags"
  - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead (filenames will be affected), if true, we need to provide
	       the spacing, size and origin of their distance transforms that generated those fids files
  - origin_x, origin_t, origin_z: the origin in physical units of the resulting distance transform
  - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
  - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform
  - mesh:      a list of ply file names for meshes to be processed
  - fids:      a list of corresponding fids files (in case with_fids = 1)
  - landmark_files: a list of text files containing the 3d point locations of the landmarks associated with each mesh (a single file for each mesh with each row a different landmark xyz)
  - landmark_labels: the name of each landmark specified in landmark files, this will be used in the naming of the output feature files (fea)
 
This tool uses fids to project a given landmark onto the closest VERTEX on the mesh then use this vertex as a seed for geodesics computation. It will be more accurate to project the point onto 
a triangle and use its vertices as seeds to initiate geodesics to the rest of the mesh then use geodesic approximation scheme to obtain the geodesic from the projected point to all the mesh vertices.

## ComputeGeodesicDistanceFromVerticesToPoints 

Generate a text file containing geodesic distance to vertices of given triangular mesh (ply) for given set of 
points with their triangle information (x y z triangleId alpha beta gamma)

Usage: ComputeGeodesicDistanceFromVerticesToPoints meshFile(.ply) pointsFile outFileName

## ComputeMeshGeodesics 

Compute pairwise vertices geodesic distances on given triangular meshes (ply format) 

It uses a parameter file with the following tags"
  - mesh: a list of ply file names for meshes to be processed
  - geo: a list of geo file names for meshes to be saved
  - stopping_distance_factor: percentage of the great circle circumference where vertices will be assigned an very large geodesic distance
 
## ComputeRasterizationVolumeOriginAndSize 

Given a set of meshes (vtk), compute the origin and size of a volume that would contain the rasterization of each mesh 
so that all meshes will be embedded within the same volume characteristics

It uses a parameter file with the following tags"
  - mesh: a list of vtk file names for meshes to be processed
  - centering: a flag to indicate whether build the to-be built rasterization is centered on the shape's center
  - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
  - pad_pixels: number of pixels to be padded in each dimension
  - out_origin_filename: the filename to save the origin information
  - out_size_filename: the filename to save the size information
   
## ExtractVertexFeatureFromMesh 

Extract scalar values associated to mesh vertices to txt file

It uses a parameter file with the following tags:
  - inputs: a list of vtk file names for meshes to be processed
  - out_path: a directory (otional) to save output files, it not indicated, out files will be saved in the same directory as the mesh files
  - vtk_feature_name: feature name in the vtk file
  - out_feature_name: feature name to be used in the output file names, if not indicated, mesh filenames will be used as is
  - read_as_ply: to read ply rather than vtk files
 
## FillMeshHoles 

Given a set of meshes (vtk), this tool finds holes in a mesh and closes them. 

It uses a parameter file with the following tags:
  - inputs: a list of vtk file names for meshes to be processed
  - outputs: the vtk filenames of the output to be produced 
  
## FixCuttingPlanes 

Given bunch of curves/landmarks for each shape, fix the orientation of the provided cutting plane to point to the side where the curves/landmarks exist

It uses a parameter file with the following tags
 - mesh:      a list of vtk/ply file names for meshes to be processed
 - input_planes:      a list of txt file names for the planes to be processed
 - output_planes:      a list of txt file names for the fixed planes to be saved
 - curves_prefix: where we can find the point files of the curves, this should be the common part of the filename for all given mesh files
 - curves_suffix: string (optional) that will suffix all curve files
 - curves_ext:    the extension of the curve files
 - curve_labels:  the name of each curve according to the suffix given for each curve pts file
 
## GenerateBinaryAndDTImagesFromMeshes 

Compute binary images (i.e. segmentation) and their distance transforms of a given set of meshes (ply format)

It uses a parameter file with the following tags"
  - mesh: a list of ply file names for meshes to be processed
  - origin_x, origin_y, origin_z: the origin in physical units of the resulting distance transform
  - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform
  - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform

## GenerateFeatureGradientFiles

Probe gradient at vertices and generate feature gradient files

It uses a parameter file with the following tags"
  - mesh: a list of ply file names for meshes to be processed ".ply"
  - fea_vol: feature volume(s) ".nrrd"
  - fea_per_mesh: number of features "int"

## GenerateFidsFiles

Compute face index map of a given set of meshes (ply format) along with its signed distance transfrom using spatial hashing

It uses a parameter file with the following tags"
  - dist: a list of approximate distance transforms 	
  - mesh: a list of ply file names for meshes to be processed
  - number_of_subvoxels: : number of subvoxels to divid each voxel (higher improve subvoxel accuarcy esp for meshes with high curvature regions)
  - number_of_voxels: number of voxels to construct a supervoxel
  - narrow_band: a narrow band defined in phyical units to limit the supvoxel-accuarate distance transform computation, 
	         distance values of voxels outside this band will be inferred using fids
  - ball_radius_factor: to reduce the radius(b) at each super-voxel. (At times b is too big and contains the whole mesh. Use < 1)
  - num_threads: number of thread to be spawned 
 
## GenerateFidsFilesFromMeshes 

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
 
## GetFeatureVolume 

Compute a volumetric representation of fea files by propagating feature values (from fea files) from mesh surface to a narrowband surrounding the mesh

It uses a parameter file with the following tags"
    - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead 
    - mesh:      a list of ply file names for meshes to be processed
    - fids:      a list of corresponding fids files (in case with_fids = 1)
    - dist:       a list of corresponding distance transforms to indicate where to fill the feature volume, i.e. the narrowband
    - fea_per_mesh:      number of fea files to be processed per mesh
    - fea:      a list of fea files to be processed such that the first fea_per_mesh fea files correspond to the first mesh and so on.
    - narrow_band:     distance in physical coordinates from the mesh its inside and outside where we propagate feature values.

## PreviewCmd 

This is a commandline tool that encapsulate preview-based triangular mesh processing tools including decimation, smoothing and fixing, it has the following inputs:
    --inFile: the input vtk filename of the mesh to be processed.
    --outFile: the output vtk filename.
    --fixWinding: do element winding fix (default: 1)
    --decimate: perform mesh decimation (default: 1)
    --doLaplacianSmoothingBeforeDecimation: perform laplacian smoothing before decimation (default: 1)
    --doLaplacianSmoothingAfterDecimation: perform laplacian smoothing after decimation (default: 1)
    --smoothingLambda: laplacian smoothing lambda  (default: 0.5)
    --smoothingIterations: laplacian smoothing number of iterations  (default: 1)
    --decimationPercentage: percentage of target number of clusters/vertices  (default: 0.5)
 
## ProbeFeatureVolumesAtMeshVertices 

Given a set of meshes (vtk) and corresponding feature volumes where these meshes live, this tool probe the feature volumes at each mesh vertex and 
output vtk meshes with scalar field defined based on such probing process (report the feature values from the feature volumes at the mesh vertices)

It uses a parameter file with the following tags"
  - input_meshes: a list of vtk file names for meshes to be processed
  - feature_volumes: a list of image files (3D) to be probed corresponding to the given set of meshes
  - output_meshes: the vtk filenames of the output to be produced

## ProbeNormals

Usage: ProbeNormals DTfilename pointsFilename outFileName

## ComputeMeanNormals

Compute mean normals using spherical coordinates for given normals for a set of shapes

It uses a parameter file with the following tags
- normals:     a list of files containing normals at a set of points on each shape (output files from ProbeNormals)
- pointsCount: number of points in every shape file
- outFileName: full filename (with path) to save resulting file (default: mean.normals.txt)

## GenerateNormalFeaFiles

Probe normals at vertices and save as fea files

It uses a parameter file with the following tags
- DT:      a list of DT file names to be processed
- mesh:    a list of mesh file names to be processed

## Reflect Meshes 

A command line tool that reflect meshes with respect to a specified center and specific axis.

-inFilename - Mesh file to be reflected.
-outFilename - The filename of the output reflection mesh.
-reflectCenterFilename(Optional) - The filename for origin about which reflection occurs. (Default reflection happes about the center of the mesh bounding box)
-inputDirection - Direction along which it needs to be reflected
-meshFormat(Optional) - The IO mesh format (Default = vtk, another option is ply)
 
## RemoveFidsDTLeakage 

This tool can be used a postprocessing for fids distance trasnform to fix voxels that are mis-signed as in or out of the isosurface in fids computation (mainly due to irregular triangulation

It uses a parameter file with the following tags"
    - fids_dist:      a list of distance transforms computed via fids
    - approx_dist:       the corresponding approximate distances (from rasterization then dt computation)
    - out_dist:       output distance transform filenames
    
## SmoothMesh 

Given a set of meshes (vtk), this tool laplacian smooth the mesh

It uses a parameter file with the following tags:
  - inputs: a list of vtk file names for meshes to be processed
  - outputs: the vtk filenames of the output to be produced 
  - iterations: number of smoothing iterations
  - relaxation_factor: amount of vertex displacement in each iteration


### Alignment 

Alignment tools are used to blah blah blah...

[ICPRigid3DImageRegistration]
[ICPRigid3DMeshRegistration]
[ReflectMeshes]
[TranslateShapeToImageOrigin]
[Resize_origin_resampleShape]

## ICPRigid3DImageRegistration 

A command line tool that performs iterative closed point (ICP) 3D rigid registration on a pair of images.

It uses the following input arguments:
Input:
    -targetDistanceMap: the distance map of target image.
    -sourceDistanceMap: the distance map of source image.
    -sourceSegmentation: the segmentation of source image.
    -isoValue: as we need to get point set from surface for ICP, this iso value is required to get the isosurface. The default value is 0.0.
    -icpIterations: the number of iterations user want to run.

Output:
    -solutionSegmentation: the filename of the aligned segmentation of source image. 

## ICPRigid3DMeshRegistration 

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

## ReflectMeshes 

Reflect meshes to make data in whole ensemble align in same direction. It uses a parameter file with the following tags"
  - inputs: a list of vtk/ply file names for meshes to be relfected
  - output: a list of output filenames
  - is_unstructured_grid: : (a scalar 1/0 for each input file) indicated whether the input meshes are in vtk unstructured grid format (Default 0)
  - direction: which axis to reflect with respect to, 0 for x-axis, 1 fir y-axis, and 2 for z-axis 

## TranslateShapeToImageOrigin

A command line tool that performs translational alignment of a given shape image based on either its center of mass or a given 3d point.

It uses the following input arguments:
-inFilename               The filename of the input shape to be transformed.
-outFilename              The filename of the output transformed shape.
-MRIinFilename            The assoicated image filename to be transformed.
-MRIoutFilename           The filename of the output transformed image.
-useCenterOfMass          A flag to transform image using center of mass.
-centerX                  x-coordinate of a user-defined center point.
-centerY                  y-coordinate of a user-defined center point.
-centerZ                  z-coordinate of a user-defined center point.
-parameterFilename        The filename to store the transform parameters

## Resize_origin_resampleShape

TODO


### Analysis

Analysis is used to blah blah blah...


### Optimization

Optimization is used to blah blah blah...



### File Utilities

The File Utilities are used to read and convert data stored in a variety of different formats.

[GENERIC MESH FORMAT CONVERSION]
[fea2vtk]
[stl2ply]
[stl2vtk]
[vtk2ply]
[VTKUnstructuredGridToPolyData]
[MeshFromDT]

## GENERIC MESH FORMAT CONVERSION
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

## fea2vtk 

Read in feature files (.fea) and their corresponding triangular mesh (ply) and output a vtk file containing the feature values as scalar onto the mesh (colormap)

It uses a parameter file with the following tags"
  - mesh:      a list of ply file names for meshes to be processed
  - fea_per_mesh:      number of fea files to be processed per mesh
  - fea:      a list of fea files to be processed such that the first fea_per_mesh fea files correspond to the first mesh and so on. 

## stl2ply

Commandline tool to convert stl file format to ply file format ... 

"Usage: stl2vtk inFilename(.stl) outFilename(.ply) 

## stl2vtk 

Commandline tool to convert stl file format to vtk file format ... 

"Usage: stl2vtk inFilename(.stl) outFilename(.vtk) 

## vtk2ply 

Commandline tool to convert vtk file format to ply file format ... 

"Usage: vtk2ply inFilename(.vtk) outFilename(.ply)

## VTKUnstructuredGridToPolyData 

Convert vtk unstructered grid data to vtk polydata. It uses a paramfile of the following tags:
  - inputs: list of input vtk files containing unstructured grid data
  - outputs: list of output vtk files which will be saved containing the vtk polydata version of the corresponding input vtkfiles
 
## MeshFromDT

TODO

## FeaFromVTK

TODO
