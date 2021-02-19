#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd scaleimage -x 1.0 -y 2.0 -z 2.0 compareimage --name $DATA/scale1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin-centered.nrrd scaleimage -x 1.0 -y -2.0 -z -1.0 compareimage --name $DATA/scale2.nrrd
