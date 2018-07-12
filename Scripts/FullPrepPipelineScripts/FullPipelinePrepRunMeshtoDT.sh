#!/bin/bash

##################################################################################
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Run shapeworks prep pipeline given a set of meshes 
# Notes:
##################################################################################

### [1] Preview : Does mesh decimation and cleaning to speed up the computation for fids
doPreview=0
decimation_decimal=0.95
decimation_percentage=95

### [2] Smoothing : Performs Mesh Smoothing
doSmooth=0
smoothing_iterations=1
relaxation_factor=0.5

### [3] Alignment : Performs Mesh Alignment
doAlign=0
registration_mode=similarity
reference_meshID=nih_n17_L_femur_aligned

### [4] Size and Origin : Computes the biggest bounding box size and origin
doSizeOrigin=0
spacing=0.5
narrow_band=10

### [5] Mesh to DT : Computes parameterfile and then the DT from the meshes
doMeshesToVolumes=1
ball_radius_factor=1

### [6] Fix DT Leakage : Fixes leakage in the distance transforms for the fids
doFixFidsDT=1

### [7] Topology Preserving Smoothing : Merforms smoothing on the DT tranforms
doTPSmoothDT=1 
TPsmoothing_iterations=1

source ../setup.txt # works for server as well
source ../Utils/Utils.sh # common utility functions

parentDir='' # this is where all the preprocessed data will lie
rawDataDir='' # this is the path to the data (raw Images and Segmentations)
mesh_extension='ply'

while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -p|--parent_dir)
      parentDir="$2"
      shift
      ;;
      
      -i|--raw_data_dir) # prefix of mri images
      rawDataDir="$2"
      shift
      ;;
      
      -g|--mesh_extension)
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

buffer_distance=2

if [ $doPreview -eq 1 ]
then

    ./MeshesPreviewQualityControl.sh  --data_dir ${rawDataDir} \
                                --out_dir ${parentDir}previewed/ \
                                --mesh_extension $mesh_extension \
                                --decimation_decimal $decimation_decimal \
                                --decimation_percentage $decimation_percentage \
                                --use_preview 1
                                
fi


if [ $doSmooth -eq 1 ]
then

    ./SmoothMeshes.sh  --data_dir ${parentDir}previewed/ \
                      --out_dir ${parentDir}smoothed/ \
                      --mesh_suffix preview${decimation_percentage} \
                      --mesh_extension $mesh_extension \
                      --smoothing_iterations $smoothing_iterations --relaxation_factor $relaxation_factor
                      
fi


if [ $doAlign -eq 1 ]
then
    reference_mesh=${parentDir}smoothed/${reference_meshID}.preview${decimation_percentage}.smooth${smoothing_iterations}.ply 

    ./AlignMeshes.sh  --data_dir ${parentDir}smoothed/ \
                      --out_dir ${parentDir}aligned/ \
                      --mesh_suffix preview${decimation_percentage}.smooth${smoothing_iterations} \
                      --mesh_extension $mesh_extension \
                      --reference_mesh $reference_mesh \
                      --registration_mode $registration_mode

fi

# compute the origin and size of the distance transform volume where all meshes will fit into
if [ $doSizeOrigin -eq 1 ]
then
    
    ./RasterizationVolumeOriginAndSize.sh --data_dir ${parentDir}aligned/ \
                                          --out_dir ${parentDir}origin_size/ \
                                          --spacing $spacing --narrow_band $narrow_band\
                                          --mesh_suffix preview${decimation_percentage}.smooth${smoothing_iterations}.${registration_mode}_icp 
fi

# convert meshes to distance transforms
if [ $doMeshesToVolumes -eq 1 ]
then
    origin_filename=${parentDir}origin_size/origin_sp${spacing}_nb${narrow_band}.txt
    size_filename=${parentDir}origin_size/size_sp${spacing}_nb${narrow_band}.txt

    ./MeshesToVolumesWithOriginAndSize.sh  --data_dir ${parentDir}aligned/ \
                                --out_dir ${parentDir}fidsDT/ \
                                --mesh_suffix preview${decimation_percentage}.smooth${smoothing_iterations}.${registration_mode}_icp \
                                --mesh_extension $mesh_extension \
                                --origin_filename $origin_filename\
                                --size_filename $size_filename\
                                --spacing $spacing --narrow_band $narrow_band\
                                --ball_radius_factor $ball_radius_factor \
                                --num_meshes_in_parallel 5 --num_threads 20
fi



if [ $doFixFidsDT -eq 1 ]
then
    ./RemoveFidsDTLeakage.sh --data_dir ${parentDir}fidsDT/ \
                             --out_dir ${parentDir}fidsDT/ \
                             --mesh_suffix preview${decimation_percentage}.smooth${smoothing_iterations}.${registration_mode}_icp \
                             --mesh_extension $mesh_extension \
                             --dt_suffix DT_r${ball_radius_factor}_sp${spacing}_nb${narrow_band} \
                             --fids_dt_suffix SignedDistMap_r${ball_radius_factor}_sp${spacing}_nb${narrow_band}
                                          
fi

if [ $doTPSmoothDT -eq 1 ]
then
    ./TopologyPreservingDTSmoothing.sh  --data_dir ${parentDir}fidsDT/ \
                                        --out_dir ${parentDir}groom/ \
                                        --dt_suffix preview${decimation_percentage}.smooth${smoothing_iterations}.${registration_mode}_icp.SignedDistMap_r${ball_radius_factor}_sp${spacing}_nb${narrow_band}.fixed \
                                        --smoothing_iterations $TPsmoothing_iterations
                                
    mkdir -p ${parentDir}DistanceTransforms/
    mv ${parentDir}groom/*.tpSmoothDT.nrrd ${parentDir}DistanceTransforms/                             
fi
