# How to Groom Your Dataset?

ShapeWorks needs suitable distance transforms for establishing shape correspondence. The groom stage has the pipeline to generate the distance transforms from input images (binary segmentation or mesh).  It consists of image/mesh pre-processing and alignment tools.

## Image Pre-Processing 

### Antialias
This tool antialiases binary volumes.

``
shapeworks readimage --name $1 antialias maxrmserror $2 iterations $3 layers $4 writeimage --name $5
``

Here is the list of arguments.

* maxrmserror: Maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default: 0.01].
* iterations: Maximum number of iterations [default: 50].
* layers: Number of layers around a 3d pixel to use for this computation [default: 3].

### Resample
This tool uses an interpolation process to resample anisotropic voxels into isotropic-sized voxels.

``
shapeworks readimage --name $1 resample --isospacing $2 --spacex $3 --spacey $4 --spacez $5 --sizex $6 --sizey $7 --sizez $8 writeimage --name $9
``

Here is the list of arguments.

* isospacing: The isotropic spacing in all dimensions.
* spacex: Pixel spacing in x-direction [default: 1.0].
* spacey: Pixel spacing in y-direction [default: 1.0].
* spacez: Pixel spacing in z-direction [default: 1.0].
* sizex: Output size in x-direction [default: calculated using current size and desired spacing].
* sizey: Output size in y-direction [default: calculated using current size and desired spacing].
* sizez: Output size in z-direction [default: calculated using current size and desired spacing].

### Recenter
This tool recenters an image by changing its origin in the image header to the physical coordinates of the center of the image.

``
shapeworks readimage --name $1 recenter writeimage --name $2
``

### Pad

This tool pads pads an image with specified value by specified number of voxels in the x-, y-, and/or z- directions; origin remains at the same location (note: negative padding to shrink an image is permitted).

``
shapeworks readimage --name $1 pad --padding $2 --value $3 writeimage --name $4
``

``
shapeworks readimage --name $1 pad --padx $2 --pady $3 --padz $4 writeimage --name $5
``

Here is the list of arguments.

* padding: Pad this many voxels in ALL directions (used if set) [default: 0].
* padx: Pad this many voxels in the x-direction [default: 0].
* pady: Pad this many voxels in the y-direction [default: 0].
* padz: Pad this many voxels in the z-direction [default: 0].
* value: Value used to fill padded voxels [default: 0.0].

### Translate

This tool translates image by specified physical (image space) distance.

``
shapeworks readimage --name $1 translate --centerofmass writeimage --name $2
``

``
shapeworks readimage --name $1 translate --tx $2 --ty $3 --tz $4 writeimage --name $5
``

Here is the list of arguments.

* centerofmass: Use center of mass [default: false].
* tx: X distance
* ty: Y distance
* tz: Z distance

### Scale

This tool scales image by specified value.

``
shapeworks readimage --name $1 scale --sx $2 --sy $3 --sz $4 writeimage --name $5
``

Here is the list of arguments.

* tx: X scale
* ty: Y scale
* tz: Z scale

### Rotate

This tool rotates image by specified value.

``
shapeworks readimage --name $1 rotate --rx $2 --radians $3 writeimage --name $4
``

``
shapeworks readimage --name $1 rotate --degrees $2 writeimage --name $3
``

Here is the list of arguments.

* rx: Physical axis around which to rotate [default: z-axis]
* ry: Physical axis around which to rotate [default: z-axis]
* rz: Physical axis around which to rotate [default: z-axis]
* radians: Angle in radians
* degrees: Angle in degrees

### Extract Label

This tool extracts/isolates the specific voxel label from a given multi-label volume and outputs the corresponding binary image.

``
shapeworks readimage --name $1 extractlabel label $2 writeimage --name $3
``

Here is the list of arguments.

* label: Label value to be extracted [default: 1.0].

### Close Holes

This tool closes holes in a volume defined by values larger than specified value.

