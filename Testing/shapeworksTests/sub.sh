#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd subtract --name $DATA/la-bin.nrrd compare  # images with diff dims cannot be subtracted
if [[ $? == 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/img1.nrrd subtract --name $DATA/img2.nrrd  compare --name $DATA/img_diff.nrrd
if [[ $? != 0 ]]; then exit -1; fi 
shapeworks readimage --name $DATA/la-bin.nrrd subtract --value 3.14 compare --name $DATA/la-bin-minus-pi.nrrd
if [[ $? != 0 ]]; then exit -1; fi 
shapeworks readimage --name $DATA/la-bin.nrrd subtract --name $DATA/la-bin.nrrd --value 3.14 compare --name $DATA/la-bin-doubled-minus-pi.nrrd
