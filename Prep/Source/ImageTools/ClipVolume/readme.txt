------------------------------- 
Slice 3D Volume Using Cutting Planes 
------------------------------- 
Takes set of .nrrd volumes and corresponding cutting planes, and chops the volume accordingly 
It uses a parameter file with the following tags 
    - num_shapes: number of volumes to be processed (put this correctly) 
    - inputs: paths to input files 
    - outputs: paths to output files 
    - cutting_planes : Set of cutting planes, expressed as 3 3D points separated by spaces 
    example : 1 2 -3 0 34 102 -23.4 2 0.44 