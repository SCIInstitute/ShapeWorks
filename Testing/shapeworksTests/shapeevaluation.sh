#!/bin/bash

truth=`cut -d' ' -f4 $DATA/shapeevaluation.txt | tr '\n' ' '`

if ! result=`shapeworks readparticlesystem --names $DATA/ellipsoid_particles/* -- compactness generalization specificity | cut -d' ' -f4 | tr '\n' ' '`; then
    exit 1
fi

script="import sys
lhs = [float(x) for x in \"$truth\".split()]
rhs = [float(x) for x in \"$result\".split()]
for a,b in zip(lhs, rhs):
    if abs(a-b) > 1e-1:
        sys.exit(1)
"
python -c "$script"
