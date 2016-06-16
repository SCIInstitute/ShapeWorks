ShapeWorksStudio
=====================
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Application/Resources/Images/shapes-icon.png"  align="right" hspace="20">
A new GUI environment for ShapeWorks preprocessing, optimization, and analysis.
<br/><br/>
See the original command-line tools, aknowlegements, and papers on the 
<a href="https://github.com/SCIInstitute/shapeworks">shapeworks</a> github page.

Table of Contents
====================
- [Requirements](#requirements)<br/>
- [Code Documentation](#code-documentation)<br/>
- [Building](#building)<br/>
		- [Building ITK](#building-itk)<br/>
		- [Building VTK](#building-vtk)<br/>
		- [Building ShapeWorksStudio](#building-shapeworksstudio)<br/>
			- [Unix/OSX](#unix-and-osx)<br/>
			- [Windows](#windows)<br/>
- [Running](#running)<br/>
		- [Data](#data)<br/>
		- [Groom](#groom)<br/>
		- [Optimize](#optimize)<br/>
		- [Analysis](#analysis)<br/>
		- [Rendering Window](#rendering-window)<br/>
		- [Preferences](#preferences)<br/>
		- [File Menu](#file-menu)<br/>
- [Known Issues](#known-issues)<br/>
- [Contact and Bug Reports](#contact-and-bug-reports)<br/>

Requirements
===========
 * C++11 compatible compiler
 * Git (https://git-scm.com/)
 * CMake 2.8+ (http://www.cmake.org/)
 * Visualization ToolKit (VTK 7.* recommended) (http://www.vtk.org/)
 * Insight Toolkit (ITK 4.9+ recommended) (http://www.itk.org/)
 * Qt 5.* (http://www.qt.io/developers/)
 * Windows 7+, OSX 10.9+, and OpenSuse 13.1+ Recommended. Other platforms may work, but are not officially supported.

Code Documentation
=====================
[Doxygen Documentation](http://sciinstitute.github.io/ShapeWorksStudio/doxygen/) pages are currently out of date, but still relevant.

Building
=====================
##Building ITK
- Checkout "Particle Shape Modeling"
  - <code>cd ITK_DIR/Modules/ThirdParty</code>
  - <code>git clone https://github.com/brigb123/ITKParticleShapeModeling.git</code>
- Configure with:
  - <code>CMAKE_CXX_FLAGS+="-std=c++11"</code> on UNIX
  - <code>BUILD_SHARED_LIBS=FALSE</code>
  - <code>BUILD_EXAMPLES=FALSE</code>
  - <code>BUILD_TESTING=FALSE</code>
  - <code>ITKV3_COMPATIBILTY=TRUE</code>
- Build ITK:
  - Make command: <code>make -j4</code><br/>

##Building VTK

- Configure with:
  - <code>VTK_Group_Qt=ON</code> on UNIX
  - <code>VTK_QT_VERSION=5</code> on UNIX
  - <code>Qt5_DIR="/PATH/TO/YOUR/Qt5"</code> on UNIX
  - <code>BUILD_SHARED_LIBS=FALSE</code>
  - <code>BUILD_EXAMPLES=FALSE</code>
  - <code>BUILD_TESTING=FALSE</code><br/>
- Build VTK
  - Make command: <code>make -j4</code><br/>

##Building ShapeWorksStudio<br/>

###Unix and OSX<br/>

In a terminal:
```c++
mkdir ShapeWorksStudio/build
cd ShapeWorksStudio/build
cmake -DVTK_DIR=Path/To/Your/VTK/build -DITK_DIR=Path/To/Your/ITK/build -DQT_DIR=Path/To/Your/Qt5/build -DCMAKE_BUILD_TYPE=Release ../src
make
```

###Windows
Open a Visual Studio (32 or 64 bit) Native Tools Command Prompt. 
Follow these commands:
```c++
mkdir C:\Path\To\ShapeWorksStudio\build
cd C:\Path\To\ShapeWorksStudio\build
cmake -G "NMake Makefiles" -DVTK_DIR="C:/Path/To/Your/VTK/build" -DITK_DIR="C:/Path/To/Your/ITK/build" -DQT_DIR="C:/Path/To/Your/Qt5/build" -DCMAKE_BUILD_TYPE=Release ../src
nmake
```
**NOTE** Be sure to copy the Qt5 DLL files to the Executable directory for the program to run.
```c++
* Qt5Widgets.dll
* Qt5Core.dll
* Qt5OpenGL.dll
* Qt5Gui.dll
```

Running
=====================
Studio runs with no command-line arguments. 
<br/><br/>
You Should have a Qt window that looks similar to the one below.
<br/><br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Application/Resources/Images/Application.png">
<br/><br/>
The application has tools on the left, the rendering window, view options on the lower bar, a small file menu, and a
preferences dialog.

Tools
==========
<h2>Data</h2>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Application/Resources/Images/DataPanel.png"
align="right" hspace="20">
This tool tab displays the images (volume files in NRRD format) that have been loaded. 
You can select the image files to load by either clicking the blue "+" button at the bottom, 
or going to *File -> Import Images...* <br/> Your images will appear on the display once loaded.
<br/><br/>
You can delete images by selecting the image of choice and clicking the red "x" button at the bottom. If only one image 
is loaded, it may still display when it is deleted. It will be replaced by the next image load. 
<br/><br/><br/><br/><br/><br/><br/><br/><br/>
<h2>Groom</h2>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Application/Resources/Images/Groom.png"
align="right" hspace="20">
Once more than 1 image is loaded, the preprocessing step, "groom" is next. You can select several options for grooming.
<br/><br/>
*Fill Holes* This option fills any existing holes in the segmentations.
<br/><br/>
*Isolate* This option isolates the foreground.
<br/><br/>
*Center* This option attempts to allign the segmentations to a common center if they aren't the same sizes.
<br/><br/>
*Autocrop* Use this option to find the largest bounding box containing all input shapes, and crop all input volumes
accordingly.
<br/><br/>
*Pad* This option adds a padding around the shapes. This can help when a larger number of correspondance points are needed.
You can select what amount is appropriate for the optimization step.
<br/><br/>
*Antialias* This option antialiases the binary segmentation. You can select the number of antialias iterations.
<br/><br/>
*Fastmarching* This option computes a distance transform of the surface using the fastmarching method.
<br/><br/>
*Blur* This option blurs the distance transform to get rid of high-frequency artifacts. You can choose an appropriate sigma
for the blurring.
<br/><br/>
*Run Groom* Click this when you are ready for grooming. This step takes some time. Output is put into a 
ShapeWorksStudioTools.log file next to the executable. A progress indicator shows the tool is working.
<br/><br/>

<h2>Optimize</h2>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Application/Resources/Images/Optimize.png"
align="right" hspace="20">
Once the grooming step is complete, you can run the optimize step. The Optimize table has 5 parameters per split.
<br/><br/>
*Number of splits* Specifies the number of particles (2 ^ splits) to be used to represent each shape in the ensemble.
<br/><br/>
*Starting/Ending Regularization* These options determines the regularizations on the covariance matrix for the shape-space 
entropy estimation.
<br/><br/>
*Iterations per split* Specifies the number of iterations to run between successive particle splits during an 
initialization phase. This allows the particle system at a given granularity to converge to a stable state 
before more particles are added.
<br/><br/>
*Tolerance* This is the error tolerance per split, which sets the threshold for optimization to be considered converged.
<br/><br/>
*Decay Span* The parameters RegStart, RegEnd, and DecaySpan define an "annealing" type optimization,
where the minimum variance starts at RegStart and exponentially decreases over DecaySpan to the      
RegEnd value.  If the DecaySpan is set to 0, then only a constant is used as the Minimum Variance 
parameter and is held fixed during the optimization.
<br/><br/>
*Run Optimize* Click this when you are ready for optimizing. This step takes a while.
A progress indicator shows the tool is working.
<br/><br/>

<h2>Analysis</h2>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Application/Resources/Images/Analysis.png"
align="right" hspace="20">
Here is where all the statistical options are available to the user.
<br/><br/>
*All Samples* Display all of the image segmentation samples on the screen.
<br/><br/>
*Single Sample* Display only one sample. You can select a particular number, or click "median" to display the shape 
that is the computed median of all the shapes.
<br/><br/>
*Mean* Display the computed mean of the shapes.
<br/><br/>
*PCA* Display the computed shape from an eigen value and a standard deviation.
<br/><br/>
*Std.Dev.* Slide the slider back and forth to display the computed shape with various standard deviations.
<br/><br/>
*Animate* Check this box to watch the shape morph between various standard deviations automatically. Depending on 
the machine running the application, the number of samples, and the size of the samples, the animation may be 
slow at first while building and caching the meshes. You can select the caching, memory, and threading options
in the Preferences. Changing the number of neighbors, spacing, and smoothing options in preferences also affects
meshing time and quality.
<br/><br/>
*Mode* This selects the ordered eigen vector and eigen values from the statistical analysis. There are #samples - 1 modes.
The higher the mode, the less variance with the standard deviations. Usually the first two modes contain the most variance 
between shapes.
<br/><br/>
*Log Scale vs. Linear Scale* The bar graph can be plotted in either log or linear scaling. The graph shows the eigen values
in decreasing values to depict statistical relevancy.
<br/><br/>
*Regression* This option is not yet available.

<h3>Rendering Window</h3>
The render window has a few shortcuts to viewing options.
<br/><br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Application/Resources/Images/Render.png"
hspace="20">
<br/>
From left to right, here are the rendering options.
<br/><br/>
*Autoview* Reset the view to fit the samples. This only affects zoom and translation.
<br/><br/>
*Show Glyphs* Toggle whether to show the glyphs for the coorespondence points.
<br/><br/>
*Glyph Quality* This slider changes the quality of the coorespondance points glyphs. This is sync'd with the render window
shortcut option.
<br/><br/>
*Glyph Size* This slider changes the size of the coorespondance points glyphs. This is sync'd with the render window
shortcut option.
<br/><br/>
*Glyph options* Click the down arrow to resize the glyphs or select the quality of the glyphs.
<br/><br/>
*Show isosurface* Toggle whether to view the surface representing the shape. Click the down arrow for more options. 
You can alter the number of neighbors, point spacing, and mesh smoothing for isosurface reconstruction.
<br/><br/>
*Neighborhood Size* The neighborhood size (max vertex valence) used for isosurface reconstruction.
<br/><br/>
*Spacing* The spacing used for isosurface reconstruction.
<br/><br/>
*Smoothing* The smoothing amount for isosurface reconstruction.
<br/><br/>
*View mode drop-down* This drop-down gives 3 options for view mode. Original is the binary segmentation. You must
have loaded images for this option to be available. Groomed is for the distance transform view. You must  
run the groom step for this to be available. Reconstructed is for the calculated shape based on the set of 
coorespondance points. You must run the optimize step for this to be available.
<br/><br/>
*Center* Center the samples automatically to align. This is useful if original samples aren't the same size.
<br/><br/>
*Zoom* This slider allows the user to zoom in or out to view more/less samples. This is mainly useful in the "All Samples"
mode of the analysis tool. Zoom is automatically selected as a user switches between analysis modes.

<h3>Preferences</h3>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Application/Resources/Images/pref-general.png"
align="right" hspace="20">
*Color Scheme* Select the color scheme for the rendering window.
<br/><br/>
*PCA Range* This is the amount of standard deviation to reach on the +/- ends of the PCA Slider.
<br/><br/>
*Number of PCA Steps* This determines how many steps between +/- PCA Range to take for visualization.
<br/><br/>
*Enable Caching* To speed up mesh animation, you can cache the meshes into system memory to load as needed.
<br/><br/>
*Caching Epsilon* How sensitive the caching is. The smaller the epsilon, the more meshes that will be cached, even if
they are very similar. If this is too high, large mesh differences might be ignored.
<br/><br/>
*Memory to Use* Select the amount of system memory to use for caching. Turn this down if your machine's memory 
is bogged down from the program.
<br/><br/>
*Parallel Reconstruction* Select the amount of threads to fire (up to system hardware core max) to run while 
building meshes. This speeds reconstruction, theoretically.
<br/><br/>
*Restore Defaults* Click this to restore all options to the program's default values. Options are saved and reloaded
between application runs for convenience.
<br/><br/>
*OK* Click this when you are done changing options.
<br/><br/>
<h3>File Menu</h3>
* Start a new project
* Open and save projects
* Set the data directory for where generated images will be written when a project is saved.
* Load new images
* Export a mesh reconstructed for a sample, the mean, or a variance from the mean.
* Export an XML parameter file to run a command line too on more images
* Open recent projects
* Quit the application.
<br/><br/>

Known Issues
=====================
* On OSX, full screen mode has a widget bug visualization problem for the left panel. Future Qt releases may fix this.

Contact and Bug Reports
=====================
Please email any questions to Shapeworks-users@sci.utah.edu . If there problems or bugs, please report them using the issue tracker on GitHub. This includes feature requests. Feel free to add improvements using git pull requests. 
