#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd icp compareimage --name $DATA/icpfail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd icp compareimage --source --name $DATA/icpfail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd icp compareimage --target --name $DATA/icpfail.nrrd
