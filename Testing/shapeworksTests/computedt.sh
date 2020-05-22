#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd computedt compare --name $DATA/computedt1.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd computedt --isovalue 1.0 compare --name $DATA/computedt2.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd computedt --isovalue -1.0 compare --name $DATA/computedt3.nrrd
