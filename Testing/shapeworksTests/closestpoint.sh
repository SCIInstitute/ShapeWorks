#! /bin/bash

shapeworks readmesh --name $DATA/ellipsoid_0.ply closest-point --x 2.5 --y 0.5 --z -10 | diff - $DATA/closestpoint.txt
