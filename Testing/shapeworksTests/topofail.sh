#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd topo-preserving-smooth --iterations -2 compare --name $DATA/topofail.nrrd
