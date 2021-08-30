#! /bin/bash

shapeworks mean-normals --names $DATA/m03_normals.vtk $DATA/m04_normals.vtk -- --generatenormals 0 read-mesh --name $DATA/m03_normals.vtk set-field --name MeanNormals compare-mesh --name $DATA/meannormals.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks mean-normals --names $DATA/m03.vtk $DATA/m04.vtk -- read-mesh --name $DATA/m03_normals.vtk set-field --name MeanNormals compare-mesh --name $DATA/meannormals.vtk