#! /bin/bash

shapeworks readimage --name $DATA/image_with_holes.nrrd closeholes compareimage --name $DATA/closedholes.nrrd
