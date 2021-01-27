#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk translatemesh --tx 1.0 --ty 1.0 --tz 1.0 comparemesh --name $DATA/translate1.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk translatemesh --tx -10.0 --ty -10.0 --tz -10.0 comparemesh --name $DATA/translate2.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk translatemesh --tx 0 --ty 0 --tz 1.0 comparemesh --name $DATA/translate3.vtk
