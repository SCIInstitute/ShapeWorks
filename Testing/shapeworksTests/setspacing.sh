#! /bin/bash

shapeworks readimage --name $DATA/la1-small.nrrd setspacing compareimage --name $DATA/spacing1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/la1-small.nrrd setspacing -x 2.0 -y 2.0 -z 2.0 compareimage --name $DATA/spacing2.nrrd
