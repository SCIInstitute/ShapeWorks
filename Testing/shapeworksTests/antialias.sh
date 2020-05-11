#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd antialias writeimage --name $DATA/antialias1.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd antialias --maxrmserror -1 writeimage --name $DATA/antialias2.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd antialias --numiterations -1 writeimage --name $DATA/antialias3.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd antialias --numlayers -1 writeimage --name $DATA/antialias4.nrrd
