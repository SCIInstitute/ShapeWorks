#! /bin/bash

shapeworks read-mesh --name $DATA/ellipsoid_01.vtk write-mesh --name $DATA/ellipsoidBinary.vtk --binary 1 compare-mesh --name $DATA/ellipsoidBinary.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks read-mesh --name $DATA/ellipsoid_0.ply write-mesh --name $DATA/ellipsoidBinary.ply --binary 1 compare-mesh --name $DATA/ellipsoidBinary.ply