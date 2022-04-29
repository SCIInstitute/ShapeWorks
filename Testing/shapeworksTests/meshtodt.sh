#! /bin/bash

shapeworks readmesh --name $DATA/femur_remesh.ply meshtodt --sx=5 --sy=5 --sz=5 compareimage --name $DATA/femur_remesh_dt.nrrd
