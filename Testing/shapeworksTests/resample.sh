#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd resample compare --name $DATA/resample1.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd resample --isospacing 1.5 compare --name $DATA/resample2.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd resample --spacex 1.5 --spacey 1.5 --spacez 1.5 compare --name $DATA/resample3.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd resample --sizex 1.5 --sizey 1.5 --sizez 1.5 compare --name $DATA/resample4.nrrd
