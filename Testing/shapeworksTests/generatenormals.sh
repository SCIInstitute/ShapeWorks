#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk generatenormals comparemesh --name $DATA/normals.vtk
