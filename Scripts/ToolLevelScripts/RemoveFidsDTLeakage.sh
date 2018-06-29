#!/bin/bash

##################################################################################
# File:    RemoveFidsDTLeakage.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Remove fids-based distance tranform leakage that typically occur for meshes with sharp corners
# Notes:
##################################################################################

# adding related-binaries to system path
source ../Utils/Utils.sh # common utility functions

mesh_prefix=""
mesh_suffix=""

mesh_extension="vtk"

dt_suffix=""
fids_dt_suffix=""


# input parameters
while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -d|--data_dir)
      data_dir="$2"
      shift
      ;;
      
      --out_dir)
      out_dir="$2"
      shift
      ;;
      
      --dt_suffix)
      dt_suffix="$2"
      shift
      ;;
      
      --fids_dt_suffix)
      fids_dt_suffix="$2"
      shift
      ;;
      
      --mesh_prefix)
      mesh_prefix="$2"
      shift
      ;;
      
      --mesh_suffix)
      mesh_suffix="$2"
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

for meshfilename in $(find $data_dir -name "${mesh_prefix}*${mesh_suffix}.${mesh_extension}" | sort -t '\0' ) ;
do

    prefix=$( GetFilePrefix ${meshfilename} )
     
    meshfilename_vtk=${data_dir}${prefix}.vtk
    dtfilename=${data_dir}${prefix}.${dt_suffix}.nrrd
    fids_dtfilename=${data_dir}${prefix}.${fids_dt_suffix}.nrrd
    out_dtfilename=${out_dir}${prefix}.${fids_dt_suffix}.fixed.nrrd
     
    if [ $mesh_extension == "ply" ]
    then
        ply2vtk $meshfilename $meshfilename_vtk
    fi
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "meshFilename $meshfilename" "yellow"
    EchoWithColor "meshFilename_vtk $meshfilename_vtk" "yellow"
    EchoWithColor "dtfilename $dtfilename" "yellow"
    EchoWithColor "fids_dtfilename $fids_dtfilename" "yellow"
    EchoWithColor "out_dtfilename $out_dtfilename" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"

    xmlfilename=${out_dir}${prefix}_RemoveFidsDTLeakage.xml
    rm -rf $xmlfilename
    echo "<?xml version=\"1.0\" ?>" >> $xmlfilename
    
    echo "<fids_dist>" >> $xmlfilename  
    echo "${fids_dtfilename}" >> $xmlfilename
    echo "</fids_dist>" >> $xmlfilename  
    
    echo "<approx_dist>" >> $xmlfilename  
    echo "${dtfilename}" >> $xmlfilename
    echo "</approx_dist>" >> $xmlfilename  
    
    echo "<out_dist>" >> $xmlfilename  
    echo "${out_dtfilename}" >> $xmlfilename
    echo "</out_dist>" >> $xmlfilename  
    
    RemoveFidsDTLeakage $xmlfilename   
done



