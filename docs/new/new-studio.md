# New in ShapeWorksStudio

## Surface Reconstruction 

ShapeWorksStudio provides a particle-based surface reconstruction is can reconstruct high quality surface meshes with fewer number of particles. See [How to Analyze Your Shape Model?](../workflow/analyze.md#surface-reconstruction) for details about the method.


*With particle-based surface reconstruction, there is not need to optimize denser particle systems (i.e., with more particles) to reconstruct surface meshes with subvoxel accuracy*
![Reconstruction error vs number of particles](../img/new/recon_error.png)


*(old) VTK-based surface reconstruction*
<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/vtk_reconstruction.mp4" autoplay muted loop controls style="width:100%"></p>

*(new) particle-based surface reconstruction*
<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/particles_reconstruction.mp4" autoplay muted loop controls style="width:100%"></p>



## Dynamic Loading

ShapeWorksStudio support load-on-demand, which makes it more scalable by supporting loading in order of 100s of samples. For instance, an old small project that took 10+ seconds to load now starts instantly in Studio.


*(old) without dynamic loading*

<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/studio_load_old.mp4" autoplay muted loop controls style="width:100%"></p>


*(new) with dynamic loading*

<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/studio_load_new.mp4" autoplay muted loop controls style="width:100%"></p>

## Live Particles Movement 

ShapeWorksStudio allows for live introspection of the correpsondence placement optimization process and the ability to abort the optimization at any time (e.g., changing algorithmic parameters).


<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/studio_optimize_live.mp4" autoplay muted loop controls style="width:100%"></p>

## Feature Maps

ShapeWorksStudio has the ability to integrate **feature maps**.  A **feature map** is a 3d image volume that contains scalar values to be associated with each shape’s surface.  For example, this could be raw or processed CT/MRI data.  The feature map can be displayed for each surface by choosing the desired feature map in the feature map combobox at the bottom of the screen. 
After the correspondence is generated, the average feature map can be displayed on the mean shape in the analysis tab.

<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/studio_feature_map.mp4" autoplay muted loop controls style="width:100%"></p>




