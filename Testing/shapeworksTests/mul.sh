#!/bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd multiply -x 3.14 compare --name $DATA/baseline_multiply.nrrd
