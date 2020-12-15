#! /bin/bash

shapeworks readimage --name $DATA/input.nrrd warpimage --source $DATA/source.particles --target $DATA/target.particles compareimage --name $DATA/warp.nrrd
