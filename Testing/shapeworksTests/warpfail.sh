#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd warpimage --source_landmarks $DATA/source.particles compare --name $DATA/warpfail.nrrd
