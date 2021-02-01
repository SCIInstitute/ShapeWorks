#! /bin/bash

shapeworks readmesh --name $DATA/femurThreeFields.vtk fieldmean --name distance | diff - $DATA/mean.txt
