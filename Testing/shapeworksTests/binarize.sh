#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd binarize compare --name $DATA/binarize1.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd binarize --min 0.0 --max 1.0 compare --name $DATA/binarize2.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd binarize --min -0.1 compare --name $DATA/binarize3.nrrd
