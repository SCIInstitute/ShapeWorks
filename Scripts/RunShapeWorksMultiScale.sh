#!/bin/bash

##################################################################################
# File:    RunShapeWorksMultiScale.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Given a set of groomed DTs, run shapeworks with multiple scales using RunShapeWorksSingleScale.sh (this will work on max two groups)
# Notes:
##################################################################################

# adding related-binaries to system path
source Utils/Utils.sh

# default shapeworks parameters
max_num_particles=2048
min_num_particles=32
alpha_init=0.01
alpha_final=10
start_reg=100
end_reg=0.1
procrustes_interval=0
with_procrustes_scaling=0
use_normals=0
wt_normals=1

iterations_per_split=1000
optimization_iterations=2000
checkpointing_interval=200 #10
keep_checkpoints=0
save_init_splits=1

data_dir=""


# default output settings
model_suffix=""
verbosity=1
# input parameters
while [[ $# > 1 ]]
do
  key="$1"
  case $key in
    
      --data_dir)
      data_dir="$2"
      shift
      ;;
       
      --out_dir)
      out_dir="$2"
      shift
      ;;
      
      --model_suffix)
      model_suffix="$2"
      shift
      ;;
      
      --max_num_particles)
      max_num_particles="$2"
      shift
      ;;
      
      --min_num_particles)
      min_num_particles="$2"
      shift
      ;;
      
      --alpha_init)
      alpha_init="$2"
      shift
      ;;
      
      --alpha_final)
      alpha_final="$2"
      shift
      ;;
      
      --start_reg)
      start_reg="$2"
      shift
      ;;
      
      --end_reg)
      end_reg="$2"
      shift
      ;;
      
      --verbosity)
      verbosity="$2"
      shift
      ;;

      --init_iter)
      iterations_per_split="$2"
      shift
      ;;

      --opt_iter)
      optimization_iterations="$2"
      shift
      ;;

      --procrustes_interval)
      procrustes_interval="$2"
      shift
      ;;
      
      --with_procrustes_scaling)
      with_procrustes_scaling="$2"
      shift
      ;;
      
      --use_normals)
      use_normals="$2"
      shift
      ;;
      
      --wt_normals)
      wt_normals="$2"
      shift
      ;;
      
      --default)
      DEFAULT=YES
      shift
      ;;
      *)
      ;;
  esac
  shift
done

mkdir -p $out_dir

# number of particles in the multiscale strategy 
#EchoWithColor "Computing number of particles per scale ... " "light_green"
num_particles=()
curMax=$(( $min_num_particles / 2 ))
echo $curMax
cid=0
while [ ! $curMax -ge $max_num_particles ]
do
    curMax=$(( $curMax * 2 ))
    echo $curMax

    num_particles[$cid]=$curMax
    cid=$(( $cid + 1 ))
done
echo number of particles for multiscale : "${num_particles[@]}"

for ii in "${!num_particles[@]}"
do
    cur_num_particles="${num_particles[$ii]}"
    if [ $ii -eq 0 ]
    then
        init_with_prev_scale=0
    else
        init_with_prev_scale=1
    fi
    
    ./RunShapeWorksSingleScale.sh --data_dir $data_dir \
                                  --out_dir $out_dir \
                                  --model_suffix $model_suffix \
                                  --verbosity $verbosity \
                                  --num_particles $cur_num_particles --init_with_prev_scale $init_with_prev_scale\
                                  --alpha_init $alpha_init --alpha_final $alpha_final \
                                  --start_reg $start_reg --end_reg $end_reg \
                                  --use_normals $use_normals \
                                  --wt_normals $wt_normals \
                                  --init_iter $iterations_per_split \
                                  --opt_iter $optimization_iterations \
                                  --procrustes_interval $procrustes_interval \
                                  --with_procrustes_scaling $with_procrustes_scaling
done

