# How to Groom Your Dataset?

ShapeWorks needs suitable distance transforms for establishing shape correspondence. Groom stage has a pipeline to 
generate the distance transforms from input images (binary segmentation or mesh).  It consists of image/mesh pre-processing and 
alignment tools.

## Image Pre-Processing 

### Isotropic Resampling
This command line uses an interpolation process to resample anisotropic voxels into isotropic-sized voxels. The command
line tool can be used as follows.

``
ResampleVolumesToIsotropic --inFilename $1 --outFilename $2 --isBinaryImage $3 --isoSpacing $4 --isCenterImageOn $5
``
Here is the list of arguments.

* inFilename: The filename of the input image to be resampled.
* outFilename: The filename of the output resampled image.
* isBinaryImage: A flag to treat the input image as a binary image (specialized resampling pipeline).
* isoSpacing: The isotropic spacing in all dimensions.
* isCenterImageOn: A flag to center the image, i.e. change the origin in the image header to the physcial coordinates
 of the first voxel (lower left corner).
* sizeX                    Image size in x-direction (optional, if set to 0, the size is autmatically estimated from the input image).
* sizeY                    Image size in y-direction (optional, if set to 0, the size is autmatically estimated from the input image).
* sizeZ                    Image size in z-direction (optional, if set to 0, the size is autmatically estimated from the input image).

### Constant Padding

This command line tool pads the volume in x, y and z directions with a constant value.

``
PadVolumeWithConstant --inFilename $1 --outFilename $2 --paddingSize $3 --paddingValue $4
``
* inFilename: The filename of the input image to be padded.
* outFilename: The filename of the padded image.
* paddingSize: An integer for the number of voxels to be padded in each direction.
* paddingValue : A read value to be used to fill padded voxels.

### Find Largest Bounding Box

This command line tool computes the largest bounding box for the shape population needed for cropping all the images.

``
 FindLargestBoundingBox --inFilename $1 --outPrefix $2 --paddingSize $3 
``

It uses the following input arguments.
* inFilename: The filename of the input image.
* outPrefix : Output prefix to be used to save the parameters for the estimated bounding box in a *txt* file.
* paddingSize : number of extra voxels in each direction to pad the largest bounding box, checks minimum image size is performed to
make sure that this padding won’t get out of bounds for the smallest image in the file names provides.

### Cropping

Using this command line tool we can crop all the images to specified dimensions. We usually use the largest bounding box 
to crop the images.

``
CropImages --inFilename $1 --outFilename $2 --MRIinFilename $3 --MRIoutFilename $4 --bbX $5 --bbY $6 --bbZ $7 
--startingIndexX $8 --startingIndexY $9 --startingIndexZ $10 
``
here is the list of arguments.
* inFilename: Input image file name(binary image).
* outFilename: Output image file name.
* MRIinFilename: MRI image file name.
* MRIoutFilename: MRI output image file name.
* bbX: Bounding box value in X direction.
* bbY: Bounding box value in Y direction.
* bbZ: Bounding box value in Z direction.
* startingIndexX: Starting index in X direction.
* startingIndexY: Starting index in Y direction.
* startingIndexZ: Starting index in Z direction.

### Threshold Images
This tool performs segmentation of input volume based on the specified upper and lower bounds. 

``
ThresholdImages --inFilename $1 --outFilename $2 --lowerThresholdLevel $3 --upperThreholdLevel $4 --insideValue $5 --outsideValue $6
``

The parameters are as follows.

* inFilename: Input image file name.
* outFilename: The filename of the output threshold image.
* lowerThresholdLevel: (optional, default: FLT MIN) The lower threshold level.
* upperThresholdLevel: (optional, default: FLT MAX) The upper threshold level. 
* insideValue: The inside pixel value after threshold.
* outsideValue: The outside pixel value after threshold.

### Reflect Images

A command line tool that reflect 3D volume images with respect to image center and specific axis.

``
ReflectVolumes --inFilename $1 --outFilename $2 --centerFilename $3 --inputDirection $4 
``

It uses the following input arguments:
* inFilename: Image file name which needs to be reflected.
* outFilename: Output file name for the reflected image
* inputDirection: Axis along which it needs to be reflected
* centerFilename: The filename where the image center information will be stored. 

### Extract Given Label from Image

A command line tool that extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image.
This tool is used mostly to process the reference image for alignment.

``
ExtractGivenLabelImage  --inFilename $1 --outFilename $2 --labelVal $3
``
List of arguments are as follows.
* inFilename: The filename of the input image from which label has to be extracted.
* outFilename: The filename of the output binary image.
* labelVal: The label value which has to be extracted.

### Close Holes

This tool closes possible holes in the binary images.

``
CloseHoles --inFilename $1 --outFilename $2
``

* inFilename: The filename of the input binary image.
* outFilename: The filename of the output binary image( usually the same os the inFilename).

