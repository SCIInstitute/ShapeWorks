#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd scale -x 2.0 -y 2.0 -z 2.0 compare --name $DATA/scale1.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd scale -x -2.0 -y -2.0 -z -2.0 compare --name $DATA/scale2.nrrd