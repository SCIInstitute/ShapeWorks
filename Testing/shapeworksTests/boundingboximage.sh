#!/bin/bash

diff -u <(shapeworks boundingboximage --names $DATA/many/*.nrrd) -- <(echo "Bounding box:
{
	min: [8, 16, 16],
	max: [42, 34, 34]
}")
if [[ $? != 0 ]]; then exit -1; fi
diff <(shapeworks readimage --name $DATA/femurImage.nrrd image-bounds) <(echo "Bounding box:
{
	min: [46.4821, -192.471, -737.593],
	max: [134.482, -129.471, -609.593]
}")
if [[ $? != 0 ]]; then exit -1; fi
diff <(shapeworks readimage --name $DATA/femurImage.nrrd image-bounds --padding 2) <(echo "Bounding box:
{
	min: [44.4821, -194.471, -739.593],
	max: [136.482, -127.471, -607.593]
}")
if [[ $? != 0 ]]; then exit -1; fi
diff <(shapeworks readimage --name $DATA/femurImage.nrrd image-bounds --isovalue 1) <(echo "Bounding box:
{
	min: [47.4821, -191.471, -736.593],
	max: [134.482, -129.471, -609.593]
}")


