#! /bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd imageinfo | diff - $DATA/imageinfo.txt
