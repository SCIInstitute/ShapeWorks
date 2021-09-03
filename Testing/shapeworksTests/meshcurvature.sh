#! /bin/bash

shapeworks read-mesh --name $DATA/m03.vtk mesh-curvature --type mean set-field --name MeanCurvature compare-mesh --name $DATA/meancurvature.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks read-mesh --name $DATA/m03.vtk mesh-curvature set-field --name PrincipalCurvature compare-mesh --name $DATA/principalcurvature.vtk
if [[ $? != 0 ]]; then exit -1; fi
shapeworks read-mesh --name $DATA/m03.vtk mesh-curvature --type gaussian mean set-field --name GaussianCurvature compare-mesh --name $DATA/gaussiancurvature.vtk
