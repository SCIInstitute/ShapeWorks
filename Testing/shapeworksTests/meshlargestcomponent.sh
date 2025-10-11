#! /bin/bash

shapeworks readmesh --name $DATA/two_spheres.vtk mesh-largest-component comparemesh --name $DATA/two_spheres_largest.vtk
