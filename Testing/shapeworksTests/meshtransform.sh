#! /bin/bash

shapeworks readmesh --name $DATA/m03_L_femur.ply transform-mesh --target $DATA/m04_L_femur.ply --type rigid comparemesh --name $DATA/icpRigid.ply
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03_L_femur.ply transform-mesh --target $DATA/m04_L_femur.ply comparemesh --name $DATA/icpSimilarity.ply
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03_L_femur.ply transform-mesh --target $DATA/m04_L_femur.ply --type affine comparemesh --name $DATA/icpAffine.ply
