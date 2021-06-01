#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk decimate --reduction 0.0 --angle 0.0 --preservetopology 0 comparemesh --name $DATA/decimate1.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk decimate --reduction 0.0 --angle 0.0 comparemesh --name $DATA/decimate2.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk decimate comparemesh --name $DATA/decimate3.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk decimate --reduction 0.9 --angle 25.5 comparemesh --name $DATA/decimate4.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk decimate --reduction 0.9 --angle 25.5 --preservetopology 0 comparemesh --name $DATA/decimate5.vtk
