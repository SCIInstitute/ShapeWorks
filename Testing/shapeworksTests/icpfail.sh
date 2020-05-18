#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd icp compare --name $DATA/icpfail.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd icp compare --source --name $DATA/icpfail.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd icp compare --target --name $DATA/icpfail.nrrd
