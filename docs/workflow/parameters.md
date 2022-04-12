![Optimization Parameters](../img/workflow/params.png)

### XML Parameter File
        
Here is the list of the parameters to be included in the `<parameters.xml>` file.

* `<inputs>`: List of surface meshes or distance transforms (i.e., groom stage output) that comprises your dataset.
* `<output_dir>`:  The directory to save the output produced by the ShapeWorks optimization.
* `<domain_type>`: (default: image) The type of the domain in `<inputs>`, `image` for signed distance transforms, and `mesh` for triangular surface meshes.
* `<domains_per_shape>`: (default: 1) The number of domains for anatomies with multiple structures (domains), e.g., joints. The list of `<inputs>` should be ordered to list a consistent order of the domains (surface mesh or distance transform) of each shape (e.g., shape1-domain1, shape1-domain2, shape2-domain1, shape2-domain2 ... etc.).
* `<narrow_band>`: (default: 4.0 of `<fixed_domains>` is not active) The off-surface distance (in voxel units, largest spacing dimension) used to truncate (zero out) distance values for signed distance transforms beyond the narrow band radius (i.e., a narrow band of 4.0 preserve distance values within +/- 4.0 voxels off the surface). This is used to reduce the memory footprint required for keeping volumetric distance transforms in memory. If you get an error that particles are shooting outside the narrow band, please consider increasing this value. Narrow banding is disabled if `<fixed_domains>` is active. This does not significantly affect the memory footprint since distance transforms of the fixed domains are not loaded in memory.
* `<number_of_particles>`: The desired number of particles to be placed. ShapeWorks will produce the smallest power-of-2 number of particles greater than or equal the given `<number_of_particles>`.
* `<iterations_per_split>`: (default: 1000) The number of iterations in the initialization step for each split (i.e., scale). 
* `<optimization_iterations>`: (default: 2000) Number of iterations for each optimization step.
* `<save_init_splits>`: (default: 1) A flag to save the particles for each split in the initialization steps. 
* `<use_xyz>`: (default: 1) A flag to enable using the xyz coordinates for particles as a feature for correspondence.
* `<use_normals>`: (default: 0) A flag to consider surface normals (along with particles positions, i.e., `<use_xyz> 1 </use_xyz>`) as a correspondence feature.
* `<attribute_scales>`: A vector of weights that scale each dimension considered in the correspondence entropy. For example, if only xyz coordinates are used for particles, so `<attribute_scales>` is a vector of three weights that scale the x- and y- and z- coordinates of the particle (default = 1). If surface normals are also used, `<attribute_scales>` should be a vector of 6 entries, 3 for the xyz coordinates, and 3 for the surface normal (usually in scale of 10), assuming 3D anatomies.
* `<use_shape_statistics_after>`: (default: -1) (e.g., 32 or 64) The number of particles after which to use shape space entropy (i.e., the covariance structure) in the initialization and optimization steps. Use -1 if you want shape statistics only used when the number of particles reach the desired `<number_of_particles>`. Hence, all initialization and optimization steps will use mean energy till the `<number_of_particles>` is reached.
* `<starting_regularization>`: (default: 1000) Sets the starting regularization value (usually high value).
* `<ending_regularization>`: (default: 1.0) Sets the ending regularization value (usually small value less than 1).
* `<recompute_regularization_interval>`: (default: 1) Skip interval (i.e., number of iterations) to exponentially decay the regularization value.
* `<initial_relative_weighting>`: (default: 0.05) The relative weight of the correspondence term in the initialization steps to make sure that optimization steps start with evenly spaced particle distributions that cover the entire surfaces (usually in the order of ~0.1 or 0.01).
* `<relative_weighting>`: (default: 1) The relative weight of the correspondence term in the optimization steps.
* `<procrustes_scaling>`: (default: 1) A flag to enable factoring out scaling in the shape space when performing Procrustes alignment.
* `<procrustes_interval>`: (default: 3) Number of iterations (interval) between performing Procrustes alignment, use 0 to turn Procrustes off.
* `<mesh_based_attributes>`: (default: 0) A flag that should be enabled when `<use_normals>` is enabled to cache and interpolate surface normals using isosurfaces.
* `<keep_checkpoints>`: (default: 0) A flag to save the shape (correspondence) models through the initialization/optimization steps for debugging and troubleshooting.  
* `<checkpointing_interval>`: (default: 50) The interval (number of iterations) to be used to save the checkpoints.
* `<verbosity>`: (default: 0) '0' : almost zero verbosity (error messages only), '1': minimal verbosity (notification of running initialization/optimization steps), '2': additional details about parameters read from xml and files written, '3': full verbosity.
* `<adaptivity_mode>`: (default: 0) Used to change the expected behavior of the particles sampler, where the sampler is expected to distribute evenly spaced particles to cover all the surface. Currently, 0 is used to trigger the update project method of cutting planes.
* '<cutting_plane_counts>`: Number of cutting planes for each shape if constrained particle optimization is used.
* `<cutting_planes>`: A list of cutting planes to be used for all shapes. Each cutting plane is defined by three points in an order that indicates the plane's normal, i.e., the direction where particles are allowed to be distributed.

### Parameter Dictionary in Python

In `Examples/Python/<use-case-name.py>`, we use a python dictionary to expose important algorithmic parameters and automatically generate the XML parameter file. 

Below is a list of the currently exposed algorithmic parameters. All the keys of this parameter dictionary correspond to the [XML tags](#xml-parameter-file), except for `"normal_weight"` that sets the `<attribute_scales>` of the surface normal vector.

```
{
        "number_of_particles": 1024,
        "use_shape_statistics_after": 32, 
        "use_normals": 0, 
        "normal_weight": 0.0, 
        "checkpointing_interval" : 10000, 
        "keep_checkpoints" : 0, 
        "iterations_per_split" : 4000, 
        "optimization_iterations" : 500, 
        "starting_regularization" : 10, 
        "ending_regularization" : 1, 
        "recompute_regularization_interval" : 2,
        "domains_per_shape" : 1,
        "domain_type" : 'mesh',
        "relative_weighting" : 10,
        "initial_relative_weighting" : 1,
        "procrustes_interval" : 1,
        "procrustes_scaling" : 1,
        "save_init_splits" : 0,
        "verbosity" : 2,
}
```


## Project setup

ShapeWorks Studio and the `shapeworks` commands `groom` and `optimize` are using a new project format based off of XLSX.  The new spreadsheet format profiles a uniform, easy to edit, easy to organize way of inputing data into shapeworks for grooming, optimizing and analyzing data.

![ShapeWorks Studio Spreadsheet](../img/workflow/studio_ellipsoid_sheet.png)

Users input shapes (either binary segmentations or meshes) in the first sheet (or data sheet) using special column names to indiate type:


| Prefix | Description |
| --- | ----------- |
| shape_&lt;name&gt; | An input shape (segmentation/mesh) with a given name.<br>* Specify multiple shape_&lt;name&gt; columns to input multiple anatomies for each subject |
| group_&lt;name&gt; | An group column with categorical types |
| feature_&lt;name&gt; | A feature volume (such as an MRI or CT) |

<br/>
Many other columns will be filled in automatically by the ShapeWorks tools.

A more complete example is provided in Examples/Studio/FeatureMap/feature_map_example.xlsx :

![ShapeWorks Studio FeatureMap Example](../img/studio/studio_feature_map_example.png){: width="300" }

After completing groom and optimize steps, this worksheet will look like this:

![ShapeWorks Studio FeatureMap Example Complete](../img/studio/studio_feature_map_example_complete.png)

There will also be new worksheets with parameters from those tools and other studio settings.

