#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd extractlabel compare --name $DATA/extractlabel1.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd extractlabel --label 2.0 compare --name $DATA/extractlabel2.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd extractlabel --label -2.0 compare --name $DATA/extractlabel3.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd extractlabel --label 0 compare --name $DATA/extractlabel4.nrrd
