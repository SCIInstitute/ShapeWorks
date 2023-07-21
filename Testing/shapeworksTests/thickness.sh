#! /bin/bash

shapeworks read-mesh --name $DATA/thickness/sphere.vtk compute-thickness --image $DATA/thickness/ct.nrrd comparemesh --name $DATA/thickness/thickness.vtk
