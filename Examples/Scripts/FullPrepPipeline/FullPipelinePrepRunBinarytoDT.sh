#!/bin/bash

##################################################################################
# Authors: Shireen Elhabian
# Date:	Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Run shapeworks prep pipeline given a set of segmentations 
# Notes:
##################################################################################


# adding related-binaries to system path
source ../Utils/Utils.sh # common utility functions
source ../setup.txt

### [*] This will process the Raw images in the same way as the segmentations
needRaw=1

### [1] Resample Both Raw and Binary Images to have an isotropic spacing
doResample=1
isoSpacing=1

### [2] Padding with constant
doPad=0
padding_size=30

### [3] Do a Center of Mass Alignment for better registeration
doCOM=1

### [4] Rigid Alignment using ANTS to a reference image
doAlign=1
ref_id="dodn02Rfemuraligned" # reference id of image to which all others are aligned 

### [5] Find the Largest Bounding Box for Cropping
doLargestBB=0

### [6] Then perform cropping accordingly
doCrop=0

### [7] The Final Grooming
doGroomAndMesh=1
smoothing_iterations=2
antialias_iterations=40

seg_prefix=seg
img_prefix=img

parentDir='' # this is where all the preprocessed data will lie
rawImgDir='' # this is the path to the data (raw Images and Segmentations)
rawSegDir=''
scriptDir=../ToolLevelScripts/

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
	  
    --doResample)
    doResample="$2"
    shift
    ;;

    --doPad)
    doPad="$2"
    shift
    ;;

	  --padding_size)
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
	  
	  --doCrop)
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

parentDir=${parentDir}/
rawImgDir=${rawImgDir}/
rawSegDir=${rawSegDir}/

mkdir -p $parentDir
chmod 777 $parentDir

# resample volumes to be isotropic voxel space 
if [ $doResample -eq 1 ]
then
	if [ $needRaw -eq 1 ]
	then
		${scriptDir}ResampleVolumes.sh --data_dir ${rawImgDir}/ \
							 --data_prefix ${img_prefix} \
							 --out_dir ${parentDir}resampled/ \
							 --iso_spacing 1 --is_center_on 0 --is_binary_image 0 
	fi			
	# resample binary images
	${scriptDir}ResampleVolumes.sh --data_dir ${rawSegDir}/ \
						 --data_prefix ${seg_prefix} \
						 --out_dir ${parentDir}resampled/ \
						 --iso_spacing $isoSpacing --is_center_on 0 --is_binary_image 1 
fi

if [ $doResample -eq 0 ]
then
	 mkdir -p ${parentDir}resampled
   if [ $needRaw -eq 1]
   then
  	 for filename in $(find $rawImgDir -name "${img_prefix}*.nrrd" | sort -t '\0' ) ;
  	 do
  		 prefix=$( GetFilePrefix ${filename} )
  		 outnm=${parentDir}resampled/${prefix}.isores.nrrd
  		 cp $filename ${parentDir}resampled/
  		 mv ${parentDir}resampled/${prefix}.nrrd $outnm
  	 done
   fi
	 for filename in $(find $rawSegDir -name "${seg_prefix}*.nrrd" | sort -t '\0' ) ;
	 do
		 prefix=$( GetFilePrefix ${filename} )
		 outnm=${parentDir}resampled/${prefix}.isores.nrrd
		 cp $filename ${parentDir}resampled/
		 mv ${parentDir}resampled/${prefix}.nrrd $outnm
	 done
fi
# # pad volumes to make sure that we dont have open surfaces (segmentations near boundary)
if [ $doPad -eq 1 ]
then
	 ./PadVolumes.sh  --data_dir ${parentDir}resampled/ \
					  --out_dir ${parentDir}padded/ \
								 --padding_size ${padding_size} \
								 --img_prefix ${img_prefix} \
								 --seg_prefix ${seg_prefix} \
								 --seg_suffix .isores --process_raw $needRaw
fi

if [ $doPad -eq 0 ]
then
   mkdir -p ${parentDir}padded
   if [ $needRaw -eq 1 ]
   then
     for filename in $(find ${parentDir}resampled/ -name "${img_prefix}*.nrrd" | sort -t '\0' ) ;
     do
       prefix=$( GetFilePrefix ${filename} )
       outnm=${parentDir}padded/${prefix}.padded.nrrd
       cp $filename ${parentDir}padded/
       mv ${parentDir}padded/${prefix}.nrrd $outnm
     done
   fi
   for filename in $(find ${parentDir}resampled/ -name "${seg_prefix}*.nrrd" | sort -t '\0' ) ;
   do
     prefix=$( GetFilePrefix ${filename} )
     outnm=${parentDir}padded/${prefix}.padded.nrrd
     cp $filename ${parentDir}padded/
     mv ${parentDir}padded/${prefix}.nrrd $outnm
   done
fi

