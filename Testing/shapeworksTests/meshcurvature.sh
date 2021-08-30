#! /bin/bash

shapeworks read-mesh --name $DATA/m03.vtk mesh-curvature --type mean set-field --name MeanCurvature compare-mesh --name $DATA/meancurvature.vtk
