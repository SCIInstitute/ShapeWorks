#!/bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd divide -x 3.14 compareimage --name $DATA/baseline_divide.nrrd
