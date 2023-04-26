# ShapeWorks Cloud Optimize Module

**Note**: This documentation version could be outdated. [Click here](http://sciinstitute.github.io/ShapeWorks/dev/cloud/cloud-optimize-module.html) to get the most up-to-date version.

The optimize module provides options for particle-based optimization. The optimization particles can be viewed on the shapes in the [Shape Viewer](cloud-shape-viewer.md).

![ShapeWorks Cloud Optimize Module](../img/cloud/optimize_module.png){: width="400"}

To optimize the data, click the "Optimize" option. If you wish to re-optimize data, simply click "Reoptimize".

To restore the optimization parameters to the default values, click "Restore Defaults".

Optimization Parameters

| Parameter | Description |
| --- | ----------- |
| Number of Particles | The desired number of particles to be placed. ShapeWorks will produce the smallest power-of-2 number of particles greater than or equal to the given value |
| Relative Weighting | The relative weight of the correspondence term in the optimization steps. |
| Initial Relative Weighting | The relative weight of the correspondence term in the initialization steps to make sure that optimization steps start with evenly spaced particle distributions that cover the entire surfaces (usually in the order of ~0.1 or 0.01) |
| Starting Regularization | Sets the starting regularization value.  See details about regularization [here](../workflow/optimize.md#starting-and-ending-regularization) |
| Ending Regularization | Sets the ending regularization value. See details about regularization [here](../workflow/optimize.md#starting-and-ending-regularization)  |
| Iterations per Split | The number of iterations in the initialization step for each split (i.e., scale) |
| Optimization Iterations | Number of iterations for each optimization step. |
| Landmarks | Enable the use of landmarks as starting particles |
| Geodesic Distance | Enable the use of geodesic distance for particle to particle interactions.  This is much slower, but can help with some complex structures/ |
| Normals | Enable the use of surface normals as a correspondence feature |
| Normals Strength | Amount to scale surface normals feature to give it enough weight relative to XYZ. |
| Multiscale Mode | Enable multiscale optimization where each particle split level after the multiscale start number runs in both initialization and optimization modes |
| Multiscale Start | Number of particles to begin multscale mode |
| Procrustes | Enable use of procrustes registration as part of optimization |
| Procrustes Scaling | Enable scaling as part of procrustes to remove overall scale from the model |
| Procrustes Rotation/Translation| Enable procrustes rotation/translation to remove translation/rotation from the model |
| Procrustes Interval | Interval of iterations to run procrustes |
| Narrow Band | Number of voxels surrounding zero level for distance transform based optimizations.  Normally a value of 4 is sufficient |
