#!/bin/bash

shapeworks readimage --name $DATA/smooth1.nrrd icp-image --target $DATA/smooth2.nrrd --isovalue 1.0 --iterations 5 compareimage --name $DATA/icp.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd icpimage compareimage --name $DATA/icpfail.nrrd
if [[ $? != 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd icpimage compareimage --source --name $DATA/icpfail.nrrd
if [[ $? != 1 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd icpimage compareimage --target --name $DATA/icpfail.nrrd
if [[ $? != 1 ]]; then exit -1; fi
