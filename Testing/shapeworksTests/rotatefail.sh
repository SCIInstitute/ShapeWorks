#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd rotate compare --name $DATA/rotatefail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd rotate --angle 45 compare --name $DATA/rotatefail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd rotate -x 1.0 -y 0.0 -z 0.0 compare --name $DATA/rotatefail.nrrd
