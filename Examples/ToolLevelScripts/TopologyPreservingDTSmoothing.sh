#!/bin/bash

##################################################################################
# File:    TopologyPreservingDTSmoothing.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Smooth distance transforms without altering the shape topology
# Notes:
##################################################################################

scriptHome=../

dt_prefix=""
dt_suffix=""

smoothing_iterations=1

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
      
      --dt_prefix)
      dt_prefix="$2"
      shift
      ;;
      
      --dt_suffix)
      dt_suffix="$2"
      shift
      ;;
      
      -i|--smoothing_iterations)
      smoothing_iterations="$2"
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

for dtfilename in $(find $data_dir -name "${dt_prefix}*${dt_suffix}.nrrd" | sort -t '\0' ) ;
do
    echo $dtfilename
    prefix=$( GetFilePrefix ${dtfilename} )
    tpdtfilename=${out_dir}${prefix}.tpSmoothDT.nrrd
    isofilename=${out_dir}${prefix}.ISO.nrrd
     
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    EchoWithColor "dtfilename $dtfilename" "yellow"
    EchoWithColor "tpdtfilename $tpdtfilename" "yellow"
    EchoWithColor "isofilename $isofilename" "yellow"
    EchoWithColor "-------------------------------------------------------------------------------------------------" "yellow"
    
    
    xmlfilename=${out_dir}/paramfiles/${prefix}_tpSmoothDT.xml
    rm -rf $xmlfilename
    echo "<propagationScale> 20.0 </propagationScale>" >> "$xmlfilename"
    echo "<alpha> 10.5 </alpha>" >> "$xmlfilename"
    echo "<beta> 10.0 </beta>" >> "$xmlfilename"
    echo "<isoValue> 0 </isoValue>" >> "$xmlfilename"
    echo "<smoothing_iterations> $smoothing_iterations </smoothing_iterations>" >> "$xmlfilename"
    echo "<verbose> 1 </verbose>" >> "$xmlfilename"
    echo "<inputs>" >> "$xmlfilename"
    echo "${dtfilename}" >> "$xmlfilename"
    echo "</inputs>" >> "$xmlfilename"
    echo "<outputs>" >> "$xmlfilename"
    echo "${isofilename}" >> "$xmlfilename"
    echo "</outputs>" >> "$xmlfilename"
    echo "<dtFiles>" >> "$xmlfilename"
    echo "${tpdtfilename}" >> "$xmlfilename"
    echo "</dtFiles>" >> "$xmlfilename"

    TopologyPreservingSmoothing $xmlfilename 
    
done



