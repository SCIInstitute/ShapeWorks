#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd icpimage compareimage --name $DATA/icpfail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd icpimage compareimage --source --name $DATA/icpfail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd icpimage compareimage --target --name $DATA/icpfail.nrrd
