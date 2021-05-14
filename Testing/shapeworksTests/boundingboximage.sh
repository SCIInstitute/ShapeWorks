#!/bin/bash

diff -u <(shapeworks boundingboximage --names $DATA/many/*.nrrd) -- <(echo "Bounding box:
{
	min: [8, 16, 16],
	max: [42, 34, 34]
}")
if [[ $? != 0 ]]; then echo "bbox test 1 failed"; exit -1; fi
diff <(shapeworks readimage --name $DATA/femurImage.nrrd image-bounds) <(echo "Bounding box:
{
	min: [46.4821, -192.471, -737.593],
	max: [135.482, -128.471, -608.593]
}")
if [[ $? != 0 ]]; then echo "bbox test 2 failed"; exit -1; fi
diff <(shapeworks readimage --name $DATA/femurImage.nrrd image-bounds --padding 2) <(echo "Bounding box:
{
	min: [44.4821, -194.471, -739.593],
	max: [137.482, -126.471, -606.593]
}")
if [[ $? != 0 ]]; then echo "bbox test 3 failed"; exit -1; fi
diff <(shapeworks readimage --name $DATA/femurImage.nrrd image-bounds --isovalue 1) <(echo "Bounding box:
{
	min: [47.4821, -191.471, -736.593],
	max: [134.482, -129.471, -609.593]
}")
if [[ $? != 0 ]]; then echo "bbox test 4 failed"; exit -1; fi
diff <(shapeworks readimage --name $DATA/1x2x2.nrrd image-bounds) <(echo "Bounding box:
{
	min: [0, 0.5, 0.5],
	max: [100, 100.5, 100.5]
}")
if [[ $? != 0 ]]; then echo "bbox test 5 failed"; exit -1; fi
diff <(shapeworks readimage --name $DATA/1x2x2.nrrd image-bounds --padding 1) <(echo "Bounding box:
{
	min: [-1, -0.5, -0.5],
	max: [101, 101.5, 101.5]
}")
if [[ $? != 0 ]]; then echo "bbox test 6 failed"; exit -1; fi


