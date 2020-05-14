#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd translate --centerofmass 1 compare --name $DATA/centerofmass.nrrd
