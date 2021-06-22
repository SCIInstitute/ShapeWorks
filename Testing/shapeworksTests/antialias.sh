#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd antialias compareimage --name $DATA/antialias1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd antialias --maxrmserror -1 compareimage --name $DATA/antialias2.nrrd
if [[ $? != 0 ]]; then exit -1; fi 
shapeworks readimage --name $DATA/1x2x2.nrrd antialias --iterations -1 compareimage --name $DATA/antialias5.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd antialias --maxrmserror 0.1 --iterations 5 compareimage --name $DATA/antialias4.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd antialias --layers -1 compareimage-image --name $DATA/antialiasfail.nrrd
if [[ $? != 1 ]]; then exit -1; fi
