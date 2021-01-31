#! /bin/bash

shapeworks readmesh --name $DATA/m04_L_femur.ply coverage --name $DATA/icp.ply comparemesh --name $DATA/coverage.vtk
