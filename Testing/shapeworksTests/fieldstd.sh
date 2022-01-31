#! /bin/bash

shapeworks readmesh --name $DATA/femurThreeFields.vtk fieldstd --name distance --type point | diff - $DATA/std.txt
