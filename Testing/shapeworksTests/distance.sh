#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk distance --name $DATA/pelvis.vtk set-field --name distance comparemesh --name $DATA/meshdistance2.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/pelvis.vtk distance --name $DATA/femur.vtk set-field --name distance comparemesh --name $DATA/meshdistance2rev.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03_L_femur.ply distance --name $DATA/m04_L_femur.ply --summary true set-field --name distance compare-mesh --name $DATA/meshdistance1p2p.vtk | diff - $DATA/meshdistance1p2p.txt
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m04_L_femur.ply distance --name $DATA/m03_L_femur.ply --summary true set-field --name distance comparemesh --name $DATA/meshdistance1rev.vtk | diff - $DATA/meshdistance1rev.txt
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03_L_femur.ply distance --name $DATA/m04_L_femur.ply --method "point-to-cell" --summary true set-field --name distance comparemesh --name $DATA/meshdistance1p2c.vtk | diff - $DATA/meshdistance1p2c.txt
