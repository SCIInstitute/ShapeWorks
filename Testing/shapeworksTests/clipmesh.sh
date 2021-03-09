#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk clipmesh --nx 0.0 --ny 0.0 --nz 1.0 --ox -91.0 --oy 0.0 --oz 1230.0 comparemesh --name $DATA/clip1.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk clipmesh --nx 0.0 --ny 0.0 --nz -1.0 --ox -91.0 --oy 0.0 --oz 1230.0 comparemesh --name $DATA/clip2.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/femur.vtk clipmesh --nx -5.0 --ny 3.14159 --nz 1.0 --ox -60.0 --oy 10.0 --oz 1235.0 comparemesh --name $DATA/clip3.vtk
