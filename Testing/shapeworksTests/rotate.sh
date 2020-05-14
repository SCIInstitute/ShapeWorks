#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd rotate -x 1.0 -y 0.0 -z 0.0  --angle 45 compare --name $DATA/rotate.nrrd
