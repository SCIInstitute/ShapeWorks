#! /bin/bash

shapeworks readimage --name $DATA/input.nrrd warpimage --source $DATA/source.particles --target $DATA/target.particles compareimage --name $DATA/warp1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd warpimage --source $DATA/source.particles compareimage --name $DATA/warpfail.nrrd
if [[ $? != 1 ]]; then exit -1; fi
