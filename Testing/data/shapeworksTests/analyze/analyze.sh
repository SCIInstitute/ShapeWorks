#!/bin/bash

shapeworks analyze --name ellipsoid.swproj --output analysis_output.json
if [[ $? != 0 ]]; then exit -1; fi

echo "Comparing analysis output"
diff analysis_output.json analysis_baseline.json
if [[ $? != 0 ]]; then exit -1; fi

echo "Comparing mean_shape_0.vtk"
shapeworks readmesh --name mean_shape_0.baseline.vtk \
           comparemesh --name mean_shape_0.vtk
if [[ $? != 0 ]]; then exit -1; fi

echo "comparing pca_mode_1_domain_0_minus_0.3.vtk"
shapeworks readmesh --name pca_mode_1_domain_0_minus_0.3.baseline.vtk \
           comparemesh --name pca_mode_1_domain_0_minus_0.3.vtk
if [[ $? != 0 ]]; then exit -1; fi

echo "Checking --evaluation_modes and --skip_evaluation"
shapeworks analyze --name ellipsoid.swproj --output analysis_limited.json --evaluation_modes 1
if [[ $? != 0 ]]; then exit -1; fi
shapeworks analyze --name ellipsoid.swproj --output analysis_skipped.json --skip_evaluation
if [[ $? != 0 ]]; then exit -1; fi

script="import json, sys
full = {c['title']: c['y'] for c in json.load(open('analysis_baseline.json'))['charts']}
limited = {c['title']: c['y'] for c in json.load(open('analysis_limited.json'))['charts']}
skipped = {c['title']: c['y'] for c in json.load(open('analysis_skipped.json'))['charts']}
if limited['Compactness'] != full['Compactness']: sys.exit('compactness should not be limited')
for title in ['Generalization', 'Specificity']:
    if limited[title] != full[title][:1]: sys.exit(title + ' was not limited to one mode')
if list(skipped) != ['Compactness']: sys.exit('evaluation was not skipped')
"
python3 -c "$script"
if [[ $? != 0 ]]; then exit -1; fi
