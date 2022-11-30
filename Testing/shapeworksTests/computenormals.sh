#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk computenormals comparemesh --name $DATA/normals.vtk
