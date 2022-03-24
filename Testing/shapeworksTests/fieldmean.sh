#! /bin/bash

shapeworks readmesh --name $DATA/femurThreeFields.vtk fieldmean --name distance --type point | diff - $DATA/mean.txt
