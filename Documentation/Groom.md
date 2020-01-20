## ShapeWorks Groom

ShapeWorks needs suitable distance transforms for establishing shape correspondence. Groom stage has a pipeline to 
generate the distance transforms from input images (binary segmentation or mesh).  It consists of image/mesh pre-processing and 
alignment tools.

### Image pre-Processing 

#### Isotropic Resampling
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
*isCenterImageOn: A flag to center the image, i.e. change the origin in the image header to the physcial coordinates
 of the first voxel (lower left corner).
* sizeX                    Image size in x-direction (optional, if set to 0, the size is autmatically estimated from the input image).
* sizeY                    Image size in y-direction (optional, if set to 0, the size is autmatically estimated from the input image).
* sizeZ                    Image size in z-direction (optional, if set to 0, the size is autmatically estimated from the input image).

##### Constant Padding

This command line tool pads the volume in x, y and z directions with a constant value.

``
PadVolumeWithConstant --inFilename $1 --outFilename $2 --paddingSize $3 --paddingValue $4
``
* inFilename: The filename of the input image to be padded.
* outFilename: The filename of the padded image.
* paddingSize: An integer for the number of voxels to be padded in each direction.
* paddingValue : A read value to be used to fill padded voxels.

#### Find Largest Bounding Box

This command line tool computes the largest bounding box for the shape population needed for cropping all the images.

``
 FindLargestBoundingBox --inFilename $1 --outPrefix $2 --paddingSize $3 
``

It uses the following input arguments.
* inFilename: The filename of the input image.
* outPrefix : Output prefix to be used to save the parameters for the estimated bounding box in a *txt* file.
* paddingSize : number of extra voxels in each direction to pad the largest bounding box, checks minimum image size is performed to
make sure that this padding won’t get out of bounds for the smallest image in the file names provides.

#### Cropping

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

#### Threshold Images
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

#### Reflect Images

A command line tool that reflect 3D volume images with respect to image center and specific axis.

``
ReflectVolumes --inFilename $1 --outFilename $2 --centerFilename $3 --inputDirection $4 
``

It uses the following input arguments:
* inFilename: Image file name which needs to be reflected.
* outFilename: Output file name for the reflected image
* inputDirection: Axis along which it needs to be reflected
* centerFilename: The filename where the image center information will be stored. 

#### Extract Given Label from Image

A command line tool that extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image.
This tool is used mostly to process the reference image for alignment.

``
ExtractGivenLabelImage  --inFilename $1 --outFilename $2 --labelVal $3
``
List of arguments are as follows.
* inFilename: The filename of the input image from which label has to be extracted.
* outFilename: The filename of the output binary image.
* labelVal: The label value which has to be extracted.

#### Close Holes

This tool closes possible holes in the binary images.

``
CloseHoles --inFilename $1 --outFilename $2
``

* inFilename: The filename of the input binary image.
* outFilename: The filename of the output binary image( usually the same os the inFilename).

#### Fast Marching
A tool that computes distance transform volume from a binary anti-aliased image.

``
FastMarching --inFilename $1 --outFilename $2 --isoValue $3
`` 

* inFilename: The filename of the input image to be processed.
* outFilename: The filename of the output distance transform image.
* isoValue: The level set value that defines the interface between foreground and background.

#### Topology Preserving Smoothing
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

<p align="center"><img src="images/alpha.png" /></p>



### Mesh pre-Processing 

### Alignment 