#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd curvature compare --name $DATA/curvature1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd curvature --iterations 5 compare --name $DATA/curvature2.nrrd
