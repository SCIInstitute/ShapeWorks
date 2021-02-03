#! /bin/bash

shapeworks readmesh --name $DATA/femur.ply meshdistance --name $DATA/pelvis.vtk writemesh --name /tmp/meshdistance2.vtk comparemesh --name $DATA/meshdistance2.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/pelvis.vtk meshdistance --name $DATA/femur.ply writemesh --name /tmp/meshdistance2rev.vtk comparemesh --name $DATA/meshdistance2rev.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03_L_femur.ply meshdistance --name $DATA/m04_L_femur.ply --summary true writemesh --name /tmp/meshdistance1p2p.vtk compare-mesh --name $DATA/meshdistance1p2p.vtk | diff - $DATA/meshdistance1p2p.txt
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m04_L_femur.ply meshdistance --name $DATA/m03_L_femur.ply --summary true writemesh --name /tmp/meshdistance1rev.vtk comparemesh --name $DATA/meshdistance1rev.vtk | diff - $DATA/meshdistance1rev.txt
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readmesh --name $DATA/m03_L_femur.ply meshdistance --name $DATA/m04_L_femur.ply --method "point-to-cell" --summary true comparemesh --name $DATA/meshdistance1p2c.vtk | diff - $DATA/meshdistance1p2c.txt
