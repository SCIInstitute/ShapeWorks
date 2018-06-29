 

-------------------------------
ICPRigid3DImageRegistration 
-------------------------------

A command line tool that performs iterative closed point (ICP) 3D rigid registration on a pair of images.

It uses the following input arguments:
Input:
    -targetDistanceMap: the distance map of target image.
    -sourceDistanceMap: the distance map of source image.
    -sourceSegmentation: the segmentation of source image.
    -isoValue: as we need to get point set from surface for ICP, this iso value is required to get the isosurface. The default value is 0.0.
    -icpIterations: the number of iterations user want to run.

Output:
    -solutionSegmentation: the filename of the aligned segmentation of source image.