### Fast Marching
A tool that computes distance transform volume from a binary anti-aliased image.

``
FastMarching --inFilename $1 --outFilename $2 --isoValue $3
`` 

* inFilename: The filename of the input image to be processed.
* outFilename: The filename of the output distance transform image.
* isoValue: The level set value that defines the interface between foreground and background.

### Topology Preserving Smoothing
This tool smooths the distance transforms while preserving the shape’s topology. This command line tool reads the parameters 
from a *xml* file. 

``
TopologyPreservingSmoothing parameterFile.xml
`` 

Here is the list of all parameters in the parameter file.

* inputs: The file names of the input distance transforms to be smoothed.
* dtFiles: The file names of the output smoothed distance transforms.
* outputs: The file names of the output smoothed iso-surface images.
* verbose: (default; 0) Show each intermediate step .
* isoValue: (default: 0.0) Isovalue to be used to define the surface in the input distance transform.
* smoothing iterations: (default: 10) Number of iterations to perform smoothing.
* alpha : (default: 10.5) Smoothing parameter in [I'](#equation/alpha).
* beta : (default: 10.0) Smoothing parameter in [I'](#equation/alpha).
* propagationScale : The PropagationScaling parameter can be used to switch from propagation outwards (POSITIVE) versus propagating
inwards (NEGATIVE).
alpha and beta are smoothing parameters in the following formula.

![](../img/workflow/alpha.png)

### Reflect Image
This tool reflects an image. It can be used when you want to align all pairs of anatomies together. For example, if you have left and right femurs and want to align them all, you would first reflect one side or the other so they are all either left or right femurs.

``
ReflectVolumes --inFilename $1 --outFilename $2 --centerFilename $3 --inputDirection $4 
``

* inFilename: The filename of the input image to be reflected.
* outFilename: The filename of the output reflected image.
* centerFilename: The filename where the image center about which reflection occured will be stored.
* inputDirection: Direction along which to reflect.

### Write Image Information
This tool will write information about an image to text. This can be useful if you have a segmented mesh that corresponds to the image which you would like to turn into a volume with the same parameters for example. It produces three text files: one with the x, y, and z origin, one with the x, y, and z size (or dimensions of the image), and one with the x, y, and z voxel spacing.

``
WriteImageInfoToText --inFilename" $1 --outPrefix $2
``

* inFilename: The filename of the input image.
* outPrefix: The prefix for the output files. Output files will be: prefix_origin.txt, prefix_size.txt, and prefix_spacing.txt.

### Clip Volume
This tool clips a binary volume using a defined cutting plane. It could be used to remove unwanted variance across samples. For example, if you have various femur segmentations with different shaft lengths you may want to clip these volumes so all the shaft lengths are close to the same.

``
ClipVolume parameterFile.xml
``

Here is the list of all parameters in the parameter file.

* num_shapes: Number of volumes you are clipping.
* inputs: The filenames of binary volumes to clip.
* outputs: The filenames of the output clipped volumes.
* cutting_planes: The planes with which to cut the volumes. Planes are defined by three points (pt1, pt2, and pt3) and the parameter should be formated in the following way: pt1_x pt1_y pt1_z pt2_x pt2_y pt2_z pt3_x pt3_y pt3_z.



## Mesh Pre-Processing

#### Reflect Mesh
This tool reflects a mesh segmentation. It can be used when you want to align all pairs of anatomies together. For example, if you have left and right femurs and want to align them all, you would first reflect one side or the other so they are all either left or right femurs.

``
ReflectMesh --inFilename $1 --outFilename $2 --reflectCenterFilename $3 --inputDirection $4 --meshFormat $5
``

* inFilename: The filename of the input mesh to be reflected.
* outFilename: The filename of the output reflected mesh.
* reflectCenterFilename: The filename where the image center about which reflection occured will be stored.
* inputDirection: Direction along which to reflect.
* meshFormat: Mesh file format such as "vtk" or "ply"

### Mesh to volume
This tool generates a binary volume and distance transform from a ".ply" mesh. If an image corresponding to the mesh exists, the origin, spacing, and size settings should be the same as this image. This information can be acquired using the WriteImageInfoToText tool.

``
TopologyPreservingSmoothing parameterFile.xml
`` 

Here is the list of all parameters in the parameter file.
* mesh: The file name of the input mesh.
* origin_x: The x origin of the output ".nrrd" files.
* origin_y: The y origin of the output ".nrrd" files.
* origin_z: The z origin of the output ".nrrd" files.
* size_x: The x size of the output ".nrrd" files.
* size_y: The y size of the output ".nrrd" files.
* size_z: The z size of the output ".nrrd" files.
* spacing_x: The x spacing of the output ".nrrd" files.
* spacing_y: The y spacing of the output ".nrrd" files.
* spacing_z: The z spacing of the output ".nrrd" files.

## Alignment 
