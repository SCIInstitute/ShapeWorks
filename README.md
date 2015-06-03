Shapeworks Command Line Tools
=====================
<img src="https://raw.githubusercontent.com/SCIInstitute/shapeworks/master/src/deprecated/documentation/splashmain.png" 
 width="400px" align="left" hspace="20">

The ShapeWorks software is an open-source distribution of a new method for constructing compact 
statistical point-based models of ensembles of similar shapes that does not rely on any specific 
surface parameterization. The method requires very little preprocessing or parameter tuning, and 
is applicable to a wide range of shape analysis problems, including nonmanifold surfaces and 
objects of arbitrary topology. The proposed correspondence point optimization uses an entropy-based 
minimization that balances the simplicity of the model (compactness) with the accuracy of the 
surface representations. The ShapeWorks software includes tools for preprocessing data, computing 
point-based shape models, and visualizing the results.
<br/>

<h3>Aknowledgements</h3>
If you use ShapeWorks in work that leads to published research, we humbly ask that you add the 
following to the 'Acknowledgments' section of your paper: "This project was supported by the 
National Institute of General Medical Sciences of the National Institutes of Health under 
grant number P41GM103545."

<h3> Relevant Papers </h3>

The following selected papers give background on the methods used in ShapeWorks and a 
sample of the areas ShapeWorks has been applied:

