#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd computedt compare --name $DATA/computedt1.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd computedt --isovalue 1.0 compare --name $DATA/computedt2.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd computedt --isovalue -1.0 compare --name $DATA/computedt3.nrrd
