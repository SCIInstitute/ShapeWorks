



# ShapeWorks Commands


`shapeworks` is a single executable for ShapeWorks with a set of sub-executables (commands) that are flexible, modular, loosely coupled, and standardized subcommands, with interactive help to perform individual operations needed for a typical shape modeling workflow that includes the Groom, Optimize, and Analyze phases.

## shapeworks


**Usage:**

```
  shapeworks <command> [args]...
```  


**Description:** Unified ShapeWorks executable that includes command line utilities for automated construction of compact statistical landmark-based shape models of ensembles of shapes  


**Options:**

**-h, --help:** show this help message and exit

**--version:** show program's version number and exit

**-q, --quiet:** don't print status messages  
  
<a href="#top">Back to Top</a>

## Image Commands

### add


**Usage:**

```
shapeworks  add [args]...
```  


**Description:** add a value to each pixel in the given image and/or add another image in a pixelwise manner  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --value=DOUBLE:**  Value to add to each pixel.

**--name=STRING:** Name of image to add pixelwise.  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### antialias


**Usage:**

```
shapeworks  antialias [args]...
```  


**Description:** antialiases binary volumes  


**Options:**

**-h, --help:** show this help message and exit

**--maxrmserror=DOUBLE:** Maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default: 0.01].

**--iterations=INT:** Maximum number of iterations [default: 50].

**--layers=INT:** Number of layers around a 3d pixel to use for this computation [default: 3].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### binarize


**Usage:**

```
shapeworks  binarize [args]...
```  


**Description:** sets portion of image greater than min and less than or equal to max to the specified value  


**Options:**

**-h, --help:** show this help message and exit

**--min=DOUBLE:** Lower threshold level [default: 0.0].

**--max=DOUBLE:** Upper threshold level [default: inf ].

