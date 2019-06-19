 

-------------------------------
TopologyPreservingSmoothing 
-------------------------------

A parameter file based tool that smoothes distance transforms while preserving shape's topology.


parameter file tags are as follows:
 -inputs               The filenames of the input distance transforms to be smoothed.
 -dtFiles              The filenames of the output smoothed distance transforms.
 -outputs              The filenames of the output smoothed isosurface images.
 -verbose                  Show each intermediate step [default 0].
 -isoValue                 Isovalue to be used to define the surface in the input distance transform [default 0.0].
 -smoothing_iterations      Number of iterations to perform smoothing [default 10].
 -alpha                    Smoothing parameter in I' = (max-min). \frac{1}{1+exp(-\frac{1-\beta}{\alpha)} + min [default 10.5].
 -beta                     Smoothing parameter in I' = (max-min). \frac{1}{1+exp(-\frac{1-\beta}{\alpha)} + min [default 10.0].
 -propagationScale         The PropagationScaling parameter can be used to switch from propagation outwards (POSITIVE) versus propagating inwards (NEGATIVE). [default 20.0].

