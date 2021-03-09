#!/bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd negate compareimage --name $DATA/la-bin-negative.nrrd
