#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk smooth comparemesh --name $DATA/smooth2.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk smooth --iterations 10 --relaxation 0.01 comparemesh --name $DATA/smooth1.vtk
