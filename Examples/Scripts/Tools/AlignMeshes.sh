#!/bin/bash

##################################################################################
# File:    AlignMeshes
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Perform ICP registration on meshes
# Notes:
##################################################################################

scriptHome=../
mesh_prefix=""
mesh_suffix=""
mesh_extension="vtk"
registration_mode=similarity

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
      
      --registration_mode)
      registration_mode="$2"
      shift
      ;;
      
      --reference_mesh)
      reference_mesh="$2"
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
source ${scriptHome}/Utils/Utils.sh

mkdir -p $out_dir
mkdir -p $out_dir/paramfiles
mkdir -p $out_dir/transformations
ref_prefix=$( RemoveFileExtension $reference_mesh)
reference_mesh_vtk=${ref_prefix}.vtk

EchoWithColor "reference mesh $reference_mesh_vtk" "yellow"

ref_ext=$( GetFileExtension $reference_mesh)
# if [ $ref_ext == "ply" ]
# then
#     echo "This WORKS"
#     echo $reference_mesh
#     ply2vtk $reference_mesh $reference_mesh_vtk
# fi
echo $mesh_prefix
echo $mesh_suffix
echo $mesh_extension
for meshfilename in $(find $data_dir -name "${mesh_prefix}*${mesh_suffix}.${mesh_extension}" | sort -t '\0' ) ;
do

    prefix=$( GetFilePrefix ${meshfilename} )
    
    meshfilename_vtk=${data_dir}${prefix}.vtk
    meshfilename_out_vtk=${out_dir}${prefix}.${registration_mode}_icp.vtk
    transform_filename=${out_dir}/transformations/${prefix}.${registration_mode}_icp.txt
     
    if [ $mesh_extension == "ply" ]
    then
        ply2vtk $meshfilename $meshfilename_vtk
    fi
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "meshFilename $meshfilename" "yellow"
    EchoWithColor "meshFilename_vtk $meshfilename_vtk" "yellow"
    EchoWithColor "meshfilename_out_vtk $meshfilename_out_vtk" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"

    xmlfilename=${out_dir}/paramfiles/${prefix}_ICPRigid3DMeshRegistration.xml
    rm -rf $xmlfilename
    echo "<?xml version=\"1.0\" ?>" >> $xmlfilename
    
    echo "<source_mesh>" >> $xmlfilename
    echo "${meshfilename_vtk}" >> $xmlfilename
    echo "</source_mesh>" >> $xmlfilename
    
    echo "<target_mesh>" >> $xmlfilename
    echo "${reference_mesh_vtk}" >> $xmlfilename
    echo "</target_mesh>" >> $xmlfilename
    
    echo "<out_mesh>" >> $xmlfilename
    echo "${meshfilename_out_vtk}" >> $xmlfilename
    echo "</out_mesh>" >> $xmlfilename
    
    echo "<out_transform>" >> $xmlfilename
    echo "${transform_filename}" >> $xmlfilename
    echo "</out_transform>" >> $xmlfilename
    
    echo "<mode> ${registration_mode} </mode>" >> $xmlfilename
    echo "<icp_iterations> 1000 </icp_iterations>" >> $xmlfilename
    echo "<debug> 1 </debug>" >> $xmlfilename
    echo "<visualize> 0 </visualize>" >> $xmlfilename
    
    ICPRigid3DMeshRegistration $xmlfilename 
    
    if [ $mesh_extension == "ply" ]
    then
        meshfilename_out_ply=${out_dir}${prefix}.${registration_mode}_icp.ply
        vtk2ply $meshfilename_out_vtk $meshfilename_out_ply
    fi   
    
done
