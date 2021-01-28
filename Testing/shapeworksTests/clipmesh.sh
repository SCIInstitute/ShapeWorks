#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk clipmesh --x1 1.0 --y1 1.0 --z1 1.0 comparemesh --name $DATA/clip1.vtk
