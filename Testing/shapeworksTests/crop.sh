#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd imageinfo set-region --xmin 25 --xmax 49 --ymin 2.5 --ymax 78.5 --zmin 24.5 --zmax 44.5 crop writeimage --name "/tmp/yay.nrrd" compareimage --name $DATA/cropped1.nrrd
