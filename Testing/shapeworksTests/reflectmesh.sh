#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk reflectmesh --axis X comparemesh --name $DATA/reflect1.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk reflectmesh --axis Y comparemesh --name $DATA/reflect2.vtk
