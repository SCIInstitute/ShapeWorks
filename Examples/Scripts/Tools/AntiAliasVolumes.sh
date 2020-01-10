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

scriptHome=../

# input parameters
while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -d|--data_dir)
      data_dir="$2"
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
      
      -s|--scriptHome)
      scriptHome="$2"
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

# adding related-binaries to system path
source ${scriptHome}/setup.txt # works for server as well
source ${scriptHome}/Utils/Utils.sh # common utility functions

mkdir -p $out_dir
out_dir="/home/sci/riddhishb/Documents/git-repos/voxelmorph/data/ThousandStrongRotate/RotateBoxes-with-points/AAnrrd/"
# CropImages -inFilename ${data_dir}${seg_prefix}_average.nrrd -outFilename ${out_dir}${seg_prefix}_average_cropped.nrrd -bbX $bb0 -bbY $bb1 -bbZ $bb2 \
#                -startingIndexX $smallestIndex0 -startingIndexY $smallestIndex1 -startingIndexZ $smallestIndex2 
data_dir="/home/sci/riddhishb/Documents/git-repos/voxelmorph/data/ThousandStrongRotate/RotateBoxes-with-points/nrrd/"      
segPrefixLength=${#seg_prefix}
for segfilename in $(find $data_dir -name "*.nrrd" | sort -t '\0' ) ;
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
    
    # # remove all occurences of underscore
    # suffix=$( RemoveAllOccurrenceOf $suffix "_" )
    

    # if [[ $( StringContainsOnlyNumbers $suffix ) -eq 1 ]]; 
    # then
    #     echo correct prefix
    # fi
    
    subject_id=$suffix
    echo $suffix
    # imgfilename=${data_dir}${img_prefix}.${subject_id}${seg_suffix}.nrrd
    
    segfilename_cropped=${out_dir}${subject_id}.aliased.nrrd

    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "segfilename $segfilename" "yellow"
    EchoWithColor  "imgfilename $imgfilename" "yellow"
    echo
    EchoWithColor "segfilename_cropped $segfilename_cropped" "yellow"
    EchoWithColor  "imgfilename_cropped $imgfilename_cropped" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    shapeworks antialias --inFilename $segfilename --outFilename $segfilename_cropped 
  
done


