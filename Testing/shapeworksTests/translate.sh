#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd translate -x 10.0 -y 10.0 -z 10.0 compare --name $DATA/translate1.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd translate -x -10.0 -y -10.0 -z -10.0 compare --name $DATA/translate2.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin.nrrd translate --centerofmass 1 compare --name $DATA/translate_com.nrrd
