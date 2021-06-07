#!/bin/bash

diff --strip-trailing-cr <(shapeworks boundingboxmesh --names $DATA/femur.ply $DATA/ellipsoid_0.ply) <(echo "Bounding box:
{
	min: [-15.4298, -192.471, -737.593],
	max: [135.026, 8.9597, 8.96317]
}")
if [[ $? != 0 ]]; then exit -1; fi
diff --strip-trailing-cr <(shapeworks readmesh --name $DATA/femur.ply mesh-bounds) <(echo "Bounding box:
{
	min: [46.4821, -192.471, -737.593],
	max: [135.026, -128.644, -609.55]
}")
if [[ $? != 0 ]]; then exit -1; fi
