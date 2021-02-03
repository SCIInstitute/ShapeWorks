#! /bin/bash

shapeworks readmesh --name $DATA/distance.vtk fieldnames | diff - $DATA/meshfieldnames.txt
