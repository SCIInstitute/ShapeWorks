#!/bin/bash

##################################################################################
# File:    RasterizationVolumeOriginAndSize
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Compute the origin and size of the distance transform volume where all meshes will fit into
# Notes:
##################################################################################

# adding related-binaries to system path
source /usr/sci/projects/coherex/EXPERIMENTS/SCRIPTS/Utils.sh

mesh_prefix=""
mesh_suffix=""

mesh_extension="vtk"


spacing=0.5
narrow_band=10

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
      
      --spacing)
      spacing="$2"
      shift
      ;;
      
      --narrow_band)
      narrow_band="$2"
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
# origin and size settings
padding=$(echo "1.3*($narrow_band/$spacing)" | bc -l)
pad_pixels="${padding%.*}" # get integer value
echo pad_pixels: $pad_pixels
centering=0

out_origin_filename=${out_dir}origin_sp${spacing}_nb${narrow_band}.txt
out_size_filename=${out_dir}size_sp${spacing}_nb${narrow_band}.txt

EchoWithColor "out_origin_filename $out_origin_filename" "yellow"
EchoWithColor "out_size_filename $out_size_filename" "yellow"
    
rm -rf ${out_origin_filename}
rm -rf ${out_size_filename}

xmlfilename=${out_dir}ComputeRasterizationVolumeOriginAndSize.xml
rm -rf $xmlfilename

echo "<? xml version="1.0" ?>" >> $xmlfilename
echo "<centering> ${centering} </centering>" >> $xmlfilename

echo "<spacing_x> ${spacing} </spacing_x>" >> $xmlfilename
echo "<spacing_y> ${spacing} </spacing_y>" >> $xmlfilename
echo "<spacing_z> ${spacing} </spacing_z>" >> $xmlfilename

echo "<pad_pixels> ${pad_pixels} </pad_pixels>" >> $xmlfilename
echo "<out_origin_filename> ${out_origin_filename} </out_origin_filename>" >> $xmlfilename
echo "<out_size_filename> ${out_size_filename} </out_size_filename>" >> $xmlfilename

echo "<mesh>" >> $xmlfilename

for meshfilename in $(find $data_dir -name "${mesh_prefix}*${mesh_suffix}.${mesh_extension}" | sort -t '\0' ) ;
do        
    EchoWithColor "meshFilename $meshfilename" "yellow"
    echo "${meshfilename}" >> $xmlfilename 
done
echo "</mesh>" >> $xmlfilename

ComputeRasterizationVolumeOriginAndSize $xmlfilename


