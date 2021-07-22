import argparse
import os
import glob
import shapeworks as sw
import numpy as np
import pandas as pd




def Run_Pipeline(args):
    output_directory = 'Output/femur_pvalues/'
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    dataset_name = 'femur_stats'
    sw.data.download_and_unzip_dataset(dataset_name, output_directory)

    shape_models = f'{output_directory}{dataset_name}/shape_model/'
    csvfile = 'femur_data.csv'
    data = pd.read_csv( f'{output_directory}{dataset_name}/{csvfile}')

    num_particles = 1024
    if args.tiny_test:
        permutations = 20
    else:
        permutations = 200

    data["filename"] = output_directory + data["filename"]
    pvalues = sw.stats.compute_pvalues_for_group_difference(data,num_particles,permutations)

    print("Saving the pvalues for the group difference at -" + f"{output_directory}{dataset_name}/")
    np.savetxt(f'{output_directory}{dataset_name}/femur_pvalues.txt',pvalues)