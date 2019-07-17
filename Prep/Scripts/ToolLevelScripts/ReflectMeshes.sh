#!/bin/bash

##################################################################################
# File:    ReflectMeshes.sh
# Authors: Praful Agrawal
# Date:    Fall 2019
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Reflect meshes from left to right or vice versa
# Notes:
##################################################################################

scriptHome=../

reflect_direction=0
use_ply=1
# input parameters
while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -i|--input_dir)
      input_dir="$2"
      shift
      ;;
      
      -o|--output_dir) 
      out_dir="$2"
      shift
      ;;
      
      -d|--direction) # direction of reflection, default value - 0, x-axis
      reflect_direction="$2"
      shift
      ;;
      
      -p|--use_ply) # format of input files - ply or vtk ?
      use_ply="$2"
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

if [ $use_ply -eq 1 ]
then
    mesh_format=ply
else
    mesh_format=vtk
fi

mkdir -p $out_dir
mkdir -p $out_dir/parmafiles
xmlfilename=${out_dir}/parmafiles/reflect_meshes.xml
rm -rf $xmlfilename

echo "<?xml version=\"1.0\" ?>" >> $xmlfilename

echo "<direction> ${reflect_direction} </direction>" >> $xmlfilename

echo "<read_as_ply> ${use_ply} </read_as_ply>" >> $xmlfilename

echo "" >> $xmlfilename

echo "<inputs>" >> $xmlfilename

for meshes in $(find $input_dir -name "*.${mesh_format}" | sort -t '\0' ) ;
do
    echo "$meshes" >> $xmlfilename
done

echo "</inputs>" >> $xmlfilename

echo "<outputs>" >> $xmlfilename

for meshes in $(find $input_dir -name "*.${mesh_format}" | sort -t '\0' ) ;
do
    echo "${meshes/${input_dir}/${out_dir}}" >> $xmlfilename
done

echo "</outputs>" >> $xmlfilename

ReflectMeshes $xmlfilename
