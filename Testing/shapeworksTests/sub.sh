#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd sub --name $DATA/la-bin.nrrd compare  # images with diff dims cannot be subtracted
if [[ $? == 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/img1.nrrd sub --name $DATA/img2.nrrd  compare --name $DATA/img_diff.nrrd
