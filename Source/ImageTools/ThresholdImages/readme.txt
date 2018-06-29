------------------------
ThresholdImages
------------------------

A command line tool that threholds a given image into a binary label based on upper and lower intensity bounds given by the user.....

    --inFilename = Input image file path.
    --outFilename = The filename of the output threshold image.
    --lowerThresholdLevel = The lower threshold level (optional, default = FLT_MIN)
    --upperThresholdLevel = The upper threshold level (optional, default = FLT_MAX)
    --insideValue = The inside pixel value after threshold
    --outsideValue = The outside pixel value after threshold