#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd scale -x 1.0 -y 2.0 -z 2.0 compare --name $DATA/scale1.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin-centered.nrrd scale -x 1.0 -y -2.0 -z -1.0 compare --name $DATA/scale2.nrrd
