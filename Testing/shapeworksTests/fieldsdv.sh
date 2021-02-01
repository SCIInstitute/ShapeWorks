#! /bin/bash

shapeworks readmesh --name $DATA/femurThreeFields.vtk fieldsdv --name distance | diff - $DATA/sdv.txt
