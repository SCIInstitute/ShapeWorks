#! /bin/bash

shapeworks read-mesh --name $DATA/ellipsoid_0.ply mesh-curvature --type mean set-field --name MeanCurvature --type point compare-mesh --name $DATA/meanCurvatureEllipsoid.vtk --epsilon 1e-14
if [[ $? != 0 ]]; then exit -1; fi
shapeworks read-mesh --name $DATA/ellipsoid_0.ply mesh-curvature --type principal set-field --name PrincipalCurvature --type point compare-mesh --name $DATA/principalCurvatureEllipsoid.vtk --epsilon 1e-14
if [[ $? != 0 ]]; then exit -1; fi
shapeworks read-mesh --name $DATA/ellipsoid_0.ply mesh-curvature --type gaussian set-field --name GaussianCurvature --type point compare-mesh --name $DATA/gaussianCurvatureEllipsoid.vtk --epsilon 1e-14
if [[ $? != 0 ]]; then exit -1; fi
shapeworks read-mesh --name $DATA/m03.vtk mesh-curvature --type mean set-field --name MeanCurvature --type point compare-mesh --name $DATA/meanCurvatureFemur.vtk --epsilon 1e-14
if [[ $? != 0 ]]; then exit -1; fi
shapeworks read-mesh --name $DATA/m03.vtk mesh-curvature set-field --name PrincipalCurvature --type point compare-mesh --name $DATA/principalCurvatureFemur.vtk --epsilon 1e-14
if [[ $? != 0 ]]; then exit -1; fi
shapeworks read-mesh --name $DATA/m03.vtk mesh-curvature --type gaussian set-field --name GaussianCurvature --type point compare-mesh --name $DATA/gaussianCurvatureFemur.vtk --epsilon 1e-14
