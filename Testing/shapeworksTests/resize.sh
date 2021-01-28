#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd resize compareimage --name $DATA/1x2x2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la1-small.nrrd resize --sizex 2 compareimage --name $DATA/resize2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la1-small.nrrd resize --sizex 96 --sizey 96 --sizez 12 compareimage --name $DATA/resize3.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la1-small.nrrd resize --sizex 24 --sizey 120 --sizez 6 compareimage --name $DATA/resize4.nrrd
