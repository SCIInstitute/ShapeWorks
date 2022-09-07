#!/bin/bash

shapeworks analyze --name ellipsoid.swproj --output analysis_output.json
if [[ $? != 0 ]]; then exit -1; fi
diff analysis_output.json analysis_baseline.json
if [[ $? != 0 ]]; then exit -1; fi

shapeworks readmesh --name mean_shape_0.baseline.vtk \
           comparemesh --name mean_shape_0.vtk
if [[ $? != 0 ]]; then exit -1; fi

shapeworks readmesh --name pca_mode_1_domain_0_minus_0.4.baseline.vtk \
           comparemesh --name pca_mode_1_domain_0_minus_0.4.vtk
if [[ $? != 0 ]]; then exit -1; fi
