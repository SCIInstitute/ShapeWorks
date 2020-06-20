
ShapeWorks Commands
===================

Contents
========

* [shapeworks](#shapeworks)
* [Image Commands](#image-commands)
	* [antialias](#antialias)
	* [blur](#blur)
	* [bounding-box](#bounding-box)
	* [clip](#clip)
	* [close-holes](#close-holes)
	* [compare](#compare)
	* [compute-dt](#compute-dt)
	* [crop](#crop)
	* [curvature](#curvature)
	* [extract-label](#extract-label)
	* [filter](#filter)
	* [gradient](#gradient)
	* [icp](#icp)
	* [info](#info)
	* [pad](#pad)
	* [read-image](#read-image)
	* [recenter](#recenter)
	* [reflect](#reflect)
	* [resample](#resample)
	* [rotate](#rotate)
	* [scale](#scale)
	* [set-origin](#set-origin)
	* [sigmoid](#sigmoid)
	* [threshold](#threshold)
	* [topo-preserving-smooth](#topo-preserving-smooth)
	* [tp-levelset](#tp-levelset)
	* [translate](#translate)
	* [warp-image](#warp-image)
	* [write-image](#write-image)
* [Mesh Commands](#mesh-commands)
	* [coverage](#coverage)
	* [read-mesh](#read-mesh)
	* [write-mesh](#write-mesh)
* [ParticleSystem Commands](#particlesystem-commands)
	* [compactness](#compactness)
	* [generalization](#generalization)
	* [read-particle-system](#read-particle-system)
	* [specificity](#specificity)

# shapeworks


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

# Image Commands

## antialias


**Usage:**

```
shapeworks  antialias [args]...
```  


**Description:** antialiases binary volumes  


**Options:**

**-h, --help:** show this help message and exit

**--maxrmserror=FLOAT:** Maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default 0.01].

**--iterations=INT:** Number of iterations [default 50].

**--layers=INT:** Number of layers around a 3d pixel to use for this computation [default image dims].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## blur


**Usage:**

```
shapeworks  blur [args]...
```  


**Description:** applies gaussian blur  


**Options:**

**-h, --help:** show this help message and exit

**--sigma=DOUBLE:** Value of sigma [default 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## bounding-box


**Usage:**

```
shapeworks  bounding-box [args]...
```  


**Description:** compute largest bounding box size of the given set of binary images  


**Options:**

**-h, --help:** show this help message and exit --names <list of strings> Paths to images

**--padding=INT:** Number of extra voxels in each direction to pad the largest bounding box [default 0].

**--isovalue=FLOAT:** Threshold value [default 1.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## clip


**Usage:**

```
shapeworks  clip [args]...
```  


**Description:** chops volume with corresponding cutting planes  


**Options:**

**-h, --help:** show this help message and exit

**--x1=DOUBLE:** Value of x1 for cutting plane [default 0.0].

**--y1=DOUBLE:** Value of y1 for cutting plane [default 0.0].

**--z1=DOUBLE:** Value of z1 for cutting plane [default 0.0].

**--x2=DOUBLE:** Value of x2 for cutting plane [default 0.0].

**--y2=DOUBLE:** Value of y2 for cutting plane [default 0.0].

**--z2=DOUBLE:** Value of z2 for cutting plane [default 0.0].

**--x3=DOUBLE:** Value of x3 for cutting plane [default 0.0].

**--y3=DOUBLE:** Value of y3 for cutting plane [default 0.0].

**--z3=DOUBLE:** Value of z3 for cutting plane [default 0.0].

**--val=DOUBLE:** Value of clipped pixels [default 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## close-holes


**Usage:**

```
shapeworks  close-holes [args]...
```  


**Description:** closes holes in a given binary volume  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## compare


**Usage:**

```
shapeworks  compare [args]...
```  


**Description:** compare two images  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of image with which to compare

**--precision=DOUBLE:** Allowed pixel difference to still be considered equal (default: none)

**--verifyall=BOOL:** Verify origin, spacing, and direction of both images match (default: true)  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## compute-dt


**Usage:**

```
shapeworks  compute-dt [args]...
```  


**Description:** computes distance transform volume from a binary (antialiased) image  


**Options:**

**-h, --help:** show this help message and exit

**--isovalue=FLOAT:** Level set value that defines the interface between foreground and background [default 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## crop


**Usage:**

```
shapeworks  crop [args]...
```  


**Description:** performs translational alignment of shape image based on its center of mass or given 3D point  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## curvature


**Usage:**

```
shapeworks  curvature [args]...
```  


**Description:** denoises an image using curvature driven flow using curvature flow image filter  


**Options:**

**-h, --help:** show this help message and exit

**--iterations=INT:** Number of iterations [default 10].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## extract-label


**Usage:**

```
shapeworks  extract-label [args]...
```  


**Description:** extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image  


**Options:**

**-h, --help:** show this help message and exit

**--label=FLOAT:** Label value which has to be extracted [default 1.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## filter


**Usage:**

```
shapeworks  filter [args]...
```  


**Description:** apply the specified filter (curvature, gradient, sigmoid, tplevelset, gaussian, antialias)  


**Options:**

**-h, --help:** show this help message and exit

**--type=STRING:** filter type: curvature, gradient, sigmoid, tplevelset, gaussian, antialias --iterations=UNSIGNED Number of iterations [default 10].

**--alpha=DOUBLE:** Value of alpha [default 10.0].

**--beta=DOUBLE:** Value of beta [default 10.0]. --featureimage=STRING Path of feature image for tplevelset filter

**--scaling=DOUBLE:** Value of scale [default 20]

**--sigma=DOUBLE:** Value of sigma [default 0.0].

**--maxrmserror=FLOAT:** Maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default 0.01].

**--layers=INT:** Number of layers around a 3d pixel to use for this computation [default image dims].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## gradient


**Usage:**

```
shapeworks  gradient [args]...
```  


**Description:** computes gradient magnitude of an image region at each pixel using gradient magnitude filter  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## icp


**Usage:**

```
shapeworks  icp [args]...
```  


**Description:** performs iterative closed point (ICP) 3D rigid registration on pair of images  


**Options:**

**-h, --help:** show this help message and exit

**--target=STRING:** Distance map of target image.

**--source=STRING:** Distance map of source image.

**--isovalue=FLOAT:** Value of isovalue [default 0.0]. --iterations=UNSIGNED Number of iterations run ICP registration [default 20].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## info


**Usage:**

```
shapeworks  info [args]...
```  


**Description:** prints logical dims, spacing, size, origin, direction (coordinate system), center, center of mass and bounding box  


**Options:**

**-h, --help:** show this help message and exit

**--dims=BOOL:** Whether to display logical dims [default is false]

**--spacing=BOOL:** Whether to display physical spacing [default is false]

**--size=BOOL:** Whether to display size [default is false]

**--origin=BOOL:** Whether to display physical origin [default is false]

**--direction=BOOL:** Whether to display direction [default is false]

**--center=BOOL:** Whether to display center [default is false]

**--centerofmass=BOOL:** Whether to display center of mass [default is false]

**--boundingbox=BOOL:** Whether to display bounding box [default is false]  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## pad


**Usage:**

```
shapeworks  pad [args]...
```  


**Description:** pads an image with a contant value in the x-, y-, and z- directions  


**Options:**

**-h, --help:** show this help message and exit

**--padding=INT:** Number of voxels to be padded in each direction [default 0].

**--value=FLOAT:** Value to be used to fill padded voxels [default 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## read-image


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
<a href="#top">Back to Top</a>
## recenter


**Usage:**

```
shapeworks  recenter [args]...
```  


**Description:** recenters an image by changing its origin in the image header to the physical coordinates of the center of the image  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## reflect


**Usage:**

```
shapeworks  reflect [args]...
```  


**Description:** reflect images with respect to logical image center and the specified normal (ex: <1,0,0> reflects along X axis across YZ-plane).  


**Options:**

**-h, --help:** show this help message and exit -x DOUBLE, --nx=DOUBLE Value of x in normal [default 1]. -y DOUBLE, --ny=DOUBLE Value of y in normal [default 0]. -z DOUBLE, --nz=DOUBLE Value of z in normal [default 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## resample


**Usage:**

```
shapeworks  resample [args]...
```  


**Description:** resamples an image  


**Options:**

**-h, --help:** show this help message and exit

**--isospacing=DOUBLE:** Use this spacing in all dimensions.

**--spacex=DOUBLE:** Pixel spacing in x-direction [default 1.0].

**--spacey=DOUBLE:** Pixel spacing in y-direction [default 1.0].

**--spacez=DOUBLE:** Pixel spacing in z-direction [default 1.0].

**--sizex=UNSIGNED:** Image size in x-direction [default estimated from the image].

**--sizey=UNSIGNED:** Image size in y-direction [default estimated from the image].

**--sizez=UNSIGNED:** Image size in z-direction [default estimated from the image].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## rotate


**Usage:**

```
shapeworks  rotate [args]...
```  


**Description:** rotates images  


**Options:**

**-h, --help:** show this help message and exit -x DOUBLE, --rx=DOUBLE Physical axis around which to rotate (z-axis if unspecified) -y DOUBLE, --ry=DOUBLE Physical axis around which to rotate (z-axis if unspecified) -z DOUBLE, --rz=DOUBLE Physical axis around which to rotate (z-axis if unspecified)

**--radians=DOUBLE:** Angle in radians

**--degrees=DOUBLE:** Angle in degrees  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## scale


**Usage:**

```
shapeworks  scale [args]...
```  


**Description:** scales images  


**Options:**

**-h, --help:** show this help message and exit -x DOUBLE, --sx=DOUBLE x scale -y DOUBLE, --sy=DOUBLE y scale -z DOUBLE, --sz=DOUBLE z scale  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## set-origin


**Usage:**

```
shapeworks  set-origin [args]...
```  


**Description:** set origin  


**Options:**

**-h, --help:** show this help message and exit -x DOUBLE, --x=DOUBLE x value of origin [default 0]. -y DOUBLE, --y=DOUBLE y value of origin [default 0]. -z DOUBLE, --z=DOUBLE z value of origin [default 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## sigmoid


**Usage:**

```
shapeworks  sigmoid [args]...
```  


**Description:** computes sigmoid function pixel-wise using sigmoid image filter  


**Options:**

**-h, --help:** show this help message and exit

**--alpha=DOUBLE:** Value of alpha [default 10.0].

**--beta=DOUBLE:** Value of beta [default 10.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## threshold


**Usage:**

```
shapeworks  threshold [args]...
```  


**Description:** threholds image into binary label based on upper and lower intensity bounds given by user  


**Options:**

**-h, --help:** show this help message and exit

**--min=FLOAT:** Lower threshold level [default = epsilon].

**--max=FLOAT:** Upper threshold level [default = FLT_MAX].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## topo-preserving-smooth


**Usage:**

```
shapeworks  topo-preserving-smooth [args]...
```  


**Description:** Helper command that applies gradient and sigmoid filters to create a feature image for the TPLevelSet filter; note that a curvature flow filter is sometimes applied to the image before this.  


**Options:**

**-h, --help:** show this help message and exit

**--scaling=DOUBLE:** Scale for TPLevelSet level set filter [default 20].

**--alpha=DOUBLE:** Value of alpha for sigmoid fitler [default 10.0].

**--beta=DOUBLE:** Value of beta for sigmoid fitler [default 10.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## tp-levelset


**Usage:**

```
shapeworks  tp-levelset [args]...
```  


**Description:** segments structures in images using topology preserving geodesic active contour level set filter  


**Options:**

**-h, --help:** show this help message and exit --featureimage=STRING Path of feature image for filter

**--scaling=DOUBLE:** Value of scale [default 20].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## translate


**Usage:**

```
shapeworks  translate [args]...
```  


**Description:** translates images  


**Options:**

**-h, --help:** show this help message and exit

**--centerofmass=BOOL:** Use center of mass [default set to false]. --applycenterofmass=BOOL Apply calculated center of mass [default set to false]. -x DOUBLE, --tx=DOUBLE Explicit tx in image space (physical coordinates) -y DOUBLE, --ty=DOUBLE Explicit ty in image space (e.g., 3.14) -z DOUBLE, --tz=DOUBLE Explicit tz in image space  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## warp-image


**Usage:**

```
shapeworks  warp-image [args]...
```  


**Description:** warps an image from source to target based on landmark guidence  


**Options:**

**-h, --help:** show this help message and exit --source_landmarks=STRING Path to source landmarks. --target_landmarks=STRING Path to target landmarks.

**--stride=INT:** Every _stride_ points will be used for warping [default 1].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
## write-image


**Usage:**

```
shapeworks  write-image [args]...
```  


**Description:** writes the current image (determines type by its extension)  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of file to write

**--compressed=BOOL:** Whether to compress file [default is true]  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)  
<a href="#top">Back to Top</a>
# Mesh Commands

## coverage


**Usage:**

```
shapeworks  coverage [args]...
```  


**Description:** coverage between two meshes  


**Options:**

**-h, --help:** show this help message and exit

**--second_mesh=STRING:** Second mesh to apply coverage.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)  
<a href="#top">Back to Top</a>
## read-mesh


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
<a href="#top">Back to Top</a>
## write-mesh


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
<a href="#top">Back to Top</a>
# ParticleSystem Commands

## compactness


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
<a href="#top">Back to Top</a>
## generalization


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
<a href="#top">Back to Top</a>
## read-particle-system


**Usage:**

```
shapeworks  read-particle-system [args]...
```  


**Description:** reads a particle system  


**Options:**

**-h, --help:** show this help message and exit --names <list of strings> paths to .particle files  
  
<a href="#top">Back to Top</a>
  
[Back to ParticleSystem Commands](#particlesystem-commands)  
<a href="#top">Back to Top</a>
## specificity


**Usage:**

```
shapeworks  specificity [args]...
```  


**Description:** compute specificity of a loaded particle system  


**Options:**

**-h, --help:** show this help message and exit

**--nmodes=INT:** Number of modes to use

**--saveto=STRING:** Save the reconstructions sorted by generalization along with the mapping to the original shape  
  
<a href="#top">Back to Top</a>
  
[Back to ParticleSystem Commands](#particlesystem-commands)  
<a href="#top">Back to Top</a>