#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd curvature --iterations -1 compare --name $DATA/curvaturefail.nrrd
