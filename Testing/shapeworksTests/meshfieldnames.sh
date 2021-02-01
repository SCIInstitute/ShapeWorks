#! /bin/bash

shapeworks readmesh --name $DATA/distance.vtk fieldnames
shapeworks readmesh --name $DATA/distance.vtk fieldnames | diff - $DATA/meshfieldnames.txt
