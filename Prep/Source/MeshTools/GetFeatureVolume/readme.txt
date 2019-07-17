 

-------------------------------
GetFeatureVolume 
-------------------------------


Compute a volumetric representation of fea files by propagating feature values (from fea files) from mesh surface to a narrowband surrounding the mesh

It uses a parameter file with the following tags"
    - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead 
    - mesh:      a list of ply file names for meshes to be processed
    - fids:      a list of corresponding fids files (in case with_fids = 1)
    - dist:       a list of corresponding distance transforms to indicate where to fill the feature volume, i.e. the narrowband
    - fea_per_mesh:      number of fea files to be processed per mesh
    - fea:      a list of fea files to be processed such that the first fea_per_mesh fea files correspond to the first mesh and so on.
    - narrow_band:     distance in physical coordinates from the mesh its inside and outside where we propagate feature values.

