import os
import shapeworks as sw
import numpy as np
import pandas as pd
import json
import zipfile

def Run_Pipeline(args):
    output_directory = 'Output/femur_pvalues/'
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # unzip Data/femur_pvalues.zip
    zip_file = 'Data/femur_pvalues.zip'
    with zipfile.ZipFile(zip_file, 'r') as zip_ref:
        zip_ref.extractall("Output/")

    swproj_file = f'{output_directory}/femur_pvalues.swproj'
    csvfile = convert_swproj_to_csv(swproj_file)
    data = pd.read_csv(csvfile)

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

def convert_swproj_to_csv(swproj_file):
    f = open(swproj_file)
    data = json.load(f)
    file_paths, group_ids = [], []
    for femur_data in data["data"]:
        file_paths.append(femur_data["world_particles_1"])
        group_ids.append(femur_data["group_ids"])
    df = pd.DataFrame({"filename": file_paths, "group_ids": group_ids})
    csv_file = swproj_file.replace('femur_pvalues.swproj', 'femur_data.csv')
    df.to_csv(csv_file, index=False)
    return csv_file
