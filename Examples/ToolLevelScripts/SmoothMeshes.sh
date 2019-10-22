#!/bin/bash

##################################################################################
# File:    SmoothMeshes
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Perform laplacian smoothing of mesh (vtk)
# Notes:
##################################################################################

scriptHome=../

mesh_prefix=""
mesh_suffix=""

mesh_extension="vtk"
smoothing_iterations=1
relaxation_factor=0.5

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
      
      --mesh_prefix)
      mesh_prefix="$2"
      shift
      ;;
      
      --mesh_suffix)
      mesh_suffix="$2"
      shift
      ;;
      
      --mesh_extension)
      mesh_extension="$2"
      shift
      ;;
      
      --smoothing_iterations)
      smoothing_iterations="$2"
      shift
      ;;
      
      --relaxation_factor)
      relaxation_factor="$2"
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
mkdir -p $out_dir/paramfiles

for meshfilename in $(find $data_dir -name "${mesh_prefix}*${mesh_suffix}.${mesh_extension}" | sort -t '\0' ) ;
do

    prefix=$( GetFilePrefix ${meshfilename} )
    
    meshfilename_vtk=${data_dir}${prefix}.vtk
    meshfilename_out_vtk=${out_dir}${prefix}.smooth${smoothing_iterations}.vtk
     
    if [ $mesh_extension == "ply" ]
    then
        ply2vtk $meshfilename $meshfilename_vtk
    fi
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "meshFilename $meshfilename" "yellow"
    EchoWithColor "meshFilename_vtk $meshfilename_vtk" "yellow"
    EchoWithColor "meshfilename_out_vtk $meshfilename_out_vtk" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"

    xmlfilename=${out_dir}/paramfiles/${prefix}_SmoothMesh.xml
    rm -rf $xmlfilename
    echo "<?xml version=\"1.0\" ?>" >> $xmlfilename
    
    echo "<inputs>" >> $xmlfilename
    echo "${meshfilename_vtk}" >> $xmlfilename
    echo "</inputs>" >> $xmlfilename
    
    echo "<outputs>" >> $xmlfilename
    echo "${meshfilename_out_vtk}" >> $xmlfilename
    echo "</outputs>" >> $xmlfilename
    
    echo "<iterations>" >> $xmlfilename
    echo "${smoothing_iterations}" >> $xmlfilename
    echo "</iterations>" >> $xmlfilename
    
    echo "<relaxation_factor>" >> $xmlfilename
    echo "${relaxation_factor}" >> $xmlfilename
    echo "</relaxation_factor>" >> $xmlfilename
    
    SmoothMesh $xmlfilename 
    
    if [ $mesh_extension == "ply" ]
    then
        meshfilename_out_ply=${out_dir}${prefix}.smooth${smoothing_iterations}.ply
        vtk2ply $meshfilename_out_vtk $meshfilename_out_ply
    fi   
    
done


