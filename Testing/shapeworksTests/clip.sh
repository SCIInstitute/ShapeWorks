#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd clip --x1 1 --y2 1 --z3 1 compare --name $DATA/clip1.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd clip --x1 1 --y2 1 --z3 1 --val 3.14 compare --name $DATA/clip2.nrrd
