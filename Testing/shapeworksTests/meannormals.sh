#! /bin/bash

shapeworks mean-normals --names $DATA/m03_normals.vtk $DATA/m04_normals.vtk -- read-mesh --name $DATA/m03_normals.vtk set-field --name MeanNormals compare-mesh --name $DATA/meannormals.vtk
