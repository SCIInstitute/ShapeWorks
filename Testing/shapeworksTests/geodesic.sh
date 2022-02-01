#! /bin/bash

shapeworks readmesh --name $DATA/ellipsoid_0.ply geodesic-distance --v1 10 --v2 200 | diff - $DATA/geodesic.txt
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03.vtk geodesic-distance-landmark --x 7.5 --y -4.5 --z -6.59 set-field --name GeodesicDistanceToLandmark --type point compare-mesh --name $DATA/geodesic3.vtk
