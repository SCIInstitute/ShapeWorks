#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd setorigin compare --name $DATA/setorigin1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd setorigin -x 1 compare --name $DATA/setorigin2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd setorigin -x -1 compare --name $DATA/setorigin3.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd setorigin -x 1 -y 1 -z 1 compare --name $DATA/setorigin4.nrrd
