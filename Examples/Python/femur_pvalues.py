import os
import shapeworks as sw
import numpy as np
import pandas as pd
import json

def Run_Pipeline(args):
    output_directory = 'Output/femur_pvalues/'
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    dataset_name = 'femur_pvalues'
    sw.download_and_unzip_dataset(dataset_name, output_directory)
    swproj_file = f'{output_directory}{dataset_name}/"femur_pvalues.swproj"'
    
    csvfile = 'femur_data.csv'
    data = pd.read_csv(f'{output_directory}{dataset_name}/{csvfile}')

    num_particles = 1024
    if args.tiny_test:
        permutations = 10
    else:
        permutations = 200

    print("Computing p-values")
    data["filename"] = output_directory + data["filename"]
    pvalues = sw.stats.compute_pvalues_for_group_difference(data, num_particles, permutations)

    filename = f"{output_directory}femur_pvalues.txt"
    print(f"Saving the pvalues for the group difference: {filename}")
    np.savetxt(filename, pvalues)

    print("Performing LDA")
    group1_x,group2_x,group1_pdf,group2_pdf,group1_map,group2_map = sw.stats.lda(data)
    sw.plot.lda_plot(group1_x,group2_x,group1_pdf,group2_pdf,group1_map,group2_map,output_directory,['Femur Pathology','Femur Control'])
