#!/bin/bash

##################################################################################
# File:    GroomAndExtractMeshesFromSegmentation.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Generate QCed meshes from binary segmentations (grooming is performed to extract smooth isosurfaces)
# Notes:
##################################################################################

# adding related-binaries to system path
source ../setup.txt # works for server as well
source ../Utils/Utils.sh # common utility functions

percentage=50
decimal=0.5
smoothing_iterations=2
antialias_iterations=40
# input parameters
while [[ $# > 1 ]]
do
  key="$1"
  case $key in
  
      -d|--data_dir)
      data_dir="$2"
      shift
      ;;
      
      -s|--seg_prefix) # prefix of the segmentation (binary image) 
      seg_prefix="$2"
      shift
      ;;
      
      -f|--seg_suffix) # prefix of the segmentation (binary image) 
      seg_suffix="$2"
      shift
      ;;
      
      -o|--out_dir)
      out_dir="$2"
      shift
      ;;
      
      -i|--smoothing_iterations)
      smoothing_iterations="$2"
      shift
      ;;
      
      --antialias_iterations)
      antialias_iterations="$2"
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

segPrefixLength=${#seg_prefix}
for segfilename in $(find $data_dir -name "${seg_prefix}*${seg_suffix}.nrrd" | sort -t '\0' ) ;
do
    prefix=$( GetFilePrefix ${segfilename} )
    nbr_of_dots=$( CountNumberOfOccurrencesOf  $prefix ".")
    tmp=1
    nbr_of_dots=$(expr $nbr_of_dots - $tmp) 
    count=0
    while [ $count -lt $nbr_of_dots ];
    do
        prefix=$( GetFilePrefix ${prefix} ) # remove .XX
        count=$((count + 1))
    done

    # make sure that the current file matches with the input prefix (laa_ prefix will also matach laa_wall)
    suffix=${prefix:segPrefixLength}
    
    # # remove all occurences of underscore # no need for this
    # suffix=$( RemoveAllOccurrenceOf $suffix "_" )
    
    # remove all occurences of dot
    suffix=$( RemoveAllOccurrenceOf $suffix "." )

    if [[ $( StringContainsOnlyNumbers $suffix ) -eq 1 ]]; 
    then
        echo correct prefix
    fi
    
    subject_id=$suffix    
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "segfilename $segfilename" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    basename=${out_dir}${seg_prefix}_${subject_id}${seg_suffix}
    
    dtnrrdfilename=${basename}.DT.nrrd
    tpdtnrrdfilename=${basename}.tpSmoothDT.nrrd
    isonrrdfilename=${basename}.ISO.nrrd
    vtkfilename=${basename}.tpSmoothDT.vtk
    vtkfilename_preview=${basename}.tpSmoothDT.preview${percentage}.vtk

    echo generating mesh for $basename
    
    foreground=1
    
    isoValue=0.0
    ExtractGivenLabelImage --inFilename ${segfilename} --outFilename  ${segfilename} --labelVal $foreground
    CloseHoles --inFilename ${segfilename} --outFilename  ${segfilename}
    AntiAliasing --inFilename ${segfilename} --outFilename  ${dtnrrdfilename} --numIterations $antialias_iterations
    FastMarching --inFilename ${dtnrrdfilename} --outFilename  ${dtnrrdfilename} --isoValue $isoValue
    
    #     xmlfilename=${basename}.genDT.xml
    #     rm -rf $xmlfilename
    #     echo "<?xml version=\"1.0\" ?>" >> "$xmlfilename"
    #     echo "<background> 0.000000 </background>" >> "$xmlfilename"
    #     echo "<foreground> 1.000000 </foreground>" >> "$xmlfilename"
    #     echo "<pad> 0 </pad>" >> "$xmlfilename"
    #     echo "<transform_file> translations </transform_file>" >> "$xmlfilename"
    #     echo "<antialias_iterations> 40 </antialias_iterations>" >> "$xmlfilename"
    #     echo "<fastmarching_isovalue> 0.000000 </fastmarching_isovalue>" >> "$xmlfilename"
    #     echo "<verbose> 1 </verbose>" >> "$xmlfilename"
    #     echo "<inputs>" >> "$xmlfilename"
    #     echo "${segfilename}" >> "$xmlfilename"
    #     echo "</inputs>" >> "$xmlfilename"
    #     echo "<outputs>" >> "$xmlfilename"
    #     echo "${dtnrrdfilename}" >> "$xmlfilename"
    #     echo "</outputs>" >> "$xmlfilename"
    # 
    #     ShapeWorksGroom $xmlfilename isolate hole_fill antialias fastmarching
    #unu save -i ${dtnrrdfilename} -o ${dtnrrdfilename} -f nrrd -e raw #Praful -- not needed, FastMarching output is not in gzip format
    
    xmlfilename=${basename}.tpSmoothDT.xml
    rm -rf $xmlfilename
    echo "<propagationScale> 20.0 </propagationScale>" >> "$xmlfilename"
    echo "<alpha> 10.5 </alpha>" >> "$xmlfilename"
    echo "<beta> 10.0 </beta>" >> "$xmlfilename"
    echo "<isoValue> 0 </isoValue>" >> "$xmlfilename"
    echo "<smoothing_iterations> $smoothing_iterations </smoothing_iterations>" >> "$xmlfilename"
    echo "<verbose> 1 </verbose>" >> "$xmlfilename"
    echo "<inputs>" >> "$xmlfilename"
    echo "${dtnrrdfilename}" >> "$xmlfilename"
    echo "</inputs>" >> "$xmlfilename"
    echo "<outputs>" >> "$xmlfilename"
    echo "${isonrrdfilename}" >> "$xmlfilename"
    echo "</outputs>" >> "$xmlfilename"
    echo "<dtFiles>" >> "$xmlfilename"
    echo "${tpdtnrrdfilename}" >> "$xmlfilename"
    echo "</dtFiles>" >> "$xmlfilename"

    TopologyPreservingSmoothing $xmlfilename 
    
    xmlfilename=${basename}.MeshFromDT.xml
    rm -rf $xmlfilename
    echo "<?xml version=\"1.0\" ?>" >> $xmlfilename
    echo "<inputs>" >> $xmlfilename
    echo "${tpdtnrrdfilename}" >> $xmlfilename
    echo "</inputs>" >> $xmlfilename
    echo "<outputs>" >> $xmlfilename
    echo "${vtkfilename}" >> $xmlfilename
    echo "</outputs>" >> $xmlfilename
    echo "<lsSmootherIterations> 1 </lsSmootherIterations>" >> $xmlfilename
    echo "<targetReduction> 0.0001 </targetReduction>" >> $xmlfilename
    echo "<featureAngle> 30 </featureAngle>" >> $xmlfilename
    echo "<preserveTopology> 1 </preserveTopology>" >> $xmlfilename
    
    MeshFromDistanceTransforms $xmlfilename
    PreviewCmd --inFile ${vtkfilename} --outFile ${vtkfilename_preview} --decimationPercentage $decimal
done

