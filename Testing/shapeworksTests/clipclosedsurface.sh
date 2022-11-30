#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk clipclosedsurface --nx 0.0 --ny 850.0 --nz 0.0 --px 10.0 --py 0.0 --pz 10.0 comparemesh --name $DATA/clipClosed1.vtk
