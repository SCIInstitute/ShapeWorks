#!/bin/bash

##################################################################################
# File:    MeshesPreviewQualityControl.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: Coherex-Appendage project
# Purpose: decimate/remesh meshes to speed up fids computation and fix mesh quality
# Notes:
##################################################################################

# adding related-binaries to system path
source ../Utils/Utils.sh # common utility functions

mesh_prefix=""
mesh_suffix=""

mesh_extension="vtk"

decimation_decimal=0.95
decimation_percentage=95
nThreads=8
use_preview=0

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
      
      --use_preview)
      use_preview="$2"
      shift
      ;;
      
      --decimation_decimal)
      decimation_decimal="$2"
      shift
      ;;
      
      --decimation_percentage)
      decimation_percentage="$2"
      shift
      ;;
      
      --mesh_extension)
      mesh_extension="$2"
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
    meshfilename_out_vtk=${out_dir}${prefix}.preview${decimation_percentage}.vtk
     
    if [ $mesh_extension == "ply" ]
    then
        ply2vtk $meshfilename $meshfilename_vtk
    fi
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "meshFilename $meshfilename" "yellow"
    EchoWithColor "meshFilename_vtk $meshfilename_vtk" "yellow"
    EchoWithColor "meshfilename_out_vtk $meshfilename_out_vtk" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"

    if [ $use_preview -eq 1 ]
    then
         PreviewCmd --inFile ${meshfilename_vtk} --outFile ${meshfilename_out_vtk} \
                    --decimationPercentage ${decimation_decimal} --doLaplacianSmoothingAfterDecimation 1 --doLaplacianSmoothingBeforeDecimation 1 \
                    --smoothingLambda 0.1 --smoothingIterations 1
    else
        xmlfilename=${out_dir}${prefix}_MeshDecimation.xml
        rm -rf $xmlfilename
        echo "<?xml version=\"1.0\" ?>" >> $xmlfilename
        echo "<inputs>" >> $xmlfilename
        echo "${meshfilename_vtk}" >> $xmlfilename
        echo "</inputs>" >> $xmlfilename
        echo "<outputs>" >> $xmlfilename
        echo "${meshfilename_out_vtk}" >> $xmlfilename
        echo "</outputs>" >> $xmlfilename
        echo "<display> 0 </display>" >> $xmlfilename
        echo "<decimation_percentage> ${decimation_decimal} </decimation_percentage>" >> $xmlfilename
        echo "<gradation> 0.5 </gradation>" >> $xmlfilename
        echo "<number_of_threads> ${nThreads} </number_of_threads>" >> $xmlfilename
        
        MeshDecimation $xmlfilename # AVCD code - better triangulation of flat surfaces
    fi
    
    if [ $mesh_extension == "ply" ]
    then
        meshfilename_out_ply=${out_dir}${prefix}.preview${decimation_percentage}.ply
        vtk2ply $meshfilename_out_vtk $meshfilename_out_ply
    fi   
    
done


