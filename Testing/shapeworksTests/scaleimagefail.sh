#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd scaleimage --sx 0.0 compareimage --name $DATA/scalefail.nrrd
