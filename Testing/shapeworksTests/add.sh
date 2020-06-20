#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd add --name $DATA/la-bin.nrrd compare  # images with diff dims cannot be added
if [[ $? == 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin.nrrd add --name $DATA/la-bin.nrrd  compare --name $DATA/la-bin-doubled.nrrd
