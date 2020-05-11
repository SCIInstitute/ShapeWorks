#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd tp-levelset --featureimage $DATA/featureimage.nrrd --scaling 10.0 compare --name $DATA/tplevelset.nrrd
