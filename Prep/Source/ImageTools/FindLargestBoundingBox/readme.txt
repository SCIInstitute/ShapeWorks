 

-------------------------------
FindLargestBoundingBox 
-------------------------------

A command line tool that o compute largest bounding box size given a set of images.

It uses the following input arguments:
 -inFilename - A text file with the file names for which the largest size has to be computed.
- outPrefix - output prefix to be used to save the parameters for the estimated bounding box
- paddingSize - number of extra voxels in each direction to pad the largest bounding box, checks agains min image size is performed to make sure that this padding won't get out of bounds for the smallest image in the file names provides
