#!/bin/bash

##################################################################################
# File:    PadVolumes.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Pad images and segmentations to avoid open surfaces (segmentations near boundary)
# Notes:
##################################################################################

# adding related-binaries to system path
source ../setup.txt # works for server as well
source ../Utils/Utils.sh # common utility functions

padVal=0
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
      
      -p|--padding_size) 
      padding_size="$2"
      shift
      ;;

      -p|--padding_value) 
      padVal="$2"
      shift
      ;;
      
      -o|--out_dir)
      out_dir="$2"
      shift
      ;;

      -r|--process_raw)
      process_raw="$2"
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


mkdir -p $out_dir

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
    
    segfilename_padded=${out_dir}${seg_prefix}.${subject_id}${seg_suffix}.padded.nrrd
    imgfilename_padded=${out_dir}${img_prefix}.${subject_id}${seg_suffix}.padded.nrrd
    
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "segfilename $segfilename" "yellow"
    EchoWithColor  "imgfilename $imgfilename" "yellow"
    echo
    EchoWithColor "segfilename_padded $segfilename_padded" "yellow"
    EchoWithColor  "imgfilename_padded $imgfilename_padded" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    PadVolumeWithConstant --inFilename $segfilename --outFilename $segfilename_padded --paddingSize $padding_size --paddingValue $padVal
    if [ $process_raw -eq 1 ]
    then
      PadVolumeWithConstant --inFilename $imgfilename --outFilename $imgfilename_padded --paddingSize $padding_size --paddingValue $padVal
    fi
done
