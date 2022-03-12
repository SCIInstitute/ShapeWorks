#! /bin/bash

shapeworks readmesh --name $DATA/femurThreeFields.vtk fieldrange --name distance --type point | diff - $DATA/range.txt
