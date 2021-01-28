#! /bin/bash

shapeworks readmesh --name $DATA/femur.vtk probe-volume --image $DATA/femurVtkDT.nrrd comparemesh --name $DATA/probe.vtk
