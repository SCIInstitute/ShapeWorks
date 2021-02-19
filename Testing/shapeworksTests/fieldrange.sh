#! /bin/bash

shapeworks readmesh --name $DATA/femurThreeFields.vtk fieldrange --name distance | diff - $DATA/range.txt
