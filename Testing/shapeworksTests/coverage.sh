#! /bin/bash

shapeworks readmesh --name $DATA/m04_L_femur.ply coverage --name $DATA/icpSimilarity.ply --allowbackintersections 0 comparemesh --name $DATA/coverage.vtk
