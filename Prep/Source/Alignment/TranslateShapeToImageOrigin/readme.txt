 

-------------------------------
TranslateShapeToImageOrigin
-------------------------------

A command line tool that performs translational alignment of a given shape image based on either its center of mass or a given 3d point.

It uses the following input arguments:
-inFilename               The filename of the input shape to be transformed.
-outFilename              The filename of the output transformed shape.
-MRIinFilename            The assoicated image filename to be transformed.
-MRIoutFilename           The filename of the output transformed image.
-useCenterOfMass          A flag to transform image using center of mass.
-centerX                  x-coordinate of a user-defined center point.
-centerY                  y-coordinate of a user-defined center point.
-centerZ                  z-coordinate of a user-defined center point.
-parameterFilename        The filename to store the transform parameters
