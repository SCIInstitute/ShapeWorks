#!/bin/bash

##################################################################################
# File:    RigidlyAlignAllSubjectsToReference.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Align all subjects to one reference (rigid transformation)
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
      
      -i|--img_prefix) # prefix of mri images
      img_prefix="$2"
      shift
      ;;
      
      -s|--seg_prefix) # prefix of the segmentation (binary image) to be used for registration
      seg_prefix="$2"
      shift
      ;;
      
      -f|--seg_suffix) # suffix of the segmentation to be used for file search
      seg_suffix="$2"
      shift
      ;;
      
      
      -o|--out_dir)
      out_dir="$2"
      shift
      ;;
      
      -r|--ref_id) # reference sample to be used to align all samples to
      ref_id="$2"
      shift
      ;;

      -x|--process_raw)
      process_raw="$2"
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

# grooming setting
foreground=1
isoValue=0.0
antialias_iterations=20
smoothing_iterations=1

# ANTS setting
numThreads=10
imageDimension=3
radius=4 # radius for cross correlation metric used during SyN stage (default = 4)
numHistogramBins=64 # for mutual information metric
splineDistance=26 # for BsplineSyn - 26 is the default
transformType=r  # sr or br
#        t: translation
#        r: rigid
#        a: rigid + affine
#        s: rigid + affine + deformable syn
#        sr: rigid + deformable syn
#        so: deformable syn only
#        b: rigid + affine + deformable b-spline syn
#        br: rigid + deformable b-spline syn
#        bo: deformable b-spline syn only
useHistogramMatching=1  # use histogram matching (default = 0 == false)

# antsRun=antsRegistrationSyN.sh
antsRun=../Utils/antsRegistrationSyNQuick2.sh # our adaptation to the original Quick script - this uses mutual information as the metric (note that we will do multimodal registration down the road)

ref_segfilename=${data_dir}${seg_prefix}.${ref_id}${seg_suffix}.nrrd
ref_imgfilename=${data_dir}${img_prefix}.${ref_id}${seg_suffix}.nrrd

mkdir -p $out_dir

# grooming reference shape
mkdir -p ${out_dir}reference/
basename=${out_dir}reference/${seg_prefix}.${ref_id}${seg_suffix}

ref_dtnrrdfilename=${basename}.DT.nrrd
ref_tpdtnrrdfilename=${basename}.tpSmoothDT.nrrd
ref_isonrrdfilename=${basename}.ISO.nrrd
ref_binnrrdfilename=${basename}.BIN.nrrd

EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
EchoWithColor "Grooming reference  - isolate, hole file, antialias and distance transform generation .................." "light_green"
EchoWithColor "${ref_segfilename}" "light_green"
EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"

shapeworks read-image --name ${ref_segfilename} extract-label --label $foreground write-image --name ${ref_segfilename}
CloseHoles --inFilename ${ref_segfilename} --outFilename  ${ref_segfilename}
shapeworks readimage --name ${ref_segfilename} antialias --numiterations $antialias_iterations writeimage --name ${ref_dtnrrdfilename} 
FastMarching --inFilename ${ref_dtnrrdfilename} --outFilename  ${ref_dtnrrdfilename} --isoValue $isoValue

# xmlfilename=${basename}.genDT.xml
# rm -rf $xmlfilename
# echo "<?xml version=\"1.0\" ?>" >> "$xmlfilename"
# echo "<background> 0.000000 </background>" >> "$xmlfilename"
# echo "<foreground> 1.000000 </foreground>" >> "$xmlfilename"
# echo "<pad> 0 </pad>" >> "$xmlfilename"
# echo "<transform_file> translations </transform_file>" >> "$xmlfilename"
# echo "<antialias_iterations> ${antialias_iterations} </antialias_iterations>" >> "$xmlfilename"
# echo "<fastmarching_isovalue> 0.000000 </fastmarching_isovalue>" >> "$xmlfilename"
# echo "<verbose> 1 </verbose>" >> "$xmlfilename"
# echo "<inputs>" >> "$xmlfilename"
# echo "${ref_segfilename}" >> "$xmlfilename"
# echo "</inputs>" >> "$xmlfilename"
# echo "<outputs>" >> "$xmlfilename"
# echo "${ref_dtnrrdfilename}" >> "$xmlfilename"
# echo "</outputs>" >> "$xmlfilename"
# 
# ShapeWorksGroom $xmlfilename isolate hole_fill antialias fastmarching
unu save -i ${ref_dtnrrdfilename} -o ${ref_dtnrrdfilename} -f nrrd -e raw

EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
EchoWithColor " TPSmooth reference distance transform  .................." "light_green"
EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"

xmlfilename=${basename}.tpSmoothDT.xml
rm -rf $xmlfilename
echo "<propagationScale> 20.0 </propagationScale>" >> "$xmlfilename"
echo "<alpha> 10.5 </alpha>" >> "$xmlfilename"
echo "<beta> 10.0 </beta>" >> "$xmlfilename"
echo "<isoValue> 0 </isoValue>" >> "$xmlfilename"
echo "<smoothing_iterations> $smoothing_iterations </smoothing_iterations>" >> "$xmlfilename"
echo "<verbose> 1 </verbose>" >> "$xmlfilename"
echo "<inputs>" >> "$xmlfilename"
echo "${ref_dtnrrdfilename}" >> "$xmlfilename"
echo "</inputs>" >> "$xmlfilename"
echo "<outputs>" >> "$xmlfilename"
echo "${ref_isonrrdfilename}" >> "$xmlfilename"
echo "</outputs>" >> "$xmlfilename"
echo "<dtFiles>" >> "$xmlfilename"
echo "${ref_tpdtnrrdfilename}" >> "$xmlfilename"
echo "</dtFiles>" >> "$xmlfilename"

