#! /bin/bash

shapeworks readmesh --name $DATA/ellipsoid_0.ply remesh --target 3000 --adaptivity 1.0 comparemesh --name $DATA/remesh1.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/ellipsoid_01.vtk remesh --target 1000 --adaptivity 2.0 comparemesh --name $DATA/remesh2.vtk
if [[ $? != 0 ]]; then exit -1; fi

shapeworks readmesh --name $DATA/femur.vtk remesh-percent --percentage 0.25 --adaptivity 1.0 comparemesh --name $DATA/remeshPercent1.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk remesh-percent --percentage 0.1 --adaptivity 0.5 comparemesh --name $DATA/remeshPercent2.vtk
if [[ $? != 0 ]]; then exit -1; fi
