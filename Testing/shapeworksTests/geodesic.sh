#! /bin/bash

shapeworks readmesh --name $DATA/ellipsoid_0.ply geodesic-distance --x1 1.5 --y1 -2.5 --z1 -8.82224751 --x2 1.5 --y2 -1.5 --z2 -8.951581 | diff - $DATA/geodesic.txt
