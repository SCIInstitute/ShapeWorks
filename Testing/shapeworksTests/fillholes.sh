#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk fillholes comparemesh --name $DATA/fillholes.vtk
