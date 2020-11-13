#! /bin/bash

shapeworks readimage --name $DATA/input.nrrd warpimage --source $DATA/source.particles --target $DATA/target.particles compare --name $DATA/warp1.nrrd
