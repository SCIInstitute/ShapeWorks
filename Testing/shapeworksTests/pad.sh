#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd pad compare --name $DATA/pad1.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd pad --padding -1 compare --name $DATA/pad2.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd pad --value 1.0 compare --name $DATA/pad3.nrrd
