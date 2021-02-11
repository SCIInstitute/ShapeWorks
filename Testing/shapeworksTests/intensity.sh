#!/bin/bash

shapeworks readimage --name $DATA/nonBinary.nrrd intensity --max 100 compareimage --name $DATA/intensity.nrrd
