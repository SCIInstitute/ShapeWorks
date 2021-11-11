



# ShapeWorks Commands


`shapeworks` is a single executable for ShapeWorks with a set of sub-executables (commands) that are flexible, modular, loosely coupled, and standardized subcommands, with interactive help to perform individual operations needed for a typical shape modeling workflow that includes the Groom, Optimize, and Analyze phases.
!!! danger "Activate shapeworks environment"
	 Each time you use ShapeWorks from the command line, you must first activate its environment using the `conda activate shapeworks` command on the terminal. 
!!! danger "Add shapeworks to your path"
	 Please make sure that `shapeworks` is in your path. See [Adding to PATH Environment Variable](../dev/paths.md). 

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

## Groom Commands

### groom


**Usage:**

```
shapeworks  groom [args]...
```  


**Description:** groom a shapeworks project  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Path to parameter file.  
  
<a href="#top">Back to Top</a>
  
[Back to Groom Commands](#groom-commands)
## Image Commands

### add


**Usage:**

```
shapeworks  add [args]...
```  


**Description:** add a value to each pixel in the given image and/or add another image in a pixelwise manner  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --value=DOUBLE:**  Value to add to each pixel [default: 0.0].

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

**--iterations=INT:** Maximum number of iterations [default: 50].

**--maxrmserror=DOUBLE:** Maximum RMS error determines how fast the solver converges. Range [0.0, 1.0], larger is faster [default: 0.01].

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

**--value=DOUBLE:** Value to set region [default: 1].  
  
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

**--sigma=DOUBLE:** Value of sigma [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### bounding-box-image


**Usage:**

```
shapeworks  bounding-box-image [args]...
```  


**Description:** compute largest physical bounding box surrounding the specified isovalue of the specified set of images  


**Options:**

**-h, --help:** show this help message and exit

**--names <list of strings>:**  Paths to images (must be followed by `--`), ex: "bounding-box-image --names *.nrrd -- --isovalue 1.5")

**--isovalue=DOUBLE:** Threshold value [default: 1].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### clip-image


**Usage:**

```
shapeworks  clip-image [args]...
```  


**Description:** clips volume with the specified cutting planes defined by three 3D points  


**Options:**

**-h, --help:** show this help message and exit

**--x1=DOUBLE:** Value of x1 for cutting plane [default: 0].

**--y1=DOUBLE:** Value of y1 for cutting plane [default: 0].

**--z1=DOUBLE:** Value of z1 for cutting plane [default: 0].

**--x2=DOUBLE:** Value of x2 for cutting plane [default: 0].

**--y2=DOUBLE:** Value of y2 for cutting plane [default: 0].

**--z2=DOUBLE:** Value of z2 for cutting plane [default: 0].

**--x3=DOUBLE:** Value of x3 for cutting plane [default: 0].

**--y3=DOUBLE:** Value of y3 for cutting plane [default: 0].

**--z3=DOUBLE:** Value of z3 for cutting plane [default: 0].

**--value=DOUBLE:** Value of clipped pixels [default: 0].  
  
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

**--value=DOUBLE:** Largest value not in volume [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### compare-image


**Usage:**

```
shapeworks  compare-image [args]...
```  


**Description:** compare two images  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Compare this image with another.

**--verifyall=BOOL:** Also verify origin, spacing, and direction matches [default: true].

**--tolerance=DOUBLE:** Allowed percentage of pixel differences [default: 0].

**--precision=DOUBLE:** Allowed difference between two pixels for them to still be considered equal [default: 1e-12].  
  
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

**--isovalue=DOUBLE:** Level set value that defines the interface between foreground and background [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### crop


**Usage:**

```
shapeworks  crop [args]...
```  


**Description:** crop image down to the current region of physical space (from bounding-box or set-region commands)  


**Options:**

**-h, --help:** show this help message and exit  
  
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

**-x DOUBLE, --value=DOUBLE:**  Value with which to divide [default: 0.0].  
  
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

**--label=DOUBLE:** Label value to be extracted [default: 1].  
  
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
### icp-image


**Usage:**

```
shapeworks  icp-image [args]...
```  


**Description:** transform current image using iterative closest point (ICP) 3D rigid registration computed from current distance map to target distance map  


**Options:**

**-h, --help:** show this help message and exit

**--target=STRING:** Distance map of target image.

**--isovalue=DOUBLE:** Isovalue of distance maps used to create ICPtransform [default: 0].

**--iterations=UNSIGNED:**  Number of iterations run ICP registration [default: 20].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### image-bounds


**Usage:**

```
shapeworks  image-bounds [args]...
```  


**Description:** return bounds of image, optionally with an isovalue to restrict region  


**Options:**

**-h, --help:** show this help message and exit

**--isovalue=DOUBLE:** Isovalue [default: entire image].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### image-info


**Usage:**

```
shapeworks  image-info [args]...
```  


**Description:** prints requested image dimensions, spacing, size, origin, direction (coordinate system), center, center of mass and bounding box [default: prints everything]  


**Options:**

**-h, --help:** show this help message and exit

**--dims:** Whether to display image dimensions [default: true].

**--spacing:** Whether to display physical spacing [default: true].

**--size:** Whether to display size [default: true].

**--origin:** Whether to display physical origin [default: true].

**--direction:** Whether to display direction [default: true].

**--center:** Whether to display center. [default: true]

**--centerofmass:** Whether to display center of mass. [default: true]

**--boundingbox:** Whether to display bounding box. [default: true]  
  
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

**-v DOUBLE, --isovalue=DOUBLE:**  Isovalue to determine mesh boundary [default: 1].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### intensity


**Usage:**

```
shapeworks  intensity [args]...
```  


**Description:** applies intensity windowing image filter  


**Options:**

**-h, --help:** show this help message and exit

**--min=DOUBLE:** Minimum value of window [default: 0].

**--max=DOUBLE:** Maximum value of window [default: 0].  
  
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

**-x DOUBLE, --value=DOUBLE:**  Value with which to multiply [default: 1.0]  
  
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

**--value=DOUBLE:** Value used to fill padded voxels [default: 0].  
  
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

**--name=STRING:** Name of file to read.  
  
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
### reflect-image


**Usage:**

```
shapeworks  reflect-image [args]...
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

**-x DOUBLE, --rx=DOUBLE:**  Physical axis around which to rotate [default: z-axis].

**-y DOUBLE, --ry=DOUBLE:**  Physical axis around which to rotate [default: z-axis].

**-z DOUBLE, --rz=DOUBLE:**  Physical axis around which to rotate [default: z-axis].

**--radians=DOUBLE:** Angle in radians.

**--degrees=DOUBLE:** Angle in degrees.  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### scale-image


**Usage:**

```
shapeworks  scale-image [args]...
```  


**Description:** scales image by specified value  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --sx=DOUBLE:**  X scale.

**-y DOUBLE, --sy=DOUBLE:**  Y scale.

**-z DOUBLE, --sz=DOUBLE:**  Z scale.  
  
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

**-x DOUBLE, --x=DOUBLE:**  X value of origin [default: 0].

**-y DOUBLE, --y=DOUBLE:**  Y value of origin [default: 0].

**-z DOUBLE, --z=DOUBLE:**  Z value of origin [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### set-region


**Usage:**

```
shapeworks  set-region [args]...
```  


**Description:** set the current (physical) region to the specified min/max in each direction, for use with downstreams commands such as crop (note: could instead use the image-bounds command with an isovalue)  


**Options:**

**-h, --help:** show this help message and exit

**--xmin=DOUBLE:** Minimum X.

**--xmax=DOUBLE:** Maximum X.

**--ymin=DOUBLE:** Minimum Y.

**--ymax=DOUBLE:** Maximum Y.

**--zmin=DOUBLE:** Minimum Z.

**--zmax=DOUBLE:** Maximum Z.  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### set-spacing


**Usage:**

```
shapeworks  set-spacing [args]...
```  


**Description:** set spacing  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --x=DOUBLE:**  x value of spacing [default: 1].

**-y DOUBLE, --y=DOUBLE:**  y value of spacing [default: 1].

**-z DOUBLE, --z=DOUBLE:**  z value of spacing [default: 1].  
  
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

**--alpha=DOUBLE:** Value of alpha [default: 10].

**--beta=DOUBLE:** Value of beta [default: 10].  
  
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

**-x DOUBLE, --value=DOUBLE:**  Value to subtract from each pixel [default: 0.0].

**--name=STRING:** Name of image to subtract pixelwise.  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### topo-preserving-smooth


**Usage:**

```
shapeworks  topo-preserving-smooth [args]...
```  


**Description:** helper command that applies gradient and sigmoid filters to create a feature image for the TPLevelSet filter; note that a curvature flow filter is sometimes applied to the image before this  


**Options:**

**-h, --help:** show this help message and exit

**--scaling=DOUBLE:** Scale for TPLevelSet level set filter [default: 20].

**--alpha=DOUBLE:** Value of alpha for sigmoid fitler [default: 10].

**--beta=DOUBLE:** Value of beta for sigmoid fitler [default: 10].  
  
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

**--scaling=DOUBLE:** Value of scale [default: 20].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
### translate-image


**Usage:**

```
shapeworks  translate-image [args]...
```  


**Description:** translates image by specified physical (image space) distance  


**Options:**

**-h, --help:** show this help message and exit

**--centerofmass:** Use center of mass [default: false].

**-x DOUBLE, --tx=DOUBLE:**  X distance.

**-y DOUBLE, --ty=DOUBLE:**  Y distance.

**-z DOUBLE, --tz=DOUBLE:**  Z distance.  
  
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

**--name=STRING:** Name of file to write.

**--compressed=BOOL:** Whether to compress file [default: true].  
  
<a href="#top">Back to Top</a>
  
[Back to Image Commands](#image-commands)
## Mesh Commands

### bounding-box-mesh


**Usage:**

```
shapeworks  bounding-box-mesh [args]...
```  


**Description:** compute bounding box of mesh  


**Options:**

**-h, --help:** show this help message and exit

**--names <list of strings>:**  Paths to meshes (must be followed by `--`), ex: "bounding-box-mesh --names *.vtk -- --center 1")  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### clip-closed-surface


**Usage:**

```
shapeworks  clip-closed-surface [args]...
```  


**Description:** clips mesh resulting in a closed surface  


**Options:**

**-h, --help:** show this help message and exit

**--px=DOUBLE:** Value of point.x for cutting plane [default: 0].

**--py=DOUBLE:** Value of point.y for cutting plane [default: 0].

**--pz=DOUBLE:** Value of point.z for cutting plane [default: 0].

**--nx=DOUBLE:** Value of normal.x for cutting plane [default: 0].

**--ny=DOUBLE:** Value of normal.y for cutting plane [default: 0].

**--nz=DOUBLE:** Value of normal.z for cutting plane [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### clip-mesh


**Usage:**

```
shapeworks  clip-mesh [args]...
```  


**Description:** clips mesh  


**Options:**

**-h, --help:** show this help message and exit

**--px=DOUBLE:** Value of point.x for cutting plane [default: 0].

**--py=DOUBLE:** Value of point.y for cutting plane [default: 0].

**--pz=DOUBLE:** Value of point.z for cutting plane [default: 0].

**--nx=DOUBLE:** Value of normal.x for cutting plane [default: 0].

**--ny=DOUBLE:** Value of normal.y for cutting plane [default: 0].

**--nz=DOUBLE:** Value of normal.z for cutting plane [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### closest-point


**Usage:**

```
shapeworks  closest-point [args]...
```  


**Description:** returns closest point to given point on mesh  


**Options:**

**-h, --help:** show this help message and exit

**--x=DOUBLE:** Value of x for point.

**--y=DOUBLE:** Value of y for point.

**--z=DOUBLE:** Value of z for point.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### compare-mesh


**Usage:**

```
shapeworks  compare-mesh [args]...
```  


**Description:** compare two meshes  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Compare this mesh with another.

**--epsilon=DOUBLE:** Epsilon [default: -1].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### compute-normals


**Usage:**

```
shapeworks  compute-normals [args]...
```  


**Description:** computes and adds oriented point and cell normals  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### coverage


**Usage:**

```
shapeworks  coverage [args]...
```  


**Description:** creates mesh of coverage between two meshes  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Path to other mesh with which to create coverage.

**--allowbackintersections=BOOL:**  Allow back-intersections in coverage calculation [default: true].

**--anglethreshold=DOUBLE:**  This checks the cosine between the ray’s direction vector (e1) and the normal at the intersection point (e2) [default: 0].

**--backsearchradius=DOUBLE:**  Max distance of a back-intersection [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### cvd-decimate


**Usage:**

```
shapeworks  cvd-decimate [args]...
```  


**Description:** applies cvd (centroidal voronoi diagram) decimation filter  


**Options:**

**-h, --help:** show this help message and exit

**--percentage=DOUBLE:** Percentage of target number of clusters/vertices [default: 0.5].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### decimate


**Usage:**

```
shapeworks  decimate [args]...
```  


**Description:** applies filter to reduce number of triangles in mesh  


**Options:**

**-h, --help:** show this help message and exit

**--reduction=DOUBLE:** Percent reduction of total number of polygons [default: 0.5].

**--angle=DOUBLE:** Necessary angle (in degrees) between two trianges to warrant keeping them separate [default: 15].

**--preservetopology=BOOL:**  Whether to preserve topology [default: true].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### distance


**Usage:**

```
shapeworks  distance [args]...
```  


**Description:** computes the distance between two meshes, printing the largest distance between any point from source to target, target to source, and the Hausdorff distance (the largest of these two)  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Filename of other mesh.

**--method=CHOICE:** Method used to compute distance [default: point-to-point]. (choose from 'point-to-point', 'point-to-cell')

**--summary=BOOL:** Print largest distance of any point in mesh to target [default: true].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### field-mean


**Usage:**

```
shapeworks  field-mean [args]...
```  


**Description:** prints the mean of the given field  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of scalar field.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### field-names


**Usage:**

```
shapeworks  field-names [args]...
```  


**Description:** prints all the field names present in mesh  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### field-range


**Usage:**

```
shapeworks  field-range [args]...
```  


**Description:** prints the range of the given field  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of scalar field.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### field-std


**Usage:**

```
shapeworks  field-std [args]...
```  


**Description:** prints the standard deviation of the given field  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of scalar field.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### fill-holes


**Usage:**

```
shapeworks  fill-holes [args]...
```  


**Description:** finds holes in a mesh and closes them  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### fix-element


**Usage:**

```
shapeworks  fix-element [args]...
```  


**Description:** fix element winding of mesh  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### geodesic-distance


**Usage:**

```
shapeworks  geodesic-distance [args]...
```  


**Description:** computes geodesic distance between two vertices on mesh  


**Options:**

**-h, --help:** show this help message and exit

**--v1=INT:** Index of first point in mesh.

**--v2=INT:** Index of second point in mesh.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### geodesic-distance-landmark


**Usage:**

```
shapeworks  geodesic-distance-landmark [args]...
```  


**Description:** computes geodesic distance between a point (landmark) and each vertex on mesh  


**Options:**

**-h, --help:** show this help message and exit

**--x=DOUBLE:** Value of x for landmark point.

**--y=DOUBLE:** Value of y for landmark point.

**--z=DOUBLE:** Value of z for landmark point.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### get-field


**Usage:**

```
shapeworks  get-field [args]...
```  


**Description:** gets field of mesh with given name  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of scalar field.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### get-field-value


**Usage:**

```
shapeworks  get-field-value [args]...
```  


**Description:** prints value of element at index in given field of mesh  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of scalar field.

**-i INT, --index=INT:** index of value to return [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### invert-normal


**Usage:**

```
shapeworks  invert-normal [args]...
```  


**Description:** flips the normal  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### mean-normals


**Usage:**

```
shapeworks  mean-normals [args]...
```  


**Description:** computes average normals for each point in given set of meshes  


**Options:**

**-h, --help:** show this help message and exit

**--names <list of strings>:**  Paths to meshes (must be followed by `--`), ex: "mean-normals --names *.vtk --")

**--generatenormals=BOOL:**  Auto generate normals if the mesh does not have normals [default: true].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### mesh-bounds


**Usage:**

```
shapeworks  mesh-bounds [args]...
```  


**Description:** return physical bounds of mesh  


**Options:**

**-h, --help:** show this help message and exit  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### mesh-curvature


**Usage:**

```
shapeworks  mesh-curvature [args]...
```  


**Description:** computes and adds curvature  


**Options:**

**-h, --help:** show this help message and exit

**--type=CHOICE:** Curvature type to use [default: principal]. (choose from 'principal', 'gaussian', 'mean')  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### mesh-info


**Usage:**

```
shapeworks  mesh-info [args]...
```  


**Description:** prints requested mesh center, center of mass, number of vertices, number of faces and bounding box [default: prints everything]  


**Options:**

**-h, --help:** show this help message and exit

**--vertices:** Whether to display number of vertices [default: true].

**--faces:** Whether to display number of faces [default: true].

**--center:** Whether to display center [default: true].

**--centerofmass:** Whether to display center of mass [default: true].

**--boundingbox:** Whether to display bounding box [default: true].

**--fieldnames:** Whether to display field names [default: true].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### mesh-to-dt


**Usage:**

```
shapeworks  mesh-to-dt [args]...
```  


**Description:** converts mesh to a distance transform, using unit spacing by default  


**Options:**

**-h, --help:** show this help message and exit

**--sx=DOUBLE:** Spacing of output image in x-direction [default: unit spacing].

**--sy=DOUBLE:** Spacing of output image in y-direction [default: unit spacing].

**--sz=DOUBLE:** Spacing of output image in z-direction [default: unit spacing].

**--pad=DOUBLE:** Pad the region to extract [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### mesh-to-image


**Usage:**

```
shapeworks  mesh-to-image [args]...
```  


**Description:** converts mesh to a binary segmentation image, using unit spacing by default  


**Options:**

**-h, --help:** show this help message and exit

**--sx=DOUBLE:** Spacing of output image in x-direction [default: unit spacing].

**--sy=DOUBLE:** Spacing of output image in y-direction [default: unit spacing].

**--sz=DOUBLE:** Spacing of output image in z-direction [default: unit spacing].

**--pad=DOUBLE:** Pad the region to extract [default: 0.0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### probe-volume


**Usage:**

```
shapeworks  probe-volume [args]...
```  


**Description:** probe feature volumes at each mesh vertex and output vtk meshes with scalar field defined based on such probing process  


**Options:**

**-h, --help:** show this help message and exit

**--image=STRING:** Path of image.  
  
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

**--name=STRING:** Name of file to read.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### reflect-mesh


**Usage:**

```
shapeworks  reflect-mesh [args]...
```  


**Description:** reflect meshes with respect to a specified center and specific axis  


**Options:**

**-h, --help:** show this help message and exit

**--axis=STRING:** Axis along which to reflect (X, Y, or Z).

**-x DOUBLE, --originx=DOUBLE:**  Origin about which reflection occurs in x-direction [default: 0].

**-y DOUBLE, --originy=DOUBLE:**  Origin about which reflection occurs in y-direction [default: 0].

**-z DOUBLE, --originz=DOUBLE:**  Origin about which reflection occurs in z-direction [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### scale-mesh


**Usage:**

```
shapeworks  scale-mesh [args]...
```  


**Description:** scales mesh  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --sx=DOUBLE:**  X scale.

**-y DOUBLE, --sy=DOUBLE:**  Y scale.

**-z DOUBLE, --sz=DOUBLE:**  Z scale.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### set-field


**Usage:**

```
shapeworks  set-field [args]...
```  


**Description:** adds the current field to the current mesh with the given name.  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of scalar field.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### set-field-value


**Usage:**

```
shapeworks  set-field-value [args]...
```  


**Description:** sets value of element at index in given field of mesh  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of scalar field.

**-i INT, --index=INT:** index of value to return [default: 0].

**--value=DOUBLE:** value to be set [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### smooth


**Usage:**

```
shapeworks  smooth [args]...
```  


**Description:** applies laplacian smoothing  


**Options:**

**-h, --help:** show this help message and exit

**--iterations=INT:** Number of iterations [default: 0].

**--relaxation=DOUBLE:** Amount of displacement for a vertex to move in each iteration [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### smooth-sinc


**Usage:**

```
shapeworks  smooth-sinc [args]...
```  


**Description:** applies windowed sinc smoothing  


**Options:**

**-h, --help:** show this help message and exit

**--iterations=INT:** Number of iterations [default: 0].

**--passband=DOUBLE:** Set the passband value for the windowed sinc filter [default: 0].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### transform-mesh


**Usage:**

```
shapeworks  transform-mesh [args]...
```  


**Description:** transform mesh to target mesh using iterative closest point (ICP) using specified landmark transform (rigid, similarity, or affine)  


**Options:**

**-h, --help:** show this help message and exit

**--target=STRING:** Filename of target mesh.

**--type=CHOICE:** Alignment type to use [default: similarity]. (choose from 'rigid', 'similarity', 'affine')

**--iterations=UNSIGNED:**  Number of iterations run [default: 10].  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### translate-mesh


**Usage:**

```
shapeworks  translate-mesh [args]...
```  


**Description:** translates mesh  


**Options:**

**-h, --help:** show this help message and exit

**-x DOUBLE, --tx=DOUBLE:**  X distance.

**-y DOUBLE, --ty=DOUBLE:**  Y distance.

**-z DOUBLE, --tz=DOUBLE:**  Z distance.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### warp-mesh


**Usage:**

```
shapeworks  warp-mesh [args]...
```  


**Description:** warps a mesh given reference and target particles  


**Options:**

**-h, --help:** show this help message and exit

**--reference_mesh=STRING:**  Name of reference mesh.

**--reference_points=STRING:**  Name of reference points.

**--target_points <list of strings>:**  Names of target points (must be followed by `--`), ex: "... --target_points *.particles -- ...

**--save_dir=STRING:** Optional: Path to the directory where the mesh files will be saved  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
### write-mesh


**Usage:**

```
shapeworks  write-mesh [args]...
```  


**Description:** writes the current mesh  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Name of file to write.  
  
<a href="#top">Back to Top</a>
  
[Back to Mesh Commands](#mesh-commands)
## Optimize Commands

### optimize


**Usage:**

```
shapeworks  optimize [args]...
```  


**Description:** generate a particle system  


**Options:**

**-h, --help:** show this help message and exit

**--name=STRING:** Path to parameter file.  
  
<a href="#top">Back to Top</a>
  
[Back to Optimize Commands](#optimize-commands)
## ParticleSystem Commands

### compactness


**Usage:**

```
shapeworks  compactness [args]...
```  


**Description:** Compute compactness of a loaded particle system  


**Options:**

**-h, --help:** show this help message and exit

**--nmodes=INT:** Number of modes to use [default: 1].

**--saveto=STRING:** Save the scree plots for all modes to a file.  
  
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

**--nmodes=INT:** Number of modes to use [default: 1].

**--saveto=STRING:** Save the reconstructions sorted by generalization along with the mapping to the original shape.  
  
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

**--names <list of strings>:**  Paths to .particle files (must be followed by `--`), ex: "--names *.particle -- next-command...")  
  
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

**--nmodes=INT:** Number of modes to use [default: 1].

**--saveto=STRING:** Save the reconstructions sorted by specificity along with the mapping to the original shape.  
  
<a href="#top">Back to Top</a>
  
[Back to ParticleSystem Commands](#particlesystem-commands)
## Shapeworks Commands

### seed


**Usage:**

```
shapeworks  seed [args]...
```  


**Description:** sets the seed for random number generation (useful for debugging)  


**Options:**

**-h, --help:** show this help message and exit

**--value=INT:** Value of seed.  
  
<a href="#top">Back to Top</a>
  
[Back to Shapeworks Commands](#shapeworks-commands)