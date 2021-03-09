#!/bin/bash

shapeworks readimage --name $DATA/la-bin.nrrd multiply -x 3.14 compareimage --name $DATA/baseline_multiply.nrrd
