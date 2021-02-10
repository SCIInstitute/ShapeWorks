#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd binarize compareimage --name $DATA/binarize1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd binarize --min -0.1 compareimage --name $DATA/binarize2.nrrd
