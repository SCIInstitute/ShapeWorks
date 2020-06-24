#! /bin/bash

shapeworks readimage --name $DATA/input.nrrd warpimage --source_landmarks $DATA/source.particles --target_landmarks $DATA/target.particles compare --name $DATA/warp.nrrd
