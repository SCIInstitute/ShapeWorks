#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd antialias --layers -1 compareimage-image --name $DATA/antialiasfail.nrrd
