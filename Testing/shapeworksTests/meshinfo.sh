#! /bin/bash

shapeworks readmesh --name $DATA/distance.vtk meshinfo | diff - $DATA/meshinfo.txt
