#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd tplevelset --featureimage $DATA/curvature1.nrrd --scaling 10.0 compare --name $DATA/tplevelset.nrrd
