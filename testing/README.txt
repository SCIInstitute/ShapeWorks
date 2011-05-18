Procrustes Tests

This directory contains several test datasets for regression testing
of the Procrustes alignment code.  The data consist of the following:

1) Rotation test: rot??.pts. This data is a "cube" (8 points) rotated
around the x axis at intervals of 10 degrees.  Running ProcrustesAlign
on these datasets should align everything exactly.

2) Scale test: scale??.pts. This data is several "cubes" with different
scales.  Running ProcrustesAlign on these datasets should align
everything exactly.

3) Translation test: trans??.pts.  This data is several "cubes" of the
same size that have been translated from one another. Running
ProcrustesAlign on these datasets should align everything exactly.