``
shapeworks readimage --name $1 closeholes writeimage --name $2
``

### Binarize

This tool sets portion of image greater than min and less than or equal to max to the specified value. 

``
shapeworks readimage --name $1 binarize --min $2 --max $3 --value $4 writeimage --name $5
``

Here is the list of arguments.

* min: Lower threshold level [default: 0.0].
* max: Upper threshold level [default: inf ]. 
* value: Value to set region [default: 1.0].

### Compute DT

This tool computes signed distance transform volume from an image at the specified isovalue.

``
shapeworks readimage --name $1 computedt --isovalue $2 writeimage --name $3
`` 

Here is the list of arguments.

* isovalue: Level set value that defines the interface between foreground and background [default: 0.0].

### Curvature

This tool denoises an image using curvature driven flow using curvature flow image filter.

``
shapeworks readimage --name $1 curvature --iterations $2 writeimage --name $3
`` 

Here is the list of arguments.

* iterations: Number of iterations [default: 10].

### Gradient

This tool computes gradient magnitude of an image region at each pixel using gradient magnitude filter.

``
shapeworks readimage --name $1 curvature --iterations $2 writeimage --name $3
`` 

Here is the list of arguments.

* iterations: Number of iterations [default: 10].

### Sigmoid

This tool computes sigmoid function pixel-wise using sigmoid image filter.

``
shapeworks readimage --name $1 sigmoid --alpha $2 --beta $3 writeimage --name $4
`` 

Here is the list of arguments.

* alpha: Value of alpha [default: 10.0].
* beta: Value of beta [default: 10.0].

### TPLevelSetFilter

This tool segments structures in image using topology preserving geodesic active contour level set filter.

``
shapeworks readimage --name $1 tplevelset --featureimage $2 --scaling $3 writeimage --name $4
`` 

Here is the list of arguments.

* featureimage: Path of feature image for filter
* scaling: Value of scale [default: 20.0].

### TopologyPreservingFilter

This tool is a helper command that applies gradient and sigmoid filters to create a feature image for the TPLevelSet filter; note that the curvature flow filter is sometimes applied to the image before this.

``
shapeworks readimage --name $1 topopreservingsmooth --scaling $2 --alpha $3 --beta $4 writeimage --name $5
`` 

Here is the list of arguments.

* scaling: Scale for TPLevelSet level set filter [default: 20.0].
* alpha: Value of alpha for sigmoid fitler [default: 10.0].
* beta: Value of beta for sigmoid fitler [default: 10.0.0].

### Blur

This tool applies gaussian blur.

``
shapeworks readimage --name $1 blur --sigma $2 writeimage --name $3
`` 

Here is the list of arguments.

* sigma: Value of sigma [default: 0.0].

### ICPRigid

This tool transform current image using iterative closest point (ICP) 3D rigid registration computed from source to target distance maps.

``
shapeworks readimage --name $1 icp --source $2 --target $3 --isovalue $4 --iterations $5 writeimage --name $6
`` 

Here is the list of arguments.

* source: Distance map of source image.
* target: Distance map of target image.
* isovalue: Isovalue of distance maps used to create ICPtransform [default: 0.0].
* iterations: Number of iterations run ICP registration [default: 20].

### Bounding Box

This tool computes the largest bounding box for the shape population needed for cropping all the images.

``
shapeworks readimage --name $1 boundingbox --names $2 -- --padding $3 --isovalue $4 writeimage --name $5
``

Here is the list of arguments.

* names: Paths to images (must be followed by `--`), ex: \"bounding-box --names *.nrrd -- --isovalue 1.5\").
* padding: Number of extra voxels in each direction to pad the largest bounding box [default: 0].
* isovalue: Threshold value [default: 1.0].

### Crop

This tool crops image down to the current region (e.g., from bounding-box command), or the specified min/max in each direction [default: image dimensions].

