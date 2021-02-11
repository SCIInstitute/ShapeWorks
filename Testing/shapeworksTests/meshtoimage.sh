#! /bin/bash

shapeworks readmesh --name $DATA/femur.ply meshtoimage --originx 45 --originy -194 --originz -739 --sizex 92 --sizey 67 --sizez 131 compareimage --name $DATA/femurImageCmd.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.ply meshtoimage antialias --iterations 50 --maxrmserror 0.0 compareimage --name $DATA/antialias3.nrrd
