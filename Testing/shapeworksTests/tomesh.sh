#! /bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd imagetomesh comparemesh --name $DATA/mesh1.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/computedt1.nrrd dttomesh --levelset 1.0 comparemesh --name $DATA/mesh2.vtk