``
shapeworks readimage --name $1 crop --xmin $2 --xmax $3 --ymin $4 --ymax $5 --zmin $6 --zmax $7 writeimage --name $8
``

Here is the list of arguments.

* xmin: Minimum X.
* xmax: Maximum X.
* ymin: Minimum Y.
* ymax: Maximum Y.
* zmin: Minimum Z.
* zmax: Maximum Z.

### Clip Volume
This tool clips volume with the specified cutting planes defined by three 3D points

``
shapeworks readimage --name $1 clip --x1 $2 --y1 $3 --z1 $4 --x2 $5 --y2 $6 --z2 $7 --x3 $8 --y3 $9 --z3 $10 writeimage --name $11
``

Here is the list of arguments.

* x1: Value of x1 for cutting plane [default: 0.0].
* y1: Value of y1 for cutting plane [default: 0.0].
* z1: Value of z1 for cutting plane [default: 0.0].
* x2: Value of x2 for cutting plane [default: 0.0].
* y2: Value of y2 for cutting plane [default: 0.0].
* z2: Value of z2 for cutting plane [default: 0.0].
* x3: Value of x3 for cutting plane [default: 0.0].
* y3: Value of y3 for cutting plane [default: 0.0].
* z3: Value of z3 for cutting plane [default: 0.0].

### Reflect Images

This tool reflects image with respect to logical image center and the specified axis.

``
shapeworks readimage --name $1 reflect --axis $2 writeimage --name $3
``

Here is the list of arguments.

* axis: Axis along which to reflect (X, Y, or Z).

### Set Origin

This tool sets origin.

``
shapeworks readimage --name $1 setorigin --x $2 --y $3 --z $4 writeimage --name $5
``

Here is the list of arguments.

* x: x value of origin [default: 0.0].
* y: y value of origin [default: 0.0].
* z: z value of origin [default: 0.0].

### Warp

This tool finds the warp between the source and target landmarks and transforms image by this warp.

``
shapeworks readimage --name $1 warpimage --source $2 --target $3 --landmarks $4 writeimage --name $5
``

Here is the list of arguments.

* source: Path to source landmarks.
* target: Path to target landmarks.
* landmarks: Every _stride_ points will be used for warping [default: 1].

### Compare

This tool compares two images.

``
shapeworks readimage --name $1 compare --name $2 --verifyall $4 --tolerance $4 --precision $5 writeimage --name $6
``

Here is the list of arguments.

* name: Compare this image with another
* verifyall: Also verify origin, spacing, and direction matches [default: true].
* tolerance: Allowed percentage of pixel differences [default: 0.0].
* precision: Allowed difference between two pixels for them to still be considered equal [default: 0.0].

### Negate

This tool negates the values in the given image.

``
shapeworks readimage --name $1 negate writeimage --name $2
``

### Add

This tool adds a value to each pixel in the given image and/or add another image in a pixelwise manner.

``
shapeworks readimage --name $1 add --value $2 --name $3 writeimage --name $4
``

Here is the list of arguments.

* value: Value to add to each pixel.
* name: Name of image to add pixelwise.

### Subtract

This tool subtracts a value from each pixel in the given image and/or subtract another image in a pixelwise manner.

``
shapeworks readimage --name $1 subtract --value $2 --name $3 writeimage --name $4
``

Here is the list of arguments.

* value: Value to add to each pixel.
* name: Name of image to subtract pixelwise.

### Multiply

This tool multiplies an image by a constant.

``
shapeworks readimage --name $1 multiply --value $2 writeimage --name $3
``

Here is the list of arguments.

* value: Value with which to multiply.

### Divide

This tool divides an image by a constant.

``
shapeworks readimage --name $1 divide --value $2 writeimage --name $3
``

Here is the list of arguments.

* value: Value with which to divide.

### ImageToMesh

This tool converts the current image to a mesh.

Here is the list of arguments.

* isovalue: Isovalue to determine mesh boundary [default: 1.0].

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
