#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd crop --xmin 25 --xmax 50 --ymin 1 --ymax 40 --zmin 12 --zmax 23 compare --name $DATA/cropped.nrrd
