#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd curvature --iterations -5 compareimage --name $DATA/curvaturefail.nrrd
