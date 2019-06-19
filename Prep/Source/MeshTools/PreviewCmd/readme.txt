 

-------------------------------
PreviewCmd 
-------------------------------

This is a commandline tool that encapsulate preview-based triangular mesh processing tools including decimation, smoothing and fixing, it has the following inputs:
    --inFile: the input vtk filename of the mesh to be processed.
    --outFile: the output vtk filename.
    --fixWinding: do element winding fix (default: 1)
    --decimate: perform mesh decimation (default: 1)
    --doLaplacianSmoothingBeforeDecimation: perform laplacian smoothing before decimation (default: 1)
    --doLaplacianSmoothingAfterDecimation: perform laplacian smoothing after decimation (default: 1)
    --smoothingLambda: laplacian smoothing lambda  (default: 0.5)
    --smoothingIterations: laplacian smoothing number of iterations  (default: 1)
    --decimationPercentage: percentage of target number of clusters/vertices  (default: 0.5)
