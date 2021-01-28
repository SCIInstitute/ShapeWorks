#! /bin/bash

shapeworks readmesh --name $DATA/m03.vtk meshfix comparemesh --name $DATA/fix1.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03.vtk meshfix --lambda 1.0 --iterations 10 --percentage 1.0 comparemesh --name $DATA/fix2.vtk