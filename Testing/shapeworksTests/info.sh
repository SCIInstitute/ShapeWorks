#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd info | diff - $DATA/info.txt
