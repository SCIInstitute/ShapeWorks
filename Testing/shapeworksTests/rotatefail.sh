#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd rotate compareimage --name $DATA/rotatefail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd rotate --radians 0 compareimage --name $DATA/rotatefail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd rotate -x 1.0 -y 0.0 -z 0.0 compareimage --name $DATA/rotatefail.nrrd
