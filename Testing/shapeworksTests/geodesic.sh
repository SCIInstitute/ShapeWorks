#! /bin/bash

shapeworks readmesh --name $DATA/ellipsoid_0.ply geodesic-distance --source 10 --target 20 | diff - $DATA/geodesic.txt
