#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd translate --centerofmass 1 compare --name $DATA/centerofmass1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin.nrrd translate --centerofmass 1 compare --name $DATA/centerofmass2.nrrd
