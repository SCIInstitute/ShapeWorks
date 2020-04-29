#!/bin/bash

##################################################################################
# File:    CenterOfMassAlignment.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Align all subjects to their center of mass (translation)
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
      
      -o|--out_dir)
      out_dir="$2"
      shift
      ;;

      -r|--process_raw)
      process_raw="$2"
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

segPrefixLength=${#seg_prefix}
for segfilename in $(find $data_dir -name "${seg_prefix}*${seg_suffix}.nrrd" | sort -t '\0' ) ;
do
    prefix=$( GetFilePrefix ${segfilename} )

    nbr_of_dots=$( CountNumberOfOccurrencesOf  $prefix ".")
    tmp=1
    nbr_of_dots=$(expr $nbr_of_dots - $tmp)
    count=0
    while [ $count -lt $nbr_of_dots  ];
    do
        prefix=$( GetFilePrefix ${prefix} ) # remove .XX
        count=$((count + 1))
    done

    # make sure that the current file matches with the input prefix (laa_ prefix will also matach laa_wall)
    suffix=${prefix:segPrefixLength}
    
    # remove all occurences of underscore # no need for this!
    # suffix=$( RemoveAllOccurrenceOf $suffix "_" )
    # echo $suffix    
    # remove all occurences of dot
    suffix=$( RemoveAllOccurrenceOf $suffix "." )

    if [[ $( StringContainsOnlyNumbers $suffix ) -eq 1 ]]; 
    then
        echo correct prefix
    fi
    
    subject_id=$suffix

    imgfilename=${data_dir}${img_prefix}.${subject_id}${seg_suffix}.nrrd
    
    paramfilename_com=${out_dir}${seg_prefix}.${subject_id}${seg_suffix}.com.txt
    segfilename_com=${out_dir}${seg_prefix}.${subject_id}${seg_suffix}.com.nrrd
    imgfilename_com=${out_dir}${img_prefix}.${subject_id}${seg_suffix}.com.nrrd
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "segfilename $segfilename" "blue"
    EchoWithColor  "imgfilename $imgfilename" "blue"
    echo
    EchoWithColor "segfilename_com $segfilename_com" "yellow"
    EchoWithColor  "imgfilename_com $imgfilename_com" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    if [ $process_raw -eq 1 ]
    then
    shapeworks read-image --name $segfilename translate --centerofmass 1 writeimage --name $segfilename_com
    shapeworks read-image --name $imgfilename translate --centerofmass 1 writeimage --name $imgfilename_com
    
    else
    shapeworks read-image --name $segfilename translate --centerofmass 1 writeimage --name $segfilename_com
    fi
done
