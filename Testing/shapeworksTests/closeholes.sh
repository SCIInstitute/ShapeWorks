#! /bin/bash

shapeworks readimage --name $DATA/image_with_holes.nrrd closeholes compare --name $DATA/closedholes.nrrd
