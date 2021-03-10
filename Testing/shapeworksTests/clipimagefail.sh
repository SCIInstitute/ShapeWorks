#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd clipimage --x1 1 compareimage --name $DATA/clipfail.nrrd

shapeworks readimage --name $DATA/1x2x2.nrrd clipimage compareimage --name $DATA/clipfail.nrrd
