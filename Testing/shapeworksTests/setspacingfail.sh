#! /bin/bash

shapeworks readimage --name $DATA/la1-small.nrrd setspacing -x 0 compareimage --name $DATA/spacing1.nrrd
