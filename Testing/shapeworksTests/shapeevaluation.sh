#! /bin/bash

shapeworks readparticlesystem --names $DATA/ellipsoid_particles/* -- compactness generalization specificity | diff - $DATA/shapeevaluation.txt
