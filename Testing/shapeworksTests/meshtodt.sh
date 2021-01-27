#! /bin/bash

shapeworks readmesh --name $DATA/femur.ply meshtodt compareimage --precision 1.54 --name $DATA/femurDT.nrrd
