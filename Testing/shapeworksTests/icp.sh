#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd icp --source $DATA/smooth1.nrrd --target $DATA/smooth2.nrrd --isovalue 1.0 --iterations 5 compare --name $DATA/icp.nrrd
