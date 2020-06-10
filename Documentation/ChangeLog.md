ShapeWorks 5.4.0 - 2020-05-27

* Studio: New XLSX based file format.  See Examples/Studio/ellipsoid.xlsx for an example
* Studio: Improved scalability.  Files now loaded on-demand as necessary.
* Lower memory footprint and faster optimization: ShapeWorks is now using [OpenVDB](https://www.openvdb.org/), a more memory-efficient data structure, for signed distance transforms. Along with other refactoring and code optimizations, ShapeWorks now uses *85% less memory* (from 57.09GB to 9.67GB in one use case). Additionally, the particles optimizer is now *2X faster*.
* Lower memory footprint for estimating correspondences for new shapes on existing shape models: For <fixed_domains> usage, only distance transforms for the new shapes will be loaded. 

ShapeWorks 5.3.0 - 2020-02-20

* Use cases: Updated femur use case with interactive cutting plane
* Use cases: Added fixed domains use case that demonstrates adding a single shape to an existing shape model
* Use cases: Added ellipsoid evaluation use that demonstrates quantitative evaluation of the ellipsoid
* Studio: Replaced bar chart with explained variance chart
* New commands: Added commands to the `shapeworks` executable to quantitatively evaluate shape models: read-particle-system, compactness, generalization, specificity
* New shapeworks executables commands: read-image, write-image, antialias, isoresample, binarize, recenter-image, pad.
* New build method: Separate build_dependencies script (see Documentation/Build/BUILD.md)

ShapeWorks 5.2.2 - 2020-01-09

* Returned to superbuild.sh build process
* Fixed non-Qt build
* Fixed Windows conda_installs.bat
* Studio: Fixed import and processing of non-RAI image volumes
* Studio: Fix centering of groomed and reconstructed volumes

ShapeWorks 5.2.1 - 2019-11-09

* Fix examples, binaries

ShapeWorks 5.2.0 - 2019-11-07

* Studio: Added live particle optimization updates
* Studio: Added "Stop optimization" button
* Studio: Fixes crashes on exit during optimizations (clean shutdown of threads)
* Studio: Added support for reading View2 parameter files
* Studio: Added legacy (View2) surface reconstructor
* Studio: Replaced optimization libary with same used by ShapeWorksRun
* Use cases: Added a femur use case that demonstrates grooming surface meshes along with imaging data for shape modeling
* Use cases: Added a left atrium use case that demonstrates grooming images data along with shape data and build multi-scale shape models
* Use cases: New portal downloadable example data (ellipsoid, left atrium, and femurs)
* Windows installer and binary releases for Mac and Linux
* CMake-based superbuild with all dependencies built automatically