TopologyPreservingSmoothing $xmlfilename

ThresholdImages --inFilename ${ref_tpdtnrrdfilename} --outFilename ${ref_binnrrdfilename} --lowerThresholdLevel -0.00001 
unu save -i ${ref_binnrrdfilename} -o ${ref_binnrrdfilename} -f nrrd -e raw

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
    
    # remove all occurences of underscore # no need for this
    # suffix=$( RemoveAllOccurrenceOf $suffix "_" )
    
    # remove all occurences of dot
    suffix=$( RemoveAllOccurrenceOf $suffix "." )

    if [[ $( StringContainsOnlyNumbers $suffix ) -eq 1 ]]; 
    then
        echo correct prefix
    fi

    subject_id=$suffix
    imgfilename=${data_dir}${img_prefix}.${subject_id}${seg_suffix}.nrrd
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "segfilename $segfilename" "yellow"
    EchoWithColor  "imgfilename $imgfilename" "yellow"
    echo
    EchoWithColor "ref_segfilename $ref_segfilename" "yellow"
    EchoWithColor  "ref_imgfilename $ref_imgfilename" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    
    basename=${out_dir}${seg_prefix}.${subject_id}${seg_suffix}
    
    dtnrrdfilename=${basename}.DT.nrrd
    tpdtnrrdfilename=${basename}.tpSmoothDT.nrrd
    isonrrdfilename=${basename}.ISO.nrrd
    binnrrdfilename=${basename}.BIN.nrrd
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
    EchoWithColor "[1 of 4] Grooming - isolate, hole file, antialias and distance transform generation .................." "light_green"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
         
    shapeworks read-image --name ${segfilename} extract-label --label $foreground write-image --name ${segfilename}
    CloseHoles --inFilename ${segfilename} --outFilename  ${segfilename}
    shapeworks readimage --name ${segfilename} antialias --numiterations $antialias_iterations writeimage --name ${dtnrrdfilename} 
    FastMarching --inFilename ${dtnrrdfilename} --outFilename  ${dtnrrdfilename} --isoValue $isoValue
    
    #     xmlfilename=${basename}.genDT.xml
    #     rm -rf $xmlfilename
    #     echo "<?xml version=\"1.0\" ?>" >> "$xmlfilename"
    #     echo "<background> 0.000000 </background>" >> "$xmlfilename"
    #     echo "<foreground> 1.000000 </foreground>" >> "$xmlfilename"
    #     echo "<pad> 0 </pad>" >> "$xmlfilename"
    #     echo "<transform_file> translations </transform_file>" >> "$xmlfilename"
    #     echo "<antialias_iterations> ${antialias_iterations} </antialias_iterations>" >> "$xmlfilename"
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
    unu save -i ${dtnrrdfilename} -o ${dtnrrdfilename} -f nrrd -e raw
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
    EchoWithColor "[2 of 4] TPSmooth distance transform  .................." "light_green"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
    
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
    
    EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
    EchoWithColor "[3 of 4] Running ANTS .................." "light_green"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
        
    outputPrefix=${out_dir}${seg_prefix}.${subject_id}_aligned_to_${ref_id}_
    ${antsRun} -d $imageDimension -f $ref_tpdtnrrdfilename -m $tpdtnrrdfilename -o $outputPrefix -n $numThreads -t $transformType -r $numHistogramBins -s $splineDistance #-j $useHistogramMatching 
    
    # do the warp and compare with warped --- to make sure that we are using compose and warp correctly --- sanity check
    EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
    EchoWithColor "[4 of 4] Apply the estimated tranformation to align seg/img of current subject to the reference .................." "light_green"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "light_green"
    
    affineFilename=${outputPrefix}0GenericAffine.mat
    
    # transform the distance transform
    movingToFixedFilename=${out_dir}${seg_prefix}.${subject_id}${seg_suffix}.DT.aligned.nrrd
    WarpImageMultiTransform $imageDimension $tpdtnrrdfilename $movingToFixedFilename -R $ref_segfilename $affineFilename 
    
    # threshold the transformed distance transform
    movingToFixedBinFilename=${out_dir}${seg_prefix}.${subject_id}${seg_suffix}.aligned.nrrd
    ThresholdImages --inFilename $movingToFixedFilename --outFilename $movingToFixedBinFilename --lowerThresholdLevel -0.00001 
    unu save -i $movingToFixedBinFilename -o $movingToFixedBinFilename -f nrrd -e raw
    
    if [ $process_raw -eq 1 ]
    then
        # transform the corresponding image
        movingToFixedFilename=${out_dir}${img_prefix}.${subject_id}${seg_suffix}.aligned.nrrd
        WarpImageMultiTransform $imageDimension $imgfilename $movingToFixedFilename -R $ref_imgfilename $affineFilename
    fi            
done

# AverageImages 3 ${out_dir}${seg_prefix}_average.nrrd 0 ${out_dir}${seg_prefix}*${seg_suffix}.aligned.nrrd

