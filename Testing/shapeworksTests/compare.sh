#! /bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd compareimage --name $DATA/la-bin.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd compareimage --name $DATA/1x2x2-diff.nrrd --tolerance 1.0
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin.nrrd compareimage --name $DATA/1x2x2.nrrd
if [[ $? == 0 ]]; then exit -1; fi
