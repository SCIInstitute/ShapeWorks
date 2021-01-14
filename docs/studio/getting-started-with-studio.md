# ShapeWorks Studio

ShapeWorks Studio provides an all-in-one GUI based interface to the
ShapeWorks toolkit.  In Studio, you can Groom, Optimize and Analyze
your data all within a single interface

![ShapeWorks Studio Interface with Splash Screen](../img/studio/studio_interface.png)

## Splash Screen

The splash screen provides access to recent projects and an option to create a new blank project

## Data Module

The Data Module shows the project data table as well as notes and provides buttons to add or remove shapes.

![ShapeWorks Studio Data Module](../img/studio/studio_data.png)

## Groom Module

The Groom Module provides options to preprocess/groom the input data.  Options differ for binary segmentations and meshes.

You also have the option of skipping grooming if your data is already prepped.

![ShapeWorks Studio Groom Module](../img/studio/studio_groom.png)

## Optimize Module

The Optimize Module provides options for the particle based optimization.  After selecting options and clicking "Run Optimize", you can monitor the process of the optimization in real time.  Particle positions will be displayed on individual shapes as they progress.

![ShapeWorks Studio Optimize Module](../img/studio/studio_optimize.png)

## Analysis Module

After optimization is complete, you may analyze the data in Studio using the Analysis Module.  There are several panels available in the Analysis Module.

### View Panel

The view panel contains all of the options for controlling the analyis viewer options.

![ShapeWorks Studio Analysis View Panel](../img/studio/studio_analyze_view.png)

#### Group

The group mode allows for group comparison.  Each column of group values from the project can be selected and the differences between any two values in the groups can be displayed by selecting the group values in the left and right dropdown boxes.  A group difference display showing vectors and a colormap of distance can be shown using the "Diff -->" button.

![ShapeWorks Studio Analysis View Panel Group Display](../img/studio/studio_analyze_view_group.png)

#### Samples

The samples tab of the view panel allows us to view all of the samples, view a single sample by index, or view the median sample.

![ShapeWorks Studio Analysis View Panel Sample Display](../img/studio/studio_analyze_view_samples.png)

#### PCA

The PCA tab of the view panel shows reconstructed shapes along PCA modes of variation.

You can control the location along the PCA mode with the Standard Deviation Slider.  The extent of this slider is configurable in [Preferences](#preferences).

Displayed with each reconstruction is the Lambda value for this position on the slider and the Eigenvalue for this PCA mode.

![ShapeWorks Studio Analysis View Panel PCA Display](../img/studio/studio_analyze_view_pca.png)

### Metrics Panel

The Metrics panel shows the Explained Variance chart.  This chart shows how much of the population's variance is explained, cumulatively by each mode.  You can hover over each bar to see the individual values.

![ShapeWorks Studio Analysis Metrics Panel](../img/studio/studio_analyze_metrics.png)

### Surface Reconstruction Panel

The surface reconstruction panel provides options for the surface reconstruction method.  Using this option will provide smoother reconstructions.

![ShapeWorks Studio Analysis Surface Reconstruction Panel](../img/studio/studio_analyze_surface_reconstruction.png)

Below is an example of the difference in using this option.

![ShapeWorks Studio Surface Reconstruction Result](../img/studio/studio_analyze_surface_reconstruction_result.png)

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
* Isosurface visibility
* View mode (original/groomed/reconstructed)
* Center (whether to center each object based on center of mass)
* Number of views in lightbox (1,2,4,9,16,25,36,49,64)

Additionally, when feature volumes or meshes with scalars are loaded, the extended panel will be displayed:

![ShapeWorks Studio Viewer Extended Controls](../img/studio/studio_viewer_controls_extended.png)

This adds the feature map / scalar selection and a checkbox to apply a uniform scale to all viewers, vs local scale for each.

## Keyboard Shortcuts

The keyboard shortcuts are available from the Help -> Keyboard Shortcuts menu item.

![ShapeWorks Studio Keyboard Shortcuts](../img/studio/studio_keyboard_shortcuts.png)

### Point selection 

As shown in the keyboard shortcuts panel, you can hover over a particle and press the '1' key.  This will select this point on all viewers.

The selected point will become white and all other points will change to a color based on the distance to the chosen point.

![ShapeWorks Studio Particle Selection](../img/studio/studio_particle_selection.png)

To turn off this display, press '1' while hovering over something other than a particle.

Additionally, after selecting a point, you may select another point by pressing '2' while hovering.  This will set the first point to white, the second point to yellow and the rest to black.  This is an easy way to see how particlar particles vary over shapes.

![ShapeWorks Studio Particle Selection2](../img/studio/studio_particle_selection2.png)


## File menus

Studio's file menu is shown below.

![ShapeWorks Studio File Menu](../img/studio/studio_file_menu.png)

* New Project - Create a new project
* Open Project - Open a project from disk
* Save Project - Save project to disk
* Save Project As - Save a copy of the project to a new file
* Import Images... - Import new shapes
* Export - See export below
* Up to 4 recent projects are shown at the bottom

The File -> Export menu is shown below.

![ShapeWorks Studio Export Menu](../img/studio/studio_export_menu.png)

* Export Current Mesh - Export the current mesh as a VTK mesh file
* Export Mesh Scalars - Export the mesh scalars as a CSV file
* Export Eigenvectors - Export the PCA eigenvectors
* Export Eigenvalues - Export the PCA eigenvalues
* Export Variance Graph - Export the variance graph as a PNG file
* Export PCA Component Scores - Export the raw PCA component scores for each subject as a CSV file
* Export PCA Mode Points - Export the PCA mode points at each step of the PCA slider


## Project setup

ShapeWorks Studio and the `shapeworks` commands `groom` and `optimize` are using a new project format based off of XLSX.  The new spreadsheet format profiles a uniform, easy to edit, easy to organize way of inputing data into shapeworks for grooming, optimizing and analyzing data.

![ShapeWorks Studio Spreadsheet](../img/workflow/studio_ellipsoid_sheet.png)

Users input shapes (either binary segmentations or meshes) in the first sheet (or data sheet) using special column names to indiate type:


| Prefix | Description |
| --- | ----------- |
| segmentation_&lt;name&gt; | An input segmentation with a given name |
| group_&lt;name&gt; | An group column with categorical types |
| feature_&lt;name&gt; | A feature volume (such as an MRI or CT) |

<br/>
Many other columns will be filled in automatically by the ShapeWorks tools.

A more complete example is provided in Examples/Studio/FeatureMap/feature_map_example.xlsx :

![ShapeWorks Studio FeatureMap Example](../img/studio/studio_feature_map_example.png)

After completing groom and optimize steps, this worksheet will look like this:

![ShapeWorks Studio FeatureMap Example Complete](../img/studio/studio_feature_map_example_complete.png)

There will also be new worksheets with parameters from those tools and other studio settings.




