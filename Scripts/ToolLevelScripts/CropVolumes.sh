#!/bin/bash

##################################################################################
# File:    CropImages.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Align all subjects to one reference (rigid transformation)
# Notes:
##################################################################################

# adding related-binaries to system path
source ../setup.txt # works for server as well
source ../Utils/Utils.sh # common utility functions

# input parameters
while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -d|--data_dir)
      data_dir="$2"
      shift
      ;;
      
      -i|--img_prefix) # prefix of mri images
      img_prefix="$2"
      shift
      ;;
      
      -s|--seg_prefix) # prefix of the segmentation (binary image) 
      seg_prefix="$2"
      shift
      ;;
      
      -f|--seg_suffix) # suffix of the segmentation to be used for file search
      seg_suffix="$2"
      shift
      ;;

      -r|--process_raw)
      process_raw="$2"
      shift
      ;;

      -o|--out_dir)
      out_dir="$2"
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

bbprefix=${data_dir}${seg_prefix}_largest_bounding_box

bb0=$( head -n 1 ${bbprefix}_bb0.txt )
bb1=$( head -n 1 ${bbprefix}_bb1.txt )
bb2=$( head -n 1 ${bbprefix}_bb2.txt )

smallestIndex0=$( head -n 1 ${bbprefix}_smallestIndex0.txt )
smallestIndex1=$( head -n 1 ${bbprefix}_smallestIndex1.txt )
smallestIndex2=$( head -n 1 ${bbprefix}_smallestIndex2.txt )

echo bb0 : $bb0
echo bb1 : $bb1
echo bb2 : $bb2

echo smallestIndex0 : $smallestIndex0
echo smallestIndex1 : $smallestIndex1
echo smallestIndex2 : $smallestIndex2


mkdir -p $out_dir

# CropImages -inFilename ${data_dir}${seg_prefix}_average.nrrd -outFilename ${out_dir}${seg_prefix}_average_cropped.nrrd -bbX $bb0 -bbY $bb1 -bbZ $bb2 \
#                -startingIndexX $smallestIndex0 -startingIndexY $smallestIndex1 -startingIndexZ $smallestIndex2 
               
segPrefixLength=${#seg_prefix}
for segfilename in $(find $data_dir -name "${seg_prefix}*${seg_suffix}.nrrd" | sort -t '\0' ) ;
do
    prefix=$( GetFilePrefix ${segfilename} )
    nbr_of_dots=$( CountNumberOfOccurrencesOf  $prefix ".")
    tmp=1
    nbr_of_dots=$(expr $nbr_of_dots - $tmp) 
    count=0
    while [ $count -lt $nbr_of_dots ];
    do
        prefix=$( GetFilePrefix ${prefix} ) # remove .XX
        count=$((count + 1))
    done

    # make sure that the current file matches with the input prefix (laa_ prefix will also matach laa_wall)
    suffix=${prefix:segPrefixLength}
    
    # remove all occurences of underscore
    suffix=$( RemoveAllOccurrenceOf $suffix "_" )
    
    # remove all occurences of dot
    suffix=$( RemoveAllOccurrenceOf $suffix "." )

    if [[ $( StringContainsOnlyNumbers $suffix ) -eq 1 ]]; 
    then
        echo correct prefix
    fi
    
    subject_id=$suffix
    imgfilename=${data_dir}${img_prefix}.${subject_id}${seg_suffix}.nrrd
    
    segfilename_cropped=${out_dir}${seg_prefix}.${subject_id}${seg_suffix}.cropped.nrrd
    imgfilename_cropped=${out_dir}${img_prefix}.${subject_id}${seg_suffix}.cropped.nrrd
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "segfilename $segfilename" "yellow"
    EchoWithColor  "imgfilename $imgfilename" "yellow"
    echo
    EchoWithColor "segfilename_cropped $segfilename_cropped" "yellow"
    EchoWithColor  "imgfilename_cropped $imgfilename_cropped" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    if [ $process_raw -eq 1 ]
    then
      CropImages --inFilename $segfilename --outFilename $segfilename_cropped --bbX $bb0 --bbY $bb1 --bbZ $bb2 \
                 --startingIndexX $smallestIndex0 --startingIndexY $smallestIndex1 --startingIndexZ $smallestIndex2 \
                 --MRIinFilename $imgfilename --MRIoutFilename $imgfilename_cropped
    else
      CropImages --inFilename $segfilename --outFilename $segfilename_cropped --bbX $bb0 --bbY $bb1 --bbZ $bb2 \
                 --startingIndexX $smallestIndex0 --startingIndexY $smallestIndex1 --startingIndexZ $smallestIndex2 
    fi
done


