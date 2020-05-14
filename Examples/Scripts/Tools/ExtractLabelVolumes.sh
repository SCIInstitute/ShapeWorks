#!/bin/bash

##################################################################################
# File:    ExtractLabelVolumes.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Isolate the anatomy of interset by extracting its label from a multi-label segmentation volume
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
      
      -s|--seg_prefix)
      seg_prefix="$2"
      shift
      ;;
      
      -o|--out_dir)
      out_dir="$2"
      shift
      ;;
      
      -p|--out_prefix)
      out_prefix="$2"
      shift
      ;;
      
      -l|--seg_label)
      seg_label="$2"
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

for nrrdfilename in $(find $data_dir -name "${seg_prefix}*.nrrd" | sort -t '\0' ) ;
do
    prefix=$( GetFilePrefix ${nrrdfilename} )

    # make sure that the current file matches with the input prefix (laa_ prefix will also matach laa_wall)
    suffix=${prefix:segPrefixLength}
    
    # remove all occurences of underscore
    suffix=$( RemoveAllOccurrenceOf $suffix "_" )
    
    # remove all occurences of dot
    suffix=$( RemoveAllOccurrenceOf $suffix "." )
    #echo $suffix
    
    if [[ $( StringContainsOnlyNumbers $suffix ) -eq 1 ]]; 
    then
        echo correct prefix
    else
        continue
    fi
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "infilename: $nrrdfilename" "yellow"
    outfilename=${out_dir}${out_prefix}_${suffix}.nrrd
    EchoWithColor  "outfilename: $outfilename" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    shapeworks read-image --name $nrrdfilename extract-label --label 1.0 write-image --name $seg_label
    
done
