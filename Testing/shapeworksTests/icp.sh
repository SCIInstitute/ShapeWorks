#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd icp --source --target --isovalue 1.0 --iterations 5 compare --name $DATA/icp.nrrd
