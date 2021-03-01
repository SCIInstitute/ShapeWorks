#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd computedt compareimage --name $DATA/computedt1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd computedt --isovalue 1.0 compareimage --name $DATA/computedt2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd computedt --isovalue -1.0 compareimage --name $DATA/computedt3.nrrd
