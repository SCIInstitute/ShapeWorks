#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd blur compareimage --name $DATA/blur1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd blur --sigma 1.0 compareimage --name $DATA/blur2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd blur --sigma -1.0 compareimage --name $DATA/blur3.nrrd
