#!/bin/bash

##################################################################################
# File:    RunShapeWorksSingleScaleWithConstraints.sh
# Authors: Praful Agrawal
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC
# Purpose: Given a set of groomed DTs, run shapeworks with one scale (this will work on max two groups)
#          previous scale or initial correspondence model can be passed using init_lpts_dir (assumed to be shapeworks output of a previous scale
# Notes:
##################################################################################

# adding related-binaries to system path
source Utils/Utils.sh


ShapeWorksEXE='ShapeWorksRun' 

# default shapeworks parameters
num_particles=512
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
init_tolerance=0.0000001 # to test convergence
opt_tolerance=0.0000001 # to test convergence

verbosity=1

# to model only one group, just don't pass the second group
data_dir=""

# if both are empty, no initialization
init_with_prev_scale=0 # infer pre scale out dir

model_suffix=""

use_planes=''
use_spheres=''
spheres_radii=''
spheres_centers=''
planes_info=''

use_logging=0 # enabling this might cause I/O hanging when log files become huge

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
      
      --use_logging)
      use_logging="$2"
      shift
      ;;
      
      --verbosity)
      verbosity="$2"
      shift
      ;;
      
      --init_with_prev_scale)
      init_with_prev_scale="$2"
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
      
      --model_suffix)
      model_suffix="$2"
      shift
      ;;
      
      --num_particles)
      num_particles="$2"
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
      
      --use_planes)
      use_planes="$2"
      shift
      ;;
      
      --planes_info)
      planes_info="$2"
      shift
      ;;
      
      --use_spheres)
      use_spheres="$2"
      shift
      ;;
      
      --spheres_radii)
      spheres_radii="$2"
      shift
      ;;
      
      --spheres_centers)
      spheres_centers="$2"
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

initial_relative_weighting=$alpha_init #0.05
relative_weighting=$alpha_final #1.0
starting_regularization=$start_reg #10
ending_regularization=$end_reg #0.01
recompute_regularization_interval=2

if [ ! -z "$model_suffix" -a "$model_suffix" != " " ]; then
    echo $model_suffix
    model_suffix=_${model_suffix}
fi

paramSWDir=${out_dir}/paramfiles${model_suffix}/
modelDir=${out_dir}/modelWithConstraints${model_suffix}/
modelLogDir=${out_dir}/modelWithConstraints${model_suffix}_logs/

mkdir -p $paramSWDir
mkdir -p $modelDir

if [ $use_logging -eq 1 ]
then
    mkdir -p $modelLogDir
fi

# suffix to append all related files and folders
sfx=_xyz
if [ $use_normals -eq 1 ]
then
    sfx=${sfx}_wnormals
fi

sfx=${sfx}_a_i${initial_relative_weighting}_f${relative_weighting}

sfx=${sfx}_reg_s${starting_regularization}_e${ending_regularization}_r${recompute_regularization_interval}

ptsDir=${modelDir}final$sfx
mkdir -p $ptsDir
ptsDir=${ptsDir}/pts$num_particles/
mkdir -p $ptsDir

# total number of samples, dtfileanme and group id for each sample
num_samples=0
# group_ids=()
dtfilenames=()
sample_names=()
lptsfilenames=()
wptsfilenames=()

if [ $init_with_prev_scale -eq 1 ]
then
    prev_num_particles=$((num_particles / 2))
    initPtsDir=${modelDir}final$sfx/pts$prev_num_particles/
    init_lptsfilenames=()
