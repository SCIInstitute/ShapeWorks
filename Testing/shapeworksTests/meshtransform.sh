#! /bin/bash

shapeworks readmesh --name $DATA/m03_L_femur.ply transform-mesh --target $DATA/m04_L_femur.ply comparemesh --name $DATA/icp.ply
