#!/bin/bash

truth=`cut -d' ' -f4 $DATA/shapeevaluation.txt`

if ! result=`shapeworks readparticlesystem --names $DATA/ellipsoid_particles/* -- compactness generalization specificity | cut -d' ' -f4`; then
    exit 1
fi

script="import sys
lhs = [float(x) for x in sys.argv[1].split()]
rhs = [float(x) for x in sys.argv[2].split()]
for a,b in zip(lhs, rhs):
    if abs(a-b) > 1e-1:
        sys.exit(1)
"
python -c "$script" "$truth" "$result"
