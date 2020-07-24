#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd resample compare --name $DATA/resample1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd resample --isospacing 1.5 compare --name $DATA/resample2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd resample --spacex 1.5 --spacey 1.5 --spacez 1.5 compare --name $DATA/resample3.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la1-small.nrrd resample --spacex 0.9 --spacey 3.14159 --spacez 2.5 compare --name $DATA/resample4.nrrd
