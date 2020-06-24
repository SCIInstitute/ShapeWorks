#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd topo-preserving-smooth compare --name $DATA/topo1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd topo-preserving-smooth --scaling 10.0 --alpha 10.5 --beta 10.5 compare --name $DATA/topo2.nrrd
