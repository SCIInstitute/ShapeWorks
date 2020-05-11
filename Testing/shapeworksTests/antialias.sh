#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd antialias compare --name $DATA/antialias1.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd antialias --maxrmserror -1 compare --name $DATA/antialias2.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd antialias --numiterations -1 compare --name $DATA/antialias3.nrrd
