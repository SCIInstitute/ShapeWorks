#! /bin/bash

shapeworks readmesh --name $DATA/femurThreeFields.vtk getfieldvalue --name bloop --index 400 | diff - $DATA/fieldvalue.txt
