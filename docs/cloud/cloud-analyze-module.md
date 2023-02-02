# ShapeWorks Cloud Analyze Module

The analyze module can be viewed after a successful optimization job. There are three panels in the analyze module, [View PCA](#view-pca), [Charts](#charts), [Shape Viewer](#shape-viewer).

![ShapeWorks Cloud Analyze Module](../img/cloud/analyze_module.png){: width="400"}

## View PCA

The View PCA panel shows reconstructed shapes (surface meshes) along PCA modes of variation. Modes of variations are shape parameters learned/discovered from your data that describes shape variations. You can control the location along the PCA mode with the Standard Deviation slider.  
Values displayed:

`Lambda` - This shows the PCA loading of current position of the slider.  The middle of the slider, at the mean value, will be 0.  The extent of lambda is defined by the number of standard deviations of the slider as described above.  At standard deviation of 1.0, it will be the square root of the mode's eigenvalue.

`Eigenvalue` - This shows the eigenvalue of the currently selected mode.  This is a unitless measure of how much variance is explained by this mode.  The modes are sorted in decreasing order of eigenvalue, corresponding to the most variance explained first.

## Charts

The analysis charts provide visualizations of the shape evaulation data. For more information about shape evaluation, see [Shape Model Evaluation](../new/ssm-eval.md). You can zoom into a chart with the mouse scroll wheel and click and drag to pan.

Chart Options

| Tool | Description |
| --- | ----------- |
| Save as Image | Save the chart as an SVG |
| View Data | View the chart data in table format |
| View Data: Download | Download the chart data as a .csv file |
| View Data: Copy to Clipboard | Copy the chart data in CSV format |
| Zoom | Zoom into an area by mouse rectangle selection |
| Zoom Reset | Reset the zoom level to zero |
| Restore | Restore the chart to default view |

## Shape Viewer

While in the analyze tab, the Shape Viewer displays the mean (average) shape generated.

## Show Difference From Mean

![Show difference from mean](../img/cloud/difference_from_mean.png){: width="300"}

This checkbox allows for visualization from any surface to the mean. The arrows are placed on each particle. The direction and color of the vector/surface indicate the difference from the mean shape.
