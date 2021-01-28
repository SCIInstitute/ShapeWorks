#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd icp-image --target $DATA/smooth2.nrrd --isovalue 1.0 --iterations 5 compareimage --name $DATA/icp.nrrd
