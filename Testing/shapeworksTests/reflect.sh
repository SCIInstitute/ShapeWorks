#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd reflect --x 1 --y -1 --z 1 compare --name $DATA/reflect1.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd reflect compare --name $DATA/reflect2.nrrd
