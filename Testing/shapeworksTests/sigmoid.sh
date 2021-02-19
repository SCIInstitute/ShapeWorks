#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd sigmoid compareimage --name $DATA/sigmoid1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd sigmoid --alpha 10.5 --beta 10.5 compareimage --name $DATA/sigmoid2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd sigmoid --alpha -10.5 compareimage --name $DATA/sigmoid3.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd sigmoid --beta -10.5 compareimage --name $DATA/sigmoid4.nrrd
