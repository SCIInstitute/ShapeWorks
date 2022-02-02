#! /bin/bash

shapeworks readmesh --name $DATA/femur.ply meshtoimage --pad 3 antialias --iterations 50 --maxrmserror 0.0 compareimage --name $DATA/antialiasMesh.nrrd
