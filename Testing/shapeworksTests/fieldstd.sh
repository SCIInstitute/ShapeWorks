#! /bin/bash

shapeworks readmesh --name $DATA/femurThreeFields.vtk fieldstd --name distance | diff - $DATA/std.txt
