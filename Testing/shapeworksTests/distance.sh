#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk \
           distance --name $DATA/pelvis.vtk --method "point-to-point" \
           set-field --name distance \
           distance --name $DATA/pelvis.vtk --method "point-to-point" --ids true \
           set-field --name closestPoints \
           comparemesh --name $DATA/meshdistance_point_fwd.vtk
if [[ $? != 0 ]]; then exit -1; fi

shapeworks readmesh --name $DATA/pelvis.vtk \
           distance --name $DATA/femur.vtk --method "point-to-point" \
           set-field --name distance \
           distance --name $DATA/femur.vtk --method "point-to-point" --ids true \
           set-field --name closestPoints \
           comparemesh --name $DATA/meshdistance_point_rev.vtk
if [[ $? != 0 ]]; then exit -1; fi

shapeworks readmesh --name $DATA/m03_L_femur.ply \
           distance --name $DATA/m04_L_femur.ply --summary true \
           set-field --name distance \
           distance --name $DATA/m04_L_femur.ply --ids true \
           set-field --name closestCells \
           compare-mesh --name $DATA/meshdistance_cell_fwd.vtk \
    | diff - $DATA/meshdistance_cell_fwd.txt
if [[ $? != 0 ]]; then exit -1; fi

shapeworks readmesh --name $DATA/m04_L_femur.ply \
           distance --name $DATA/m03_L_femur.ply --method "point-to-cell" --summary true \
           set-field --name distance \
           distance --name $DATA/m03_L_femur.ply --ids true \
           set-field --name closestCells \
           comparemesh --name $DATA/meshdistance_cell_rev.vtk \
    | diff - $DATA/meshdistance_cell_rev.txt
