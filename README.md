ShapeWorksStudio
=====================

A new GUI environment for ShapeWorks preprocessing, optimization, and analysis.
<br/><br/>
See the original command-line tools, aknowlegements, and papers on the 
<a href="https://github.com/SCIInstitute/shapeworks">shapeworks</a> github page.

Requirements
=====================
 * Git (https://git-scm.com/)
 * CMake 2.6+ (http://www.cmake.org/)
 * Visualization ToolKit (VTK 6+ recommended) (http://www.vtk.org/)
 * Insight Toolkit (ITK 4.5+ recommended) (http://www.itk.org/)
 * Qt 4.8.* (http://www.qt.io/developers/)
 * Windows 7+, OSX 10.9+, and OpenSuse 13.1 Recommended. Other platforms may work, but are not officially supported.
 * Shapeworks command line tools (Groom and Run) are required for Studio (https://github.com/SCIInstitute/shapeworks/releases)

Code Documentation
=====================
[Doxygen Documentation](http://sciinstitute.github.io/ShapeWorksStudio/doxygen/) pages are currently out of date, but still relevant.

Building
=====================
<h3>Unix / OSX</h3>
In a terminal:
```c++
mkdir ShapeWorksStudio/build
cd ShapeWorksStudio/build
cmake -DVTK_DIR=Path/To/Your/VTK/build -DITK_DIR=Path/To/Your/ITK/build -DCMAKE_BUILD_TYPE=Release ../src
make
```

<h3>Windows</h3>
Open a Visual Studio (32 or 64 bit) Native Tools Command Prompt. 
Follow these commands:
```c++
mkdir C:\Path\To\ShapeWorksStudio\build
cd C:\Path\To\ShapeWorksStudio\build
cmake -G "NMake Makefiles" -DVTK_DIR="C:/Path/To/Your/VTK/build" -DITK_DIR="C:/Path/To/Your/ITK/build" -DCMAKE_BUILD_TYPE=Release ../src
nmake
```
**NOTE** Be sure to copy the Qt DLL files to the Executable directory for the program to run.

Running
=====================
Studio runs with no command-line arguments. Once running, set the locations of ShapeWorksGroom 
and ShapeWorksRun in the Preferences dialog (ctrl+,). 
<br/><br/>
You Should have a Qt window that looks similar to the one below.
<br/><br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Resources/Images/Application.png">
<br/><br/>
The application has tools on the left, the rendering window, view options on the lower bar, a small file menu, and a
preferences dialog.
<br/><br/>
<h3>Tools</h3>
**Data**<br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Resources/Images/DataPanel.png"
align="right" hspace="20">
This tool tab displays the images (volume files in NRRD format) that have been loaded. 
You can select the image files to load by either clicking the "+" button at the bottom, 
or going to *File -> Import Images...* <br/> Your images will appear on the display once loaded.
<br/><br/>
You can delete images by selecting the image of choice and clicking the "-" button at the bottom. If only one image 
is loaded, it may still display when it is deleted. It will be replaced by the next image load. 
<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
**Groom**<br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Resources/Images/Groom.png"
align="right" hspace="20">
Once images are loaded, the preprocessing step, "groom" is next. You can select several options for grooming.
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
*Export XML* If you wish to use the parameter file created for the groom step in a command line environment, 
you can export the XML with the grooming options above.
<br/><br/>
**Optimize**<br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Resources/Images/Optimize.png"
align="right" hspace="20">
Once the grooming step is complete, you can run the optimize step.
<br/><br/>
*Number of points* Specifies the number of particles to be used to represent each shape in the ensemble. If enough initial
point positions are supplied for the optimization, then ShapeWorksRun will not conduct a splitting-based initialization
phase. If insufficient particle initializations are supplied (or none at all), then ShapeWorksRun will initialize the
model as described in Section 2.3 until this number of particles exists on each shape surface.
<br/><br/>
*Starting/Ending Regularization* This option determines the regularizations on the covariance matrix for the shape-space 
entropy estimation.
<br/><br/>
*Iterations per split* Specifies the number of iterations to run between successive particle splits during an 
initialization phase. This allows the particle system at a given granularity to converge to a stable state 
before more particles are added.
<br/><br/>
*Optimization Iterations* Specify the range for a constant regularization factor that is added to the covariance 
matrix of the correspondences during the optimization process. This range, along with the number of optimization 
iterations, define the rate at which the system converges. The starting regularization decays to the ending
regularization over the specified number of iterations.
<br/><br/>
*Run Optimize* Click this when you are ready for optimizing. This step takes time, but less than grooming.
Output is put into a ShapeWorksStudioTools.log file next to the executable. A progress indicator shows the tool is working.
<br/><br/>
*Export XML* If you wish to use the parameter file created for the groom step in a command line environment, 
you can export the XML with the grooming options above. There are other options not in the GUI that you can 
add to a parameter file to run outside of Studio.
<br/><br/>
**Analysis**<br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Resources/Images/Analysis.png"
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
*Regression* This option is not yet available.
<br/><br/>
<h3>Rendering Window</h3>
The render window has a few features shortcuts to options available for viewing.
<br/><br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Resources/Images/Render.png"
hspace="20">
<br/>
From left to right, here are the rendering options.
<br/><br/>
*Autoview* Reset the view to fit the samples. This only affects zoom and translation.
<br/><br/>
*Show Glyphs* Toggle whether to show the glyphs for the coorespondence points.
<br/><br/>
*Glyph options* Click the down arrow to resize the glyphs or select the quality of the glyphs.
<br/><br/>
*Show isosurface* Toggle whether to view the surface representing the shape.
<br/><br/>
*View mode drop-down* This drop-down gives 3 options for view mode. Original is the binary segmentation. You must
have loaded images for this option to be available. Groomed is for the distance transform view. You must  
run the groom step for this to be available. Reconstructed is for the calculated shape based on the set of 
coorespondance points. You must run the optimize step for this to be available.
<br/><br/>
*Center* Center the samples automatically to align. This is useful if original samples aren't the same in size.
<br/><br/>
*Zoom* This slider allows the user to zoom in or out to view more/less samples. This is mainly useful in the "All Samples"
mode of the analysis tool. Zoom is automatically selected as a user switches between analysis modes.
<br/><br/>
<h3>Preferences</h3>
<img src="https://raw.githubusercontent.com/SCIInstitute/ShapeWorksStudio/master/src/Resources/Images/pref-general.png"
align="right" hspace="20">
*Color Scheme* Select the color scheme for the rendering window.
<br/><br/>
*Glyph Quality* This slider changes the quality of the coorespondance points glyphs. This is sync'd with the render window
shortcut option.
<br/><br/>
*Glyph Size* This slider changes the size of the coorespondance points glyphs. This is sync'd with the render window
shortcut option.
<br/><br/>
*PCA Range* This is the amount of standard deviation to reach on the +/- ends of the PCA Slider.
<br/><br/>
*Number of PCA Steps* This determines how many steps between +/- PCA Range to take for visualization.
<br/><br/>
*Enable Caching* To speed up mesh animation, you can cache the meshes into system memory to load as needed.
<br/><br/>
*Memory to Use* Select the amount of system memory to use for caching. Turn this down if your machine's memory 
is bogged down from the program.
<br/><br/>
*Parallel Reconstruction* Select the amount of threads to fire (up to system hardware core max) to run while 
building meshes. This speeds reconstruction, theoretically.
<br/><br/>
*Neighborhood Size* The neighborhood size (max vertex valence) used for isosurface reconstruction.
<br/><br/>
*Spacing* The spacing used for isosurface reconstruction.
<br/><br/>
*Smoothing* The smoothing amount for isosurface reconstruction.
<br/><br/>
*Restore Defaults* Click this to restore all options to the program's default values. Options are saved and reloaded
between application runs for convenience.
<br/><br/>
*OK* Click this when you are done changing options.
<br/><br/>
<h3>File Menu</h3>
Here you can open and save projects, load new images, open recent projects, and quit the application.
<br/><br/>

Contact / Bug Reports
=====================
Please email any questions to Shapeworks-users@sci.utah.edu . If there problems or bugs, please report them using the issue tracker on GitHub. This includes feature requests. Feel free to add improvements using git pull requests. 
