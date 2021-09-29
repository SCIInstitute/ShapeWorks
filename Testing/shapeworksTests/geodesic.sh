#! /bin/bash

shapeworks readmesh --name $DATA/ellipsoid_0.ply geodesic-distance --x1 1.5 --y1 -2.5 --z1 -8.82224751 --x2 1.5 --y2 -1.5 --z2 -8.951581 | diff - $DATA/geodesic.txt
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03.vtk geodesic-distance-landmark --x 92 --y -165 --z -720 set-field --name GeodesicDistanceToLandmark compare-mesh --name $DATA/geodesic3.vtk
