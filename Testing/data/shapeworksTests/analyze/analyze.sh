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

echo "Checking a project with more shapes than particle coordinates (#2703)"
script="import json, random
random.seed(0)
d = json.load(open('ellipsoid.swproj'))
subjects = []
for i in range(32):  # 32 shapes with 8 particles each: 3 x 8 = 24 < 31 modes
    s = dict(d['data'][i % 4])
    s['name'] = 'subject_%d' % i
    for key in ('local_particles_1', 'world_particles_1'):
        pts = [[float(v) for v in l.split()] for l in open(s[key]).read().split(chr(10))[:8]]
        if key == 'world_particles_1':
            pts = [[v + random.gauss(0, 0.2) for v in p] for p in pts]
        s[key] = 'more_shapes_%d_%s.particles' % (i, key)
        open(s[key], 'w').write(chr(10).join(' '.join('%f' % v for v in p) for p in pts) + chr(10))
    subjects.append(s)
d['data'] = subjects
json.dump(d, open('more_shapes.swproj', 'w'))
"
python3 -c "$script"
if [[ $? != 0 ]]; then exit -1; fi
shapeworks analyze --name more_shapes.swproj --output more_shapes.json --steps 3
if [[ $? != 0 ]]; then exit -1; fi
script="import json, sys
d = json.load(open('more_shapes.json'))
if len(d['modes']) != 24: sys.exit('expected 24 modes, got %d' % len(d['modes']))
for c in d['charts']:
    if len(c['y']) != 24: sys.exit(c['title'] + ' should have 24 modes')
"
python3 -c "$script"
if [[ $? != 0 ]]; then exit -1; fi
