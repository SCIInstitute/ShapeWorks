#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd translate -x 10.0 -y 10.0 -z 10.0 compare --name $DATA/translate1.nrrd
if [[ $? == 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd translate -x -10.0 -y -10.0 -z -10.0 compare --name $DATA/translate2.nrrd
