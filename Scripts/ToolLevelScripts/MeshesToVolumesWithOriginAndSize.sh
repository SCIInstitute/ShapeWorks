#!/bin/bash

##################################################################################
# File:    MeshesToVolumesWithOriginAndSize
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: Coherex-Appendage project
# Purpose: Convert meshes to volumes using fids using precomputed origin and size
# Notes:
##################################################################################

# adding related-binaries to system path
source ../Utils/Utils.sh # common utility functions

mesh_extension="vtk"

number_of_subvoxels=2
number_of_voxels=4
narrow_band=10
ball_radius_factor=1
num_threads=10

num_meshes_in_parallel=1

# input parameters
while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -d|--data_dir)
      data_dir="$2"
      shift
      ;;

      -o|--out_dir)
      out_dir="$2"
      shift
      ;;
      
      --num_threads)
      num_threads="$2"
      shift
      ;;
      
      --size_filename)
      size_filename="$2"
      shift
      ;;
      
      --origin_filename)
      origin_filename="$2"
      shift
      ;;
      
      --num_meshes_in_parallel)
      num_meshes_in_parallel="$2"
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
      
      --spacing)
      spacing="$2"
      shift
      ;;
      
      --ball_radius_factor)
      ball_radius_factor="$2"
      shift
      ;;
      
      --narrow_band)
      narrow_band="$2"
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
# getting the size and origin
origin=()
did=0
while read -r line
do
    curDim=$line	
    origin[$did]=$curDim
    did=$(( $did + 1 ))
done < "$origin_filename"
origin_x=${origin[0]}
origin_y=${origin[1]}
origin_z=${origin[2]}
echo Origin: "${origin[@]}"

size=()
did=0
while read -r line
do
    curDim=$line	
    size[$did]=$curDim
    did=$(( $did + 1 ))
done < "$size_filename"
size_x=${size[0]}
size_y=${size[1]}
size_z=${size[2]}
echo Size: "${size[@]}"


meshCounter=0    
for mfilename in $(find $data_dir -name "${mesh_prefix}*${mesh_suffix}.${mesh_extension}" | sort -t '\0' ) ;
do

    if [ $meshCounter -lt $num_meshes_in_parallel ]
    then
        meshCounter=$((${meshCounter}+1))    
    else
        EchoWithColor "--- WAITING --- WAITING --- WAITING --- WAITING --- WAITING --- WAITING --- WAITING --- WAITING --- WAITING ---" "light_red"
        wait
        
        meshCounter=1
        EchoWithColor "--- RUSUME PARALLEL RUN --- RUSUME PARALLEL RUN --- RUSUME PARALLEL RUN --- RUSUME PARALLEL RUN --- RUSUME PARALLEL RUN ---" "light_red"
        
    fi
    
    prefix=$( GetFilePrefix ${mfilename} )
    
    # remove the occurences of the suffix to get the subject id
    subject_id=$( RemoveAllOccurrenceOf $prefix ${mesh_prefix} )
    subject_id=$( RemoveAllOccurrenceOf $subject_id ${mesh_suffix} )
    
    
    meshFilename=${data_dir}${mesh_prefix}${subject_id}${mesh_suffix}.${mesh_extension}
    
    if [ $mesh_extension == "vtk" ]
    then
        meshFilename_vtk=${data_dir}${mesh_prefix}${subject_id}${mesh_suffix}.${mesh_extension}
        meshFilename=${data_dir}${mesh_prefix}${subject_id}${mesh_suffix}.ply
        
        vtk2ply $meshFilename_vtk $meshFilename
    fi
        
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "subject_id: $subject_id" "yellow"
    EchoWithColor "meshFilename $meshFilename" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    xmlfilename=${out_dir}${mesh_prefix}${subject_id}${mesh_suffix}_GenerateFidsFilesFromMeshes.xml
    rm -rf $xmlfilename
    
    echo "<? xml version="1.0" ?>" >> $xmlfilename
    echo "<origin_x> ${origin_x} </origin_x>" >> $xmlfilename
    echo "<origin_y> ${origin_y} </origin_y>" >> $xmlfilename
    echo "<origin_z> ${origin_z} </origin_z>" >> $xmlfilename
    
    echo "<spacing_x> ${spacing} </spacing_x>" >> $xmlfilename
    echo "<spacing_y> ${spacing} </spacing_y>" >> $xmlfilename
    echo "<spacing_z> ${spacing} </spacing_z>" >> $xmlfilename
    
    echo "<size_x> ${size_x} </size_x>" >> $xmlfilename
    echo "<size_y> ${size_y} </size_y>" >> $xmlfilename
    echo "<size_z> ${size_z} </size_z>" >> $xmlfilename
    
    echo "<number_of_subvoxels> ${number_of_subvoxels} </number_of_subvoxels>" >> $xmlfilename
    echo "<number_of_voxels> ${number_of_voxels} </number_of_voxels>" >> $xmlfilename
    
    echo "<narrow_band> ${narrow_band} </narrow_band>" >> $xmlfilename
    echo "<ball_radius_factor> ${ball_radius_factor} </ball_radius_factor>" >> $xmlfilename
    
    echo "<num_threads> ${num_threads} </num_threads>" >> $xmlfilename
    echo "<visualize_fids> 0 </visualize_fids>" >> $xmlfilename

    echo "<mesh>" >> $xmlfilename 
    echo "${meshFilename}" >> $xmlfilename
    echo "</mesh>" >> $xmlfilename 
    
    GenerateFidsFilesFromMeshes $xmlfilename     
done

mv ${data_dir}*.nrrd ${out_dir}
mv ${data_dir}*.fids ${out_dir}
cp ${data_dir}*.vtk ${out_dir}