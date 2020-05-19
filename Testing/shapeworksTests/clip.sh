#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd clip --x1 0 --y1 1 --z1 1 --x2 20 --y2 55 --z2 75 compare --name $DATA/clip1.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd clip --x1 0 --y1 1 --z1 1 --x2 20 --y2 55 --z2 75 --val 3.14compare --name $DATA/clip2.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd clip --x1 0 --y1 1 --z1 1 --x2 20 --y2 55 --z2 75 --x1 0 --y1 1 --z1 1 compare --name $DATA/clip3.nrrd
