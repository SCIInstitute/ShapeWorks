#! /bin/bash

shapeworks mean-normals --names $DATA/m03_normals.vtk $DATA/m04_normals.vtk -- comparemesh --name $DATA/meannormals.vtk 
