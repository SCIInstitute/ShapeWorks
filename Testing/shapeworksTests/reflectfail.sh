#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd reflect --x -1 --y -1 --z 1 compare --name $DATA/reflectfail.nrrd
