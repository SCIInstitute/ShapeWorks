#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd tplevelset --featureimage $DATA/curvature1.nrrd --scaling 10.0 compareimage --name $DATA/tplevelset1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd tplevelset --featureimage $DATA/curvature1.nrrd --scaling -10.0 compareimage --name $DATA/tplevelset2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd tplevelset compareimage --name $DATA/tplevelsetfail.nrrd
if [[ $? != 1 ]]; then exit -1; fi
