#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk scalemesh --sx 1.0 --sy 1.0 --sz 1.0 comparemesh --name $DATA/scale1.vtk
