ShapeWorks
=====================
<!--
<img src="https://raw.githubusercontent.com/SCIInstitute/shapeworks/master/src/deprecated/documentation/splashmain.png" 
 width="400px" align="left" hspace="20">
-->

<img src="Images/shapeworks-logo.png" width="300px" align="left" hspace="20">


<p align="justify">
  ShapeWorks is a free, open-source suite of software tools that uses a flexible method for automated construction of compact statistical landmark-based shape models of ensembles of anatomical shapes that does not rely on any specific surface parameterization. The method requires very little preprocessing or parameter tuning, and is applicable to a wide range of shape analysis problems, including nonmanifold surfaces and objects of arbitrary topology. ShapeWorks includes tools for preprocessing data, computing landmark-based shape models, and visualizing the results.
</p>


<br>
<img src="Images/about-shapeworks.png" width="800px" align="right" hspace="20">
<br>

<p align="justify">
The underlying scientific premise of ShapeWorks, *particle-based shape modeling*, is a groupwise approach to placing landmarks (i.e., correspondences) that considers the variability in the entire cohort of images. ShapeWorks uses a set of interacting particle systems, one for each shape, in an ensemble, to produce optimal sets of surface correspondences. Particles interact with one another via mutually repelling forces to optimally cover, and therefore describe, the surface geometry. Particles are positioned on surfaces automatically by optimizing the information content of the model via an entropy optimization scheme. In particular, ShapeWorks explicitly models the inherent trade-off between the statistical simplicity of the model (i.e., compactness) in the shape space and the accuracy of the shape representations (i.e., good surface samplings) in the configuration space. ShapeWorks has been effective in a range of applications, including psychology, biological phenotyping, cardiology, and orthopedics. [See relevant papers](#relevant-papers). 
</p>


Contact Us
=====================

User Forum
----------
Our user forum is located on [Gitter ShapeWorks User Forum](https://gitter.im/ShapeWorks).

Mailing List
------------
Please join our mailing list by sending a message to sympa@sci.utah.edu with the subject `subscribe shapeworks-users` and empty body.
You can also email any questions, bugs, or feature requests to shapeworks-users@sci.utah.edu.

Table of Contents
====================
- [Installation](#installation)  
- [Documentation](#documentation)  
- [Acknowledgements](#acknowledgements)
- [Relevant Papers](#relevant-papers)

Installation
=====================

Download
---------------------
_Coming soon:_ user installation without the necessity of building ShapeWorks directly.  
Until then, please see **[Development](#development)** to build ShapeWorks.

Development
---------------------
Please see **[BUILD.md](BUILD.md)** for the current build instructions.  
<!--
Please see **[GettingStarted.md](GettingStarted.md)** for help using `git` to clone code and `MarkDown` for documentation.
-->
Documentation is generated from [the gh-pages branch in GitHub.](https://github.com/SCIInstitute/ShapeWorks/tree/gh-pages)

As a developer, if you encounter any problems or bugs, please report them using [the issue tracker on GitHub](https://github.com/SCIInstitute/ShapeWorks/issues). This includes feature requests. Feel free to add improvements using git pull requests.

Documentation
=====================
[ShapeWorks user documentation.](http://sciinstitute.github.io/ShapeWorks/)
<!--
Desired: https://shapeworks.sci.utah.edu/docs
-->

<h3>Acknowledgements</h3>
If you use ShapeWorks in work that leads to published research, we humbly ask that you add the 
following to the 'Acknowledgments' section of your paper: "This project was supported by the 
National Institute of General Medical Sciences of the National Institutes of Health under 
grant number P41GM103545."

<h3>Relevant Papers</h3>

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

P. Agrawal, S.Y. Elhabian, R.T. Whitaker, "Learning Deep Features for Automated Placement of Correspondence Points on Ensembles of Complex Shapes," In International Conference on Medical Image Computing and Computer-Assisted Intervention, pp. 185-193. Springer, Cham, 2017.

<!--
Requirements
=====================
 * Git (https://git-scm.com/)
 * CMake 2.6+ (http://www.cmake.org/)
 * Visualization ToolKit (VTK 5.10.1 recommended) (http://www.vtk.org/) --built with Qt and Imaging support!
 * Insight Toolkit (ITK 4.7.2 recommended) (http://www.itk.org/) -- built with USE_SYSTEM_VXL and provide VXL build path
 * VXL (https://github.com/vxl/vxl) [download the release tag github-migration]
 * Qt 4.8.* (http://www.qt.io/developers/) [Only needed for ShapeWorksView2]
 * Install gcc5.0+ [only required for ShapeWorksPost on Linux]
 * Windows 7+, OSX 10.9+, and OpenSuse 13.1 Recommended. Other platforms may work, but are not officially supported.

Running
=====================
<!--
The original documentation is located in this repository under <code>src/deprecated/documentation</code>.
You may read about the details of the algorithms there. 
<br/><br/>
Here are the basic instructions/pipeline for using the Shapeworks
Command Line Tools. You can follow the same steps for either the <code>examples/torus</code> or 
<code>examples/mickey</code>. Steps for the torus example are shown. <br/>
<!--
<h3>ShapeWorksGroom</h3>
```c++
cd examples/torus
../../build/bin/ShapeWorksGroom torus.groom.xml isolate hole_fill center auto_crop antialias fastmarching blur
```
This step may takes some time. Be sure that you are in the example directory (ie. examples/torus) when running
the tools. Also be sure you are providing the actual path of the executable. This is for each step.
<!--
<h3>ShapeWorksRun</h3>
```c++
../../build/bin/ShapeWorksRun torus.run.xml
```
<!--
<h3>ShapeWorksView2</h3>
```c++
../../build/bin/ShapeWorksView2
```
The visualizer is called "View2" since a previous and outdated version uses the FLTK library, not needed for View2.
You may also double click on the Executable. You will need to load the parameter file via a dialog box: 
<code>examples/torus/torus.analysis.xml</code>.<br/>
**OSX** Please note that if you run View2 from the command line, you will need to type the path to the package
interior: 
```c++
../../build/bin/ShapeWorksView2.app/Contents/MaxOS/ShapeWorksView2
```
<br/><br/>
You Should have a Qt window pop up that looks similar to the one below.
<!--
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
*Neighborhood Size* The neighborhood size (max vertex valence) used for isosurface reconstruction.
<br/><br/>
*Spacing* The spacing used for isosurface reconstruction.
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
building meshes. This speeds reconstruction, theoretically.
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
-->

