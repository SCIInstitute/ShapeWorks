#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk translatemesh --tx 1.0 --ty 1.0 --tz 1.0 comparemesh --name $DATA/translate1.vtk
