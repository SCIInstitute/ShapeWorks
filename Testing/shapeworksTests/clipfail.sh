#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd clip --x1 1 compare --name $DATA/clipfail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd clip compare --name $DATA/clipfail.nrrd