**--value=DOUBLE:** Value to set region [default: 1.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### blur


**Usage:**

```
shapeworks  blur [args]...
```  


**Description:** applies gaussian blur  


**Options:**

**-h, --help:** show this help message and exit

**--sigma=DOUBLE:** Value of sigma [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### bounding-box


**Usage:**

```
shapeworks  bounding-box [args]...
```  


**Description:** compute largest bounding box surrounding the specified isovalue of the specified set of images  


**Options:**

**-h, --help:** show this help message and exit

**--names <list of strings>:**  Paths to images (must be followed by `--`), ex: "bounding-box --names *.nrrd -- --isovalue 1.5")

**--padding=INT:** Number of extra voxels in each direction to pad the largest bounding box [default: 0].

**--isovalue=DOUBLE:** Threshold value [default: 1.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### clip


**Usage:**

```
shapeworks  clip [args]...
```  


**Description:** clips volume with the specified cutting planes defined by three 3D points  


**Options:**

**-h, --help:** show this help message and exit

**--x1=DOUBLE:** Value of x1 for cutting plane [default: 0.0].

**--y1=DOUBLE:** Value of y1 for cutting plane [default: 0.0].

**--z1=DOUBLE:** Value of z1 for cutting plane [default: 0.0].

**--x2=DOUBLE:** Value of x2 for cutting plane [default: 0.0].

**--y2=DOUBLE:** Value of y2 for cutting plane [default: 0.0].

**--z2=DOUBLE:** Value of z2 for cutting plane [default: 0.0].

**--x3=DOUBLE:** Value of x3 for cutting plane [default: 0.0].

**--y3=DOUBLE:** Value of y3 for cutting plane [default: 0.0].

**--z3=DOUBLE:** Value of z3 for cutting plane [default: 0.0].

**--value=DOUBLE:** Value of clipped pixels [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### close-holes


**Usage:**

```
shapeworks  close-holes [args]...
```  


**Description:** closes holes in a volume defined by values larger than specified value  


**Options:**

**-h, --help:** show this help message and exit

**--value=DOUBLE:** Largest value not in volume [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### compare


**Usage:**

```
shapeworks  compare [args]...
```  


**Description:** compare two images  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Compare this image with another.

**--verifyall=BOOL:** Also verify origin, spacing, and direction matches [default: true].

**--tolerance=DOUBLE:** Allowed percentage of pixel differences [default: 0.0].

**--precision=DOUBLE:** Allowed difference between two pixels for them to still be considered equal [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### compute-dt


**Usage:**

```
shapeworks  compute-dt [args]...
```  


**Description:** computes signed distance transform volume from an image at the specified isovalue  


**Options:**

**-h, --help:** show this help message and exit

**--isovalue=DOUBLE:** Level set value that defines the interface between foreground and background [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### crop


**Usage:**

```
shapeworks  crop [args]...
```  


**Description:** crop image down to the current region (e.g., from bounding-box command), or the specified min/max in each direction [default: image dimensions]  


**Options:**

**-h, --help:** show this help message and exit

**--xmin=UNSIGNED:** Minimum X.

**--xmax=UNSIGNED:** Maximum X.

**--ymin=UNSIGNED:** Minimum Y.

**--ymax=UNSIGNED:** Maximum Y.

**--zmin=UNSIGNED:** Minimum Z.

**--zmax=UNSIGNED:** Maximum Z.  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### curvature


**Usage:**

```
shapeworks  curvature [args]...
```  


**Description:** denoises an image using curvature driven flow using curvature flow image filter  


**Options:**

**-h, --help:** show this help message and exit

**--iterations=INT:** Number of iterations [default: 10].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### divide


**Usage:**

```
shapeworks  divide [args]...
```  


**Description:** divide an image by a constant  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --value=DOUBLE:**  Value with which to divide.  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### extract-label


**Usage:**

```
shapeworks  extract-label [args]...
```  


**Description:** extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image  


**Options:**

**-h, --help:** show this help message and exit

**--label=DOUBLE:** Label value to be extracted [default: 1.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### gradient


**Usage:**

```
shapeworks  gradient [args]...
```  


**Description:** computes gradient magnitude of an image region at each pixel using gradient magnitude filter  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### icp


**Usage:**

```
shapeworks  icp [args]...
```  


**Description:** transform current image using iterative closest point (ICP) 3D rigid registration computed from source to target distance maps  


**Options:**

**-h, --help:** show this help message and exit

**--source=STRING:** Distance map of source image.

**--target=STRING:** Distance map of target image.

**--isovalue=DOUBLE:** Isovalue of distance maps used to create ICPtransform [default: 0.0].

**--iterations=UNSIGNED:**  Number of iterations run ICP registration [default: 20].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### image-to-mesh


**Usage:**

```
shapeworks  image-to-mesh [args]...
```  


**Description:** converts the current image to a mesh  


**Options:**

**-h, --help:** show this help message and exit

**-v DOUBLE, --isovalue=DOUBLE:**  Isovalue to determine mesh boundary [default: 1.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### info


**Usage:**

```
shapeworks  info [args]...
```  


**Description:** prints requested image dimensions, spacing, size, origin, direction (coordinate system), center, center of mass and bounding box [default: prints everything]  


**Options:**

**-h, --help:** show this help message and exit

**--dims:** Whether to display image dimensions

**--spacing:** Whether to display physical spacing

**--size:** Whether to display size

**--origin:** Whether to display physical origin

**--direction:** Whether to display direction

**--center:** Whether to display center

**--centerofmass:** Whether to display center of mass

**--boundingbox:** Whether to display bounding box  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### multiply


**Usage:**

```
shapeworks  multiply [args]...
```  


**Description:** multiply an image by a constant  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --value=DOUBLE:**  Value with which to multiply.  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### negate


**Usage:**

```
shapeworks  negate [args]...
```  


**Description:** negate the values in the given image  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### pad


**Usage:**

```
shapeworks  pad [args]...
```  


**Description:** pads an image with specified value by specified number of voxels in the x-, y-, and/or z- directions; origin remains at the same location (note: negative padding to shrink an image is permitted)  


**Options:**

**-h, --help:** show this help message and exit

**--padding=INT:** Pad this many voxels in ALL directions (used if set) [default: 0].

**-x INT, --padx=INT:** Pad this many voxels in the x-direction [default: 0].

**-y INT, --pady=INT:** Pad this many voxels in the y-direction [default: 0].

**-z INT, --padz=INT:** Pad this many voxels in the z-direction [default: 0].

**--value=DOUBLE:** Value used to fill padded voxels [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### read-image


**Usage:**

```
shapeworks  read-image [args]...
```  


**Description:** reads an image  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of file to read  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### recenter


**Usage:**

```
shapeworks  recenter [args]...
```  


**Description:** recenters an image by changing its origin in the image header to the physical coordinates of the center of the image  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### reflect


**Usage:**

```
shapeworks  reflect [args]...
```  


**Description:** reflect image with respect to logical image center and the specified axis  


**Options:**

**-h, --help:** show this help message and exit

**--axis=STRING:** Axis along which to reflect (X, Y, or Z).  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### resample


**Usage:**

```
shapeworks  resample [args]...
```  


**Description:** resamples an image using new physical spacing (computes new dims)  


**Options:**

**-h, --help:** show this help message and exit

**--isospacing=DOUBLE:** Use this spacing in all dimensions.

**--spacex=DOUBLE:** Pixel spacing in x-direction [default: 1].

**--spacey=DOUBLE:** Pixel spacing in y-direction [default: 1].

**--spacez=DOUBLE:** Pixel spacing in z-direction [default: 1].

**--sizex=UNSIGNED:** Output size in x-direction [default: current size].

**--sizey=UNSIGNED:** Output size in y-direction [default: current size].

**--sizez=UNSIGNED:** Output size in z-direction [default: current size].

**--originx=DOUBLE:** Output origin in x-direction [default: current origin].

**--originy=DOUBLE:** Output origin in y-direction [default: current origin].

**--originz=DOUBLE:** Output origin in z-direction [default: current origin].

**--interp=CHOICE:** Interpolation method to use [default: linear]. (choose from 'linear', 'nearest')  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### resize


**Usage:**

```
shapeworks  resize [args]...
```  


**Description:** resizes an image (computes new physical spacing)  


**Options:**

**-h, --help:** show this help message and exit

**-x UNSIGNED, --sizex=UNSIGNED:**  Output size in x-direction [default: current size].

**-y UNSIGNED, --sizey=UNSIGNED:**  Output size in y-direction [default: current size].

**-z UNSIGNED, --sizez=UNSIGNED:**  Output size in z-direction [default: current size].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### rotate


**Usage:**

```
shapeworks  rotate [args]...
```  


**Description:** rotates image by specified value  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --rx=DOUBLE:**  Physical axis around which to rotate [default: z-axis]

**-y DOUBLE, --ry=DOUBLE:**  Physical axis around which to rotate [default: z-axis]

**-z DOUBLE, --rz=DOUBLE:**  Physical axis around which to rotate [default: z-axis]

**--radians=DOUBLE:** Angle in radians

**--degrees=DOUBLE:** Angle in degrees  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### scale


**Usage:**

```
shapeworks  scale [args]...
```  


**Description:** scales image by specified value  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --sx=DOUBLE:**  X scale

**-y DOUBLE, --sy=DOUBLE:**  Y scale

**-z DOUBLE, --sz=DOUBLE:**  Z scale  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### set-origin


**Usage:**

```
shapeworks  set-origin [args]...
```  


**Description:** set origin  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --x=DOUBLE:**  x value of origin [default: 0.0].

**-y DOUBLE, --y=DOUBLE:**  y value of origin [default: 0.0].

**-z DOUBLE, --z=DOUBLE:**  z value of origin [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### sigmoid


**Usage:**

```
shapeworks  sigmoid [args]...
```  


**Description:** computes sigmoid function pixel-wise using sigmoid image filter  


**Options:**

**-h, --help:** show this help message and exit

**--alpha=DOUBLE:** Value of alpha [default: 10.0].

**--beta=DOUBLE:** Value of beta [default: 10.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### subtract


**Usage:**

```
shapeworks  subtract [args]...
```  


**Description:** subtract a value from each pixel in this image and/or subtract another image in a pixelwise manner  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --value=DOUBLE:**  Value to subtract from each pixel.

**--name=STRING:** Name of image to subtract pixelwise.  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### topo-preserving-smooth


**Usage:**

```
shapeworks  topo-preserving-smooth [args]...
```  


**Description:** Helper command that applies gradient and sigmoid filters to create a feature image for the TPLevelSet filter; note that a curvature flow filter is sometimes applied to the image before this  


**Options:**

**-h, --help:** show this help message and exit

**--scaling=DOUBLE:** Scale for TPLevelSet level set filter [default: 20.0].

**--alpha=DOUBLE:** Value of alpha for sigmoid fitler [default: 10.0].

**--beta=DOUBLE:** Value of beta for sigmoid fitler [default: 10.0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### tp-levelset


**Usage:**

```
shapeworks  tp-levelset [args]...
```  


**Description:** segments structures in image using topology preserving geodesic active contour level set filter  


**Options:**

**-h, --help:** show this help message and exit

**--featureimage=STRING:**  Path of feature image for filter

**--scaling=DOUBLE:** Value of scale [default: 20.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### translate


**Usage:**

```
shapeworks  translate [args]...
```  


**Description:** translates image by specified physical (image space) distance  


**Options:**

**-h, --help:** show this help message and exit

**--centerofmass:** Use center of mass [default: false].

**-x DOUBLE, --tx=DOUBLE:**  X distance

**-y DOUBLE, --ty=DOUBLE:**  Y distance

**-z DOUBLE, --tz=DOUBLE:**  Z distance  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### warp-image


**Usage:**

```
shapeworks  warp-image [args]...
```  


**Description:** finds the warp between the source and target landmarks and transforms image by this warp  


**Options:**

**-h, --help:** show this help message and exit

**--source=STRING:** Path to source landmarks.

**--target=STRING:** Path to target landmarks.

**--stride=INT:** Every _stride_ points will be used for warping [default: 1].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### write-image


**Usage:**

```
shapeworks  write-image [args]...
```  


**Description:** writes the current image (determines type by its extension)  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of file to write

**--compressed=BOOL:** Whether to compress file [default: true]  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
## Mesh Commands

### coverage


**Usage:**

```
shapeworks  coverage [args]...
```  


**Description:** creates mesh of coverage between two meshes  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Path to other mesh with which to create coverage.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### read-mesh


**Usage:**

```
shapeworks  read-mesh [args]...
```  


**Description:** reads a mesh  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** name of file to read  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### write-mesh


**Usage:**

```
shapeworks  write-mesh [args]...
```  


**Description:** writes the current mesh (determines type by its extension)  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** name of file to write  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
## ParticleSystem Commands

### compactness


**Usage:**

```
shapeworks  compactness [args]...
```  


**Description:** Compute compactness of a loaded particle system  


**Options:**

**-h, --help:** show this help message and exit

**--nmodes=INT:** Number of modes to use

**--saveto=STRING:** Save the scree plots for all modes to a file  
  
<a href="#top">Back to Top</a>
  
[Back to ParticleSystem Commands](#particlesystem-commands)
### generalization


**Usage:**

```
shapeworks  generalization [args]...
```  


**Description:** compute generalization of a loaded particle system  


**Options:**

**-h, --help:** show this help message and exit

**--nmodes=INT:** Number of modes to use

**--saveto=STRING:** Save the reconstructions sorted by generalization along with the mapping to the original shape  
  
<a href="#top">Back to Top</a>
  
[Back to ParticleSystem Commands](#particlesystem-commands)
### read-particle-system


**Usage:**

```
shapeworks  read-particle-system [args]...
```  


**Description:** reads a particle system  


**Options:**

**-h, --help:** show this help message and exit

**--names <list of strings>:**  paths to .particle files (must be followed by `--`), ex: "--names *.particle -- next-command...")  
  
<a href="#top">Back to Top</a>
  
[Back to ParticleSystem Commands](#particlesystem-commands)
### specificity


**Usage:**

```
shapeworks  specificity [args]...
```  


**Description:** compute specificity of a loaded particle system  


**Options:**

**-h, --help:** show this help message and exit

**--nmodes=INT:** Number of modes to use

**--saveto=STRING:** Save the reconstructions sorted by specificity along with the mapping to the original shape  
  
<a href="#top">Back to Top</a>
  
[Back to ParticleSystem Commands](#particlesystem-commands)