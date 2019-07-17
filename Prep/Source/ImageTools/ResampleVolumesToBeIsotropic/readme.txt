 

-------------------------------
ResampleVolumesToBeIsotropic 
-------------------------------

A command line tool that resamples given mri/binary volumes to have isotropic voxel spacing.

It uses the following input arguments:
  -inFilename               The filename of the input image to be resampled.
  -outFilename              The filename of the output resampled image.
  -isBinaryImage            A flag to treat the input image as a binary image (specialized resampling pipeline) [default disabled].
  -isoSpacing               The isotropic spacing in all dimensions.
  -sizeX                    Image size in x-direction (optional, if set to 0, the size is autmatically estimated from the input image).
  -sizeY                    Image size in y-direction (optional, if set to 0, the size is autmatically estimated from the input image).
  -sizeZ                    Image size in z-direction (optional, if set to 0, the size is autmatically estimated from the input image).
  -isCenterImageOn          A flag to center the image, i.e. change the origin in the image header to the physcial coordinates of the first voxel (lower left corner) [default disabled].