J. Cates, P.T. Fletcher, M. Styner, H. Hazlett, R.T. Whitaker. 
"Particle-Based Shape Analysis of Multi-Object Complexes," 
In Proceedings of the 11th International Conference on Medical Image Computing and 
Computer Assisted Intervention (MICCAI '08), Lecture Notes In Computer Science (LCNS), 
pp. 477--485. 2008. ISBN: 978-3-540-85987-1

J. Cates, P.T. Fletcher, Z. Warnock, R.T. Whitaker. 
"A Shape Analysis Framework for Small Animal Phenotyping with Application to Mice with 
a Targeted Disruption of Hoxd11," In Proceedings of the 5th IEEE International Symposium 
on Biomedical Imaging (ISBI '08), pp. 512--516. 2008. DOI: 10.1109/ISBI.2008.4541045

K.B. Jones, M. Datar, S. Ravichandran, H. Jin, E. Jurrus, R.T. Whitaker, M.R. Capecchi. 
"Toward an Understanding of the Short Bone Phenotype Associated with Multiple Osteochondromas," 
In Journal of Orthopaedic Research, Vol. 31, No. 4, pp. 651--657. 2013.

M.D. Harris, M. Datar, R.T. Whitaker, E.R. Jurrus, C.L. Peters, A.E. Anderson. 
"Statistical Shape Modeling of Cam Femoroacetabular Impingement," In Journal of Orthopaedic Research, 
Vol. 31, No. 10, pp. 1620--1626. 2013.

Requirements
=====================
 * Git (https://git-scm.com/)
 * CMake 2.6+ (http://www.cmake.org/)
 * Visualization ToolKit (VTK 6+ recommended) (http://www.vtk.org/)
 * Insight Toolkit (ITK 4.5+ recommended) (http://www.itk.org/)
 * Qt 4.8.* (http://www.qt.io/developers/)
 * Windows 7+, OSX 10.9+, and OpenSuse 13.1 Recommended. Other platforms may work, but are not officially supported.
 
Building
=====================
<h3>Unix / OSX</h3>
In a terminal:
```c++
mkdir shapeworks/build
cd shapeworks/build
cmake -DVTK_DIR=Path/To/Your/VTK/build -DITK_DIR=Path/To/Your/ITK/build -DCMAKE_BUILD_TYPE=Release ../src
make
```

<h3>Windows</h3>
Open a Visual Studio (32 or 64 bit) Native Tools Command Prompt. 
Follow these commands:
```c++
mkdir C:\Path\To\shapeworks\build
cd C:\Path\To\shapeworks\build
cmake -G "NMake Makefiles" -DVTK_DIR="C:/Path/To/Your/VTK/build" -DITK_DIR="C:/Path/To/Your/ITK/build" -DCMAKE_BUILD_TYPE=Release ../src
nmake
```
**NOTE** Be sure to copy the Qt DLL files to the View2 Executable directory for the program to run.

Running
=====================

The original documentation is located in this repository under <code>src/deprecated/documentation</code>.
You may read about the details of the algorithms there. 
<br/><br/>
Here are the basic instructions/pipeline for using the Shapeworks
Command Line Tools. You can follow the same steps for either the <code>examples/torus</code> or 
<code>examples/mickey</code>. Steps for the torus example are shown. <br/>

<h3>ShapeWorksGroom</h3>
```c++
cd examples/torus
../../build/bin/ShapeWorksGroom torus.groom.xml isolate hole_fill center auto_crop antialias fastmarching blur
```
This step may takes some time.

<h3>ShapeWorksRun</h3>
```c++
../../build/bin/ShapeWorksRun torus.run.xml
```

<h3>ShapeWorksView2</h3>
```c++
../../build/bin/ShapeWorksView2 torus.analysis.xml
```
The visualizer is called "View2" since a previous and outdated version uses the FLTK library, not needed for View2.
<br/><br/>
You Should have a Qt window pop up that looks similar to the one below.

<img src="https://raw.githubusercontent.com/SCIInstitute/shapeworks/master/src/deprecated/documentation/view2.png">
<br/><br/>
**Shape**<br/>
*Mean* The Overall mean shows the average shape of the samples. Difference refers to the difference between two groups 
of samples, which is a feature that is not yet available. 
<br/> <br/>
*Samples* Here you select which sample to show in the viewer. Clicking Median will give you the median sample.
<br/> <br/>
*PCA* This tab provides a way to visualize the shape standard deviations from the modes (eigen vector solutions from the 
statistical analysis.) You can select the mode and slide the standard deviation to show the respective shapes in the 
viewer. By clicking the "Animate" check, the slider will continuously slide back and forth to visualize the shapes.
The respective eigen values and lambdas are displayed below the slider.
<br/> <br/>
*Regression* This is another feature not yet available. 
<br/> <br/>
**Visualization** <br/>
*Show Glyphs* Toggle visualization of the correspondance point glyphs.
<br/><br/>
*Show Surface* Toggle visualization of the isosurface.
<br/><br/>
*Neighborhood Size* The neighborhood size used for isosurface reconstruction without powercrust.
<br/><br/>
*Spacing* The spacing used for isosurface reconstruction without powercrust.
<br/><br/>
*Smoothing* The smoothing amount for isosurface reconstruction.
<br/><br/>
<img src="https://raw.githubusercontent.com/SCIInstitute/shapeworks/master/src/deprecated/documentation/preferences.png" 
 align="right" hspace="20">
**Preferences (ctrl+,)** <br/>
This window provides a number of options for display and meshing.
<br/><br/>
*Color Scheme* Select the Surface and background colors for the viewer.
<br/><br/>
*Glyph Quality* Select the quality of the spheres that represent correspondance points.
<br/><br/>
*Glyph Size* Select the size of the spheres that represent correspondance points.
<br/><br/>
*Memory Cache* To speed up mesh animation, you can cache the meshes into system memory to load as needed.
<br/><br/>
*Memory to Use* Select the amount of system memory to use for caching. Turn this down if your machine's memory 
is bogged down from the program.
<br/><br/>
*Parallel Reconstruction* Select the amount of threads to fire (up to system hardware core max) to run while 
building meshes. This speeds reconstruction, theoretically. Threading is not used if powercrust is enabled.
<br/><br/>
*PCA Range* This is the amount of standard deviation to reach on the +/- ends of the PCA Slider.
<br/><br/>
*Number of PCA Steps* This determines how many steps between +/- PCA Range to take for visualization.
<br/><br/>
*Number of Regression Steps* This feature is not currently available.
<br/><br/>
*Restore Defaults* Reset the preferences to the program defaults.
<br/><br/>
**File Menu** <br/>
There are a few Import/Export options under the File Menu. The details of these options are 
beyond the scope of this document.

Contact
=====================
Please email any questions to Shapeworks-users@sci.utah.edu .
