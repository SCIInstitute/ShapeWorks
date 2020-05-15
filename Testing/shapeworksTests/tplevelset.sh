#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd tplevelset --featureimage $DATA/curvature1.nrrd --scaling 10.0 compare --name $DATA/tplevelset1.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd tplevelset --featureimage $DATA/curvature1.nrrd --scaling -10.0 compare --name $DATA/tplevelset2.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd tplevelset compare --name $DATA/tplevelset3.nrrd
