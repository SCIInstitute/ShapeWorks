#!/bin/bash

##################################################################################
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Run shapeworks prep pipeline given a set of segmentations 
# Notes:
##################################################################################


# adding related-binaries to system path
source ../Utils/Utils.sh # common utility functions
source ../setup.txt

### [*] This will process the Raw images in the same way as the segmentations
needRaw=0

### [1] Resample Both Raw and Binary Images to have an isotropic spacing
doResample=1
sizeX=269
sizeY=173
sizeZ=225
isoSpacing=1

### [2] Padding with constant
doPad=1
padding_size=30

### [3] Do a Center of Mass Alignment for better registeration
doCOM=1

### [4] Rigid Alignment using ANTS to a reference image
doAlign=0
ref_id="dodn02Rfemuraligned" # reference id of image to wich all others are aligned 

### [5] Find the Largest Bounding Box for Cropping
doLargestBB=1

### [6] Then perform cropping accordingly
doCrop=1

### [7] The Final Grooming
doGroomAndMesh=1
smoothing_iterations=2
antialias_iterations=40

seg_prefix=seg
img_prefix=img

parentDir='' # this is where all the preprocessed data will lie
rawImgDir='' # this is the path to the data (raw Images and Segmentations)
rawSegDir=''

while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -p|--parent_dir)
      parentDir="$2"
      shift
      ;;
      
      -i|--raw_image_dir) # prefix of mri images
      rawImgDir="$2"
      shift
      ;;
      
      -s|--raw_seg_dir) # prefix of the segmentation (binary image) 
      rawSegDir="$2"
      shift
      ;;

      -r|--process_raw)
      needRaw="$2"
      shift
      ;;

      -g|--img_prefix)
      img_prefix="$2"
      shift
      ;;

      -o|--seg_prefix)
      seg_prefix="$2"
      shift
      ;;
      
      --padding_size)
      doPad=1
      padding_size="$2"
      shift
      ;;
      
      --doCOM)
      doCOM="$2"
      shift
      ;;
      
      --alignRefID)
      doAlign=1
      ref_id="$2"
      shift
      ;;
      
      --largestBB)
      doLargestBB="$2"
      shift
      ;;
      
      --crop)
      doCrop="$2"
      shift
      ;;
      
      --smoothing_iterations)
      smoothing_iterations="$2"
      shift
      ;;
      
      --antialias_iterations)
      antialias_iterations="$2"
      shift
      ;;
      
      --default)
      DEFAULT=YES
      shift
      ;;
      *)
        # unknown option
      ;;
  esac
  shift
done

mkdir -p $parentDir
chmod 777 $parentDir
# resample volumes to be isotropic voxel space 
if [ $doResample -eq 1 ]
then
    if [ $needRaw -eq 1]
    then
        ./ResampleVolumes.sh --data_dir ${rawImgDir}/ \
                             --data_prefix ${img_prefix} \
                             --out_dir ${parentDir}resampled/ \
                             --iso_spacing 1 --is_center_on 0 --is_binary_image 0 \
                             --sizeX 575 --sizeY 575 --sizeZ 87
    fi            
    # resample binary images
    ./ResampleVolumes.sh --data_dir ${rawSegDir}/ \
                         --data_prefix ${seg_prefix} \
                         --out_dir ${parentDir}resampled/ \
                         --iso_spacing $isoSpacing --is_center_on 0 --is_binary_image 1 \
                         --sizeX $sizeX --sizeY $sizeY --sizeZ $sizeZ
fi

# pad volumes to make sure that we dont have open surfaces (segmentations near boundary)
if [ $doPad -eq 1 ]
then
    ./PadVolumes.sh  --data_dir ${parentDir}resampled/ \
                     --out_dir ${parentDir}padded/ \
                                --padding_size ${padding_size} \
                                --img_prefix ${img_prefix} \
                                --seg_prefix ${seg_prefix} \
                                --seg_suffix .isores --process_raw $needRaw
fi

# center of mass alignment
if [ $doCOM -eq 1 ]
then
    ./CenterOfMassAlignment.sh --data_dir ${parentDir}padded/ \
                        --out_dir ${parentDir}com_aligned/ \
                        --img_prefix ${img_prefix} \
                        --seg_prefix ${seg_prefix} \
                        --seg_suffix .isores.padded --process_raw $needRaw
fi

# align all subjects to one reference
if [ $doAlign -eq 1 ]
then
    ./RigidlyAlignAllSubjectsToReference.sh  --data_dir ${parentDir}com_aligned/ \
                                             --out_dir ${parentDir}aligned/ \
                                             --img_prefix ${img_prefix} \
                                             --seg_prefix ${seg_prefix} \
                                             --seg_suffix .isores.padded.com \
                                             --ref_id  $ref_id --process_raw $needRaw
fi

# extract meshes for debugging the largest bb
if [ $doGroomAndMeshAfterAlign -eq 1 ]
then
    ./GroomAndExtractMeshesFromSegmentations.sh  --data_dir ${parentDir}aligned/ \
                                         --out_dir ${parentDir}groom_and_meshes_after_align/ \
                                         --smoothing_iterations ${smoothing_iterations} \
                                         --antialias_iterations ${antialias_iterations} \
                                         --seg_prefix ${seg_prefix} \
                                         --seg_suffix .isores.padded.com.aligned
fi


# # find largest bounding box from the segmentations with a bit of padding to be used to crop images
if [ $doLargestBB -eq 1 ]
then
    ./FindLargestBoundingBox.sh  --data_dir ${parentDir}aligned/ \
                                --padding_size ${padding_size} \
                                --seg_prefix ${seg_prefix} \
                                --seg_suffix .isores.padded.com.aligned
fi

# # crop segmentations and their mri images based on the estimated largest bounding box
if [ $doCrop -eq 1 ]
then
    ./CropVolumes.sh --data_dir ${parentDir}aligned/ \
                    --out_dir ${parentDir}cropped/ \
                    --img_prefix ${img_prefix} \
                    --seg_prefix ${seg_prefix} \
                    --seg_suffix .isores.padded.com.aligned --process_raw $needRaw
fi

# groom cropped volumes for shapeworks
if [ $doGroomAndMesh -eq 1 ]
then
    ./GroomAndExtractMeshesFromSegmentations.sh  --data_dir ${parentDir}cropped/ \
                                         --out_dir ${parentDir}groom_and_meshes/ \
                                         --smoothing_iterations ${smoothing_iterations} \
                                         --antialias_iterations ${antialias_iterations} \
                                         --seg_prefix ${seg_prefix} \
                                         --seg_suffix .isores.padded.com.aligned.cropped
    mkdir -p ${parentDir}DistanceTransforms/
    mv ${parentDir}groom_and_meshes/*.tpSmoothDT.nrrd ${parentDir}DistanceTransforms/
fi