# center of mass alignment
if [ $doCOM -eq 1 ]
then
	 ${scriptDir}/CenterOfMassAlignment.sh --data_dir ${parentDir}padded/ \
						 --out_dir ${parentDir}com_aligned/ \
						 --img_prefix ${img_prefix} \
						 --seg_prefix ${seg_prefix} \
						 --seg_suffix .isores.padded --process_raw $needRaw
fi

if [ $doCOM -eq 0 ]
then
   mkdir -p ${parentDir}com_aligned
   if [ $needRaw -eq 1 ]
   then
     for filename in $(find ${parentDir}padded/ -name "${img_prefix}*.nrrd" | sort -t '\0' ) ;
     do
       prefix=$( GetFilePrefix ${filename} )
       outnm=${parentDir}com_aligned/${prefix}.com.nrrd
       cp $filename ${parentDir}com_aligned/
       mv ${parentDir}com_aligned/${prefix}.nrrd $outnm
     done
   fi
   for filename in $(find ${parentDir}padded/ -name "${seg_prefix}*.nrrd" | sort -t '\0' ) ;
   do
     prefix=$( GetFilePrefix ${filename} )
     outnm=${parentDir}com_aligned/${prefix}.com.nrrd
     cp $filename ${parentDir}com_aligned/
     mv ${parentDir}com_aligned/${prefix}.nrrd $outnm
   done
fi

# align all subjects to one reference
if [ $doAlign -eq 1 ]
then
	 ${scriptDir}/RigidlyAlignAllSubjectsToReference.sh  --data_dir ${parentDir}com_aligned/ \
											  --out_dir ${parentDir}aligned/ \
											  --img_prefix ${img_prefix} \
											  --seg_prefix ${seg_prefix} \
											  --seg_suffix .isores.padded.com \
											  --ref_id  $ref_id --process_raw $needRaw
fi

# # extract meshes for debugging the largest bb
# if [ $doGroomAndMeshAfterAlign -eq 1 ]
# then
#	 ./GroomAndExtractMeshesFromSegmentations.sh  --data_dir ${parentDir}aligned/ \
#										  --out_dir ${parentDir}groom_and_meshes_after_align/ \
#										  --smoothing_iterations ${smoothing_iterations} \
#										  --antialias_iterations ${antialias_iterations} \
#										  --seg_prefix ${seg_prefix} \
#										  --seg_suffix .isores.padded.com.aligned
# fi


# # # find largest bounding box from the segmentations with a bit of padding to be used to crop images
if [ $doCrop -eq 1 ]
then
	 ${scriptDir}/FindLargestBoundingBox.sh  --data_dir ${parentDir}aligned/ \
								 --padding_size ${padding_size} \
								 --seg_prefix ${seg_prefix} \
								 --seg_suffix .isores.padded.com.aligned
fi

# # crop segmentations and their mri images based on the estimated largest bounding box
if [ $doCrop -eq 1 ]
then
	 ${scriptDir}/CropVolumes.sh --data_dir ${parentDir}aligned/ \
					 --out_dir ${parentDir}cropped/ \
					 --img_prefix ${img_prefix} \
					 --seg_prefix ${seg_prefix} \
					 --seg_suffix .isores.padded.com.aligned --process_raw $needRaw
fi

if [ $doCrop -eq 0 ]
then
   mkdir -p ${parentDir}cropped
   if [ $needRaw -eq 1 ]
   then
     for filename in $(find ${parentDir}aligned/ -name "${img_prefix}*.nrrd" | sort -t '\0' ) ;
     do
       prefix=$( GetFilePrefix ${filename} )
       outnm=${parentDir}cropped/${prefix}.cropped.nrrd
       cp $filename ${parentDir}cropped/
       mv ${parentDir}cropped/${prefix}.nrrd $outnm
     done
   fi
   for filename in $(find ${parentDir}aligned/ -name "${seg_prefix}*.nrrd" | sort -t '\0' ) ;
   do
     prefix=$( GetFilePrefix ${filename} )
     outnm=${parentDir}cropped/${prefix}.cropped.nrrd
     cp $filename ${parentDir}cropped/
     mv ${parentDir}cropped/${prefix}.nrrd $outnm
   done
fi
# # groom cropped volumes for shapeworks
if [ $doGroomAndMesh -eq 1 ]
then
	 ${scriptDir}/GroomAndExtractMeshesFromSegmentations.sh  --data_dir ${parentDir}cropped/ \
										  --out_dir ${parentDir}groom_and_meshes/ \
										  --smoothing_iterations ${smoothing_iterations} \
										  --antialias_iterations ${antialias_iterations} \
										  --seg_prefix ${seg_prefix} \
										  --seg_suffix .isores.padded.com.aligned.cropped
	 mkdir -p ${parentDir}DistanceTransforms/
	 mv ${parentDir}groom_and_meshes/*.tpSmoothDT.nrrd ${parentDir}DistanceTransforms/
fi