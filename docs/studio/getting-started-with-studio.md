# Getting Started With ShapeWorks Studio

ShapeWorks Studio provides an all-in-one GUI based interface to the
ShapeWorks toolkit.  In Studio, you can manage input Data, Groom, Optimize and Analyze your data all within a single interface.

## Splash Screen

The splash screen provides access to recent projects and an option to create a new blank project.

![ShapeWorks Studio Interface with Splash Screen](../img/studio/studio_interface.png)

## Data Module

The Data Module shows the project data table, landmarks, constraints as well as user-defined/saved notes. This module enables you to add and remove shapes (segmentations and surface meshes) to your cohort, define anatomical landmarks, place cutting planes and free form constraints for surface exclusion.

![ShapeWorks Studio Data Module](../img/studio/studio_data.png){: width="300" }

See [Studio Data Module](studio-data.md) for more information.

## Groom Module

The Groom module provides options to preprocess/groom the input data.  Options differ for binary segmentations and meshes. You also have the option of skipping grooming if your data is already prepped.

![ShapeWorks Studio Groom Module](../img/studio/studio_groom.png){: width="300" }

See [Studio Groom Module](studio-groom.md) for more information.

## Optimize Module

The Optimize module provides options for the particle-based optimization.  After selecting options and clicking "Run Optimize", you can monitor the process of the optimization in real-time.  Particle positions will be displayed on individual shapes as they progress where color encodes surface correspondences across different shapes in your cohort.

![ShapeWorks Studio Optimize Module](../img/studio/studio_optimize.png){: width="300" }

See [Studio Optimize Module](studio-optimize.md) for more information.

## Analysis Module

After optimization is complete, you may analyze the data in Studio using the Analysis module.  There are several panels available in the Analysis Module.

See [Studio Analyze Module](studio-analyze.md) for more information.

## Preferences

Studio provides a number of preference options.

![ShapeWorks Studio Preferences](../img/studio/studio_preferences.png)

| Option | Description |
| --- | ----------- |
| Color Scheme | Base foreground and background colors for viewers |
| Orientation Marker Type | Medical, Triad, or None |
| Orientation Marker Corner | Upper Right, Upper Left, Lower Left, Lower Right |
| PCA Range | Number of standard deviations for PCA slider |
| Mesh Cache Enabled | Whether to use a cache of meshes |
| Mesh Cache Epsilon | Determination of how much different a mesh must be before it is considered a different mesh, for caching purposes |
| Mesh Cache Memory to Use | Percentage of phyisical memory to allow mesh cache to use |
| Mesh Parallel Reconstruction | Whether to use parallelized mesh reconstruction |
| Mesh Parallel Number of Threads | How many threads to use for parallelized mesh reconstruction |
| Log Location | A display of where the current log file is located |

<br/>

## Viewer Controls

At the bottom of the viewers are the controls for the viewers:

![ShapeWorks Studio Viewer Controls](../img/studio/studio_viewer_controls.png)

Each button has tooltips with descriptions.  The controls from left to right are

* Autoview / reset view
* Glyph visibility with dropdown for glyph size and detail
* Isosurface visibility with dropdown for surface opacity (of each domain)
* View mode (original/groomed/reconstructed)
* Center (whether to center each object based on center of mass)
* Number of views in lightbox (1,2,4,9,16,25,36,49,64)

Additionally, when feature volumes or meshes with scalars are loaded, the extended panel will be displayed:

![ShapeWorks Studio Viewer Extended Controls](../img/studio/studio_viewer_controls_extended.png)

This adds the feature map / scalar selection and a checkbox to apply a uniform scale to all viewers, vs local scale for each.  Additionally, the auto range can be turned off and specified range can be set.

## Keyboard Shortcuts

The keyboard shortcuts are available from the Help -> Keyboard Shortcuts menu item.

![ShapeWorks Studio Keyboard Shortcuts](../img/studio/studio_keyboard_shortcuts.png)

### Point selection 

As shown in the keyboard shortcuts panel, you can hover over a particle and press the '1' key.  This will select this point on all viewers.

The selected point will become white and all other points will change to a color based on the distance to the chosen point.

![ShapeWorks Studio Particle Selection](../img/studio/studio_particle_selection.png)

To turn off this display, press '1' while hovering over something other than a particle.

Additionally, after selecting a point, you may select another point by pressing '2' while hovering.  This will set the first point to white, the second point to yellow and the rest to black.  This is an easy way to see how particlar particles vary and correspond across shapes.

![ShapeWorks Studio Particle Selection2](../img/studio/studio_particle_selection2.png)


## File menus

Studio's file menu is shown below.

![ShapeWorks Studio File Menu](../img/studio/studio_file_menu.png){: width="200" }

* `New Project` - Create a new project
* `Open Project` - Open a project from disk
* `Save Project` - Save project to disk
* `Save Project As` - Save a copy of the project to a new file
* `Import Shapes...` - Import new shapes
* `Export` - See export below
* Up to 4 recent projects are shown at the bottom

The File -> Export menu is shown below.

![ShapeWorks Studio Export Menu](../img/studio/studio_export_menu.png){: width="200" }

* Export Current Mesh - Export the current mesh as a VTK mesh file
* Export Mesh Scalars - Export the mesh scalars as a CSV file
* Export Eigenvectors - Export the PCA eigenvectors
* Export Eigenvalues - Export the PCA eigenvalues
* Export Variance Graph - Export the variance graph as a PNG file
* Export PCA Component Scores - Export the raw PCA component scores for each subject as a CSV file
* Export PCA Mode Points - Export the PCA mode points at each step of the PCA slider


## Project setup
ShapeWorks Studio and the `shapeworks` commands `groom` and `optimize` are using a new project format based off of XLSX. The new spreadsheet format profiles a uniform, easy to edit, easy to organize way of inputing data into shapeworks for grooming, optimizing and analyzing data.
See [Here](../workflow/parameters.md) for more details regaring the project sheet and the python API available to generate the project sheet. 

