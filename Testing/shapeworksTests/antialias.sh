#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd antialias compare --name $DATA/antialias1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd antialias --maxrmserror -1 compare --name $DATA/antialias2.nrrd
if [[ $? != 0 ]]; then exit -1; fi 
shapeworks readimage --name $DATA/1x2x2.nrrd antialias --iterations -1 compare --name $DATA/antialias3.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd antialias --maxrmserror 0.1 --iterations 5 compare --name $DATA/antialias4.nrrd
