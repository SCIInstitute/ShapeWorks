#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk meshinfo | diff - $DATA/meshinfo.txt
