#!/bin/bash

##################################################################################
# File:    FindLargestBoundingBox.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: Coherex-Appendage project
# Purpose: Align all subjects to one reference (rigid transformation)
# Notes:
##################################################################################

# adding related-binaries to system path
source ../setup.txt # works for server as well
source ../Utils/Utils.sh # common utility functions

padding_size=0
# input parameters
while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -d|--data_dir)
      data_dir="$2"
      shift
      ;;

      -s|--seg_prefix) # prefix of the segmentation (binary image) to be used 
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

out_prefix=${data_dir}${seg_prefix}_largest_bounding_box
txtfilename=${data_dir}${seg_prefix}_filenames.txt
rm -rf $txtfilename
rm -rf ${out_prefix}*.txt

echo $txtfilename
for segfilename in $(find $data_dir -name "${seg_prefix}*${seg_suffix}.nrrd" | sort -t '\0' ) ;
do
    echo $segfilename
    echo $segfilename >> $txtfilename
done

FindLargestBoundingBox --paddingSize $padding_size --inFilename $txtfilename --outPrefix $out_prefix

