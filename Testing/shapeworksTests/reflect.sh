#! /bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd reflect --axis Z compareimage --name $DATA/reflect1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin.nrrd reflect --axis X compareimage --name $DATA/reflect2.nrrd
