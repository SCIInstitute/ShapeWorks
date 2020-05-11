#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd blur compare --name $DATA/blur1.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd blur --sigma 1.0 compare --name $DATA/blur2.nrrd
