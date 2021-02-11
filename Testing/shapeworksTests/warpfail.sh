#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd warpimage --source $DATA/source.particles compareimage --name $DATA/warpfail.nrrd
