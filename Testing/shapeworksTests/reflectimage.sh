#! /bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd reflectimage --axis Z compareimage --name $DATA/reflect1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin.nrrd reflectimage --axis X compareimage --name $DATA/reflect2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd reflectimage compareimage --name $DATA/reflectfail.nrrd
if [[ $? != 1 ]]; then exit -1; fi
