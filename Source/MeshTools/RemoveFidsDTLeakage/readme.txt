 

-------------------------------
RemoveFidsDTLeakage 
-------------------------------


This tool can be used a postprocessing for fids distance trasnform to fix voxels that are mis-signed as in or out of the isosurface in fids computation (mainly due to irregular triangulation

It uses a parameter file with the following tags"
    - fids_dist:      a list of distance transforms computed via fids
    - approx_dist:       the corresponding approximate distances (from rasterization then dt computation)
    - out_dist:       output distance transform filenames
    
