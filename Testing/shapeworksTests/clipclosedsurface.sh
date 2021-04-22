#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk clipclosedsurface --nx 0.0 --ny 850.0 --nz 0.0 --ox 10.0 --oy 0.0 --oz 10.0 comparemesh --name $DATA/clipClosed1.vtk
