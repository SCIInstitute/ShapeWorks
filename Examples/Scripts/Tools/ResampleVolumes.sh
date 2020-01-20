#!/bin/bash

##################################################################################
# File:    ResampleVolumes.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Resample volumes to be isotropic voxel space
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
      
      -p|--data_prefix)
      data_prefix="$2"
      shift
      ;;
      
      -o|--out_dir)
      out_dir="$2"
      shift
      ;;
      
      -s|--iso_spacing)
      iso_spacing="$2"
      shift
      ;;
      
      -c|--is_center_on)
      is_center_on="$2"
      shift
      ;;
      
      -b|--is_binary_image)
      is_binary_image="$2"
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

dataPrefixLength=${#data_prefix}

for nrrdfilename in $(find $data_dir -name "${data_prefix}*.nrrd" | sort -t '\0' ) ;
do
    prefix=$( GetFilePrefix ${nrrdfilename} )

    # make sure that the current file matches with the input prefix (laa_ prefix will also matach laa_wall)
    suffix=${prefix:dataPrefixLength}

    # remove all occurences of underscore # this is not needed!
    # suffix=$( RemoveAllOccurrenceOf $suffix "_" )
    # echo $suffix
    # remove all occurences of dot
    suffix=$( RemoveAllOccurrenceOf $suffix "." )

    if [[ $( StringContainsOnlyNumbers $suffix ) -eq 1 ]]; 
    then
        echo correct prefix
    fi

    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "infilename: $nrrdfilename" "blue"
    outfilename=${out_dir}${prefix}.isores.nrrd
    EchoWithColor  "outfilename: $outfilename" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    if [ $is_center_on -eq 1 ]
    then    
        if [ $is_binary_image -eq 1 ]
        then
            shapeworks readimage --name $nrrdfilename antialias resampleimage --isospacing $iso_spacing --isbinary 1 recenterimage binarize writeimage --name $outfilename
        else
            shapeworks readimage --name $nrrdfilename resampleimage --isospacing $iso_spacing recenterimage writeimage --name $outfilename
        fi
    else
        if [ $is_binary_image -eq 1 ]
        then
            shapeworks readimage --name $nrrdfilename antialias resampleimage --isospacing $iso_spacing --isbinary 1 binarize writeimage --name $outfilename
        else
            shapeworks readimage --name $nrrdfilename resampleimage --isospacing $iso_spacing writeimage --name $outfilename
        fi
    fi

    unu head $outfilename
        
done

