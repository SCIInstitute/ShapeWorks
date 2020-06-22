#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd threshold --max -0.1 compare --name $DATA/thresholdfail.nrrd
