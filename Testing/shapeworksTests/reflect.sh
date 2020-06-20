#! /bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd reflect --axis Z compare --name $DATA/reflect1.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin.nrrd reflect --axis X compare --name $DATA/reflect2.nrrd
