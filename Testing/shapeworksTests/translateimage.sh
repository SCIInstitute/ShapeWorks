#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd translateimage -x 10.0 -y 10.0 -z 10.0 compareimage --name $DATA/translate1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd translateimage -x -10.0 -y -10.0 -z -10.0 compareimage --name $DATA/translate2.nrrd
