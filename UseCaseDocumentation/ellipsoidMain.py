# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example we provide a full pipeline with an exple dataset of axis 
aligned ellipsoids. We provide two different datasets for two different 
senarios, prepared data consists of the binary images which do not require 
alignment/resampling/cropping as pre-processing and only require conversion to
signed distance transforms before running the ShapeWorks particle based 
optimization. Second is the unprepped data which requires additional 
pre-processing steps before it can be fed into the optimization. 

This example is set to serve as a test case for new ShapeWorks users, and each
step is explained in the shapeworks including the pre-processing, the 
optimization and, the post ShapeWorks visualization.

First import the necessary modules
"""

import numpy as np
import matplotlib.pyplot as plt
import os
import csv
from ellipsoidUtils import *

"""
First we decide which data we want to use for the example, prepped or 
unprepped. Following DATA_FLAG when set to 0 selects the prepped data and when
set to NOT 0 will go with unprepped data. Default is 1.
"""

DATA_FLAG = 1

"""
Unzip the data for this tutorial.

The data is inside the Ellipsoids.zip, run the following function to unzip the 
data and create necessary supporting files. The files will be Extracted in a
newly created Directory TestEllipsoids.
This data both prepped and unprepped are binary images of ellipsoids varying
one of the axes while the other two are kept fixed. 
"""

fileList = extract_zip_createFiles(DATA_FLAG)

"""
Most of the following steps even though wrapped in python functions are using
the undelying c++ code, for whihc we need to call the source paths to the 
binaries. This step should be common for any use of a function in ShapeWorks.
__This requires the full ShapeWorks to be correctly built/downloaded!__
"""

binpath="source_setup.txt" # ideally we should generate this
exportPath = "source " + binpath
os.system(exportPath)

"""
For the unprepped data the first few steps are 
-- Isotropic resampling
-- Padding
-- Center of Mass Alignment
-- Rigid Alignment
-- Cropping
"""

"""
We convert the scans to distance transforms, this step is common for both the 
prepped as well as unprepped data
"""