fi

    echo $data_dir
    for dtfilename in $data_dir/*.nrrd ;
    do
        EchoWithColor "data: - dtfilename: $dtfilename" "yellow"
        
        dtfilenames+=("$dtfilename")
        
        sample_name=$( GetFilePrefix $dtfilename )
        sample_names+=("$sample_name")
        
        ptfilename=${ptsDir}${sample_name}_local.particles
        rm -rf $ptfilename
                    
        lptsfilenames+=("$ptfilename")
        
        #ptfilename=${ptsDir}${sample_name}_world.particles
        ptfilename=${sample_name}_world.particles #only include the file name in order to use ShapeWorksView2 from the results folder
        wptsfilenames+=("$ptfilename")
        
        if [ $init_with_prev_scale -eq 1 ]
        then
            ptfilename=${initPtsDir}${sample_name}_local.particles
            init_lptsfilenames+=("$ptfilename")
        fi
        
        num_samples=$(( $num_samples + 1 ))
        
    done
    
xmlfilename=${paramSWDir}correspondence$sfx.p$num_particles.xml 
rm -f $xmlfilename
# EchoWithColor "Writing correspondence parameter file: $xmlfilename" "light_green"

echo "<?xml version=\"1.0\" ?>" >> $xmlfilename

echo "" >> $xmlfilename
echo "<output_dir>  ${ptsDir} </output_dir>" >> $xmlfilename

echo "" >> $xmlfilename

echo "<number_of_particles> $num_particles </number_of_particles>" >>  $xmlfilename
echo "" >> $xmlfilename

echo "<checkpointing_interval> $checkpointing_interval </checkpointing_interval>" >> $xmlfilename
echo "<keep_checkpoints> $keep_checkpoints </keep_checkpoints>" >> $xmlfilename
echo "" >> $xmlfilename

echo "<iterations_per_split> $iterations_per_split </iterations_per_split>" >> $xmlfilename
echo "<optimization_iterations> $optimization_iterations </optimization_iterations>" >> $xmlfilename
echo "" >> $xmlfilename

echo "<starting_regularization> $starting_regularization </starting_regularization>" >> $xmlfilename
echo "<ending_regularization> $ending_regularization </ending_regularization>" >> $xmlfilename
echo "<recompute_regularization_interval> $recompute_regularization_interval </recompute_regularization_interval>" >> $xmlfilename
echo "" >> $xmlfilename

echo "<domains_per_shape> 1 </domains_per_shape>" >> $xmlfilename

echo "<adaptivity_mode> 3 </adaptivity_mode>" >> $xmlfilename #curcial for using constraints

echo "" >> $xmlfilename

echo "<relative_weighting> $relative_weighting</relative_weighting>" >> $xmlfilename
echo "<initial_relative_weighting> $initial_relative_weighting </initial_relative_weighting>" >> $xmlfilename
echo "" >> $xmlfilename

echo "<procrustes_interval> ${procrustes_interval} </procrustes_interval>" >> $xmlfilename
echo "<procrustes_scaling> ${with_procrustes_scaling} </procrustes_scaling>" >> $xmlfilename
echo "" >> $xmlfilename

echo "<save_init_splits> $save_init_splits </save_init_splits>" >> $xmlfilename

echo "" >> $xmlfilename

if (($use_normals == 0)); then

echo "<mesh_based_attributes> 0 </mesh_based_attributes>" >> $xmlfilename

else
echo "<mesh_based_attributes> 1 </mesh_based_attributes>" >> $xmlfilename
echo "<use_xyz> 1 </use_xyz>" >> $xmlfilename
echo "<attributes_per_domain> 0 </attributes_per_domain>" >> $xmlfilename
echo "<use_normals> 1 </use_normals>" >> $xmlfilename
echo "<attribute_scales>" >> $xmlfilename
echo "1.0" >> $xmlfilename
echo "1.0" >> $xmlfilename
echo "1.0" >> $xmlfilename
echo "$wt_normals" >> $xmlfilename 
echo "$wt_normals" >> $xmlfilename 
echo "$wt_normals" >> $xmlfilename 
echo "</attribute_scales>" >> $xmlfilename
fi

echo "" >> $xmlfilename

echo "<verbosity> $verbosity </verbosity>" >> $xmlfilename

echo "<inputs>" >> $xmlfilename
for dtfilename in "${dtfilenames[@]}"
do
    echo "${dtfilename}" >> $xmlfilename
done
echo "</inputs>" >> $xmlfilename

echo "" >> $xmlfilename

if [ ! -z "$use_planes" -a "$use_planes" != " " ]; then

echo "<num_planes_per_input>" >> $xmlfilename
while IFS= read -r line; do
    echo $line >> $xmlfilename
done < $use_planes
echo "</num_planes_per_input>" >> $xmlfilename
echo "" >> $xmlfilename

echo "<cutting_planes>" >> $xmlfilename
while IFS= read -r line; do
    echo $line >> $xmlfilename
done < $planes_info
echo "</cutting_planes>" >> $xmlfilename
echo "" >> $xmlfilename

fi

if [ ! -z "$use_spheres" -a "$use_spheres" != " " ]; then

echo "<spheres_per_domain>" >> $xmlfilename
while IFS= read -r line; do
    echo $line >> $xmlfilename
done < $use_spheres
echo "</spheres_per_domain>" >> $xmlfilename
echo "" >> $xmlfilename

echo "<sphere_radii>" >> $xmlfilename
while IFS= read -r line; do
    echo $line >> $xmlfilename
done < $spheres_radii
echo "</sphere_radii>" >> $xmlfilename
echo "" >> $xmlfilename

echo "<sphere_centers>" >> $xmlfilename
while IFS= read -r line; do
    echo $line >> $xmlfilename
done < $spheres_centers
echo "</sphere_centers>" >> $xmlfilename
echo "" >> $xmlfilename

fi

if [ $init_with_prev_scale -eq 1 ]
then
    echo "<point_files>" >> $xmlfilename
	for ptfilename in "${init_lptsfilenames[@]}"
	do
		echo "${ptfilename}" >> $xmlfilename
	done
	echo "</point_files>" >> $xmlfilename
fi

# writing analyze files for view2
xmlfilename=${ptsDir}ShapeWorksView2-Analyze.xml
rm -f $xmlfilename
EchoWithColor "Writing analyze parameter file (SWView2): $xmlfilename" "light_green"

echo "<point_files>" >> $xmlfilename
for ptfilename in "${wptsfilenames[@]}"
do
    echo "${ptfilename}" >> $xmlfilename
done
echo "</point_files>" >> $xmlfilename 
echo "" >> $xmlfilename

# writing the shapeworks post analyze file
xmlfilename=${ptsDir}ShapeWorksPost-Analyze.xml
rm -f $xmlfilename
EchoWithColor "Writing analyze parameter file (SWPost): $xmlfilename" "light_green"

echo "<point_files>" >> $xmlfilename
for ptfilename in "${wptsfilenames[@]}"
do
    echo "${ptfilename}" >> $xmlfilename
done
echo "</point_files>" >> $xmlfilename

echo "<num_points> $num_particles </num_points>" >> $xmlfilename
echo "<mesh_decimation_on> 1 </mesh_decimation_on>" >> $xmlfilename
echo "<mesh_decimation_percent>0.75</mesh_decimation_percent>" >> $xmlfilename

echo "<rep_DT>" >> $xmlfilename
echo "${dtfilenames[1]}" >> $xmlfilename
echo "</rep_DT>" >> $xmlfilename

echo "<rep_point>" >> $xmlfilename
echo "${lptsfilenames[1]}" >> $xmlfilename
echo "</rep_point>" >> $xmlfilename


# writing analyze files with group ids
#xmlfilename=${ptsDir}view2_analyze_groups.xml
#rm -f $xmlfilename
#EchoWithColor "Writing analyze parameter file with group ids: $xmlfilename" "light_green"

#echo "<point_files>" >> $xmlfilename
#for ptfilename in "${wptsfilenames[@]}"
#do
#    echo "${ptfilename}" >> $xmlfilename
#done
#echo "</point_files>" >> $xmlfilename
#echo "" >> $xmlfilename


# echo "<group_ids>" >> $xmlfilename
# for group_id in "${group_ids[@]}"
# do
#     echo "${group_id}" >> $xmlfilename
# done
# echo "</group_ids>" >> $xmlfilename 

# # write analyze for different groups
# for gid in "${!groups[@]}"
# do
#     group="${groups[$gid]}"
    
#     xmlfilename=${ptsDir}analyze_${group}.xml
#     rm -f $xmlfilename
#     EchoWithColor "Writing analyze parameter file for $group group: $xmlfilename" "light_green"

#     cur_group_id=$(( $gid + 1 )) # need 1-based index for analyze files
#     echo "<point_files>" >> $xmlfilename
    
#     for n in "${!wptsfilenames[@]}"
#     do
#         ptfilename="${wptsfilenames[$n]}"
#         group_id="${group_ids[$n]}"
        
#         if [ $group_id -eq $cur_group_id ]
#         then
#             echo "${ptfilename}" >> $xmlfilename
#         fi
#     done
    
#     echo "</point_files>" >> $xmlfilename 
# done

EchoWithColor "Running $ShapeWorksEXE ...." "light_red"

if [ $use_logging -eq 1 ]
then
    logFile=${modelLogDir}runLog$sfx.p$num_particles.txt
    echo "Generating Hierarchical correspondences..." 2>&1 | tee ${logFile}
else
    echo "Generating Hierarchical correspondences..."
fi

ptsDir=${modelDir}final$sfx/pts$num_particles
cp ${paramSWDir}correspondence$sfx.p$num_particles.xml $ptsDir

if [ $use_logging -eq 1 ]
then
    $ShapeWorksEXE ${paramSWDir}correspondence$sfx.p$num_particles.xml 2>&1 | tee ${logFile}
    echo "Done with $num_particles particles" 2>&1 | tee ${logFile}
else
    $ShapeWorksEXE ${paramSWDir}correspondence$sfx.p$num_particles.xml 
    echo "Done with $num_particles particles" 
fi    
