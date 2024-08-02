#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Common utility functions
"""
import os
import re
import numpy as np
from sklearn.cluster import SpectralClustering
import xml.etree.ElementTree as ET
from zipfile import ZipFile
import shapeworks as sw

def dataset_exists_check(use_case):
    existsFlag = False
    OutputDirectory = "Output/"
    for filename in os.listdir(OutputDirectory):
        if(use_case == filename):
            existsFlag = True
    return existsFlag

def generate_download_flag(outputDirectory,folder):
    download_flag = False
    #if output/dataset + subfolders exits 
    if(os.path.exists(outputDirectory+folder)):
            # if the folder is empty or has less than 3 files then download
            if(len(os.listdir(outputDirectory+folder))==0 or len(os.listdir(outputDirectory+folder))<3):
                download_flag = True
            else:
                print("Data available in " + folder + "  is sufficient, no new data will be downloaded")

    #if the subfolder folder does not exists then download
    else:
        download_flag = True        
    return download_flag

def download_and_unzip_dataset(datasetName, outputDirectory):
    # Check if the unzipped data is present and number of files are more than 3 for full use case
    if generate_download_flag(outputDirectory,datasetName):
        # check if the zipped data is present
        zipfile = 'Data/' + datasetName + ".zip"
        if not os.path.exists(zipfile):
            print("Can't find " + zipfile)
            import DatasetUtils
            DatasetUtils.downloadDataset(datasetName, destinationPath='./Data/')
        print("Unzipping " + zipfile + " into " + outputDirectory)
        with ZipFile(zipfile, 'r') as zipObj:
            zipObj.extractall(path=outputDirectory)

def get_file_list(directory, ending='', indices=[]):
    file_list = []
    for file in sorted(os.listdir(directory)):
        if ending in file:
            file_list.append(directory + file)
    if indices:
        file_list = [file_list[i] for i in indices]
    return file_list
   

def sample_images(inDataList, num_sample,domains_per_shape=1):
    
    if(domains_per_shape==1):
        print("\n########## Sample subset of data #########\n")
        D = np.zeros((len(inDataList), len(inDataList)))
        for i in range(len(inDataList)):
            image_1 = inDataList[i].toArray()
            x, y, z = image_1.shape
            for j in range(i, len(inDataList)):
                image_2 = inDataList[j].toArray()
                x, y, z = max(x, image_2.shape[0]), max(y, image_2.shape[1]), max(z, image_2.shape[2])
                image_1 = np.pad(image_1, (((x - image_1.shape[0]) // 2, (x - image_1.shape[0]) - (x - image_1.shape[0]) // 2),
                             ((y - image_1.shape[1]) // 2, (y - image_1.shape[1]) - (y - image_1.shape[1]) // 2),
                             ((z - image_1.shape[2]) // 2, (z - image_1.shape[2]) - (z - image_1.shape[2]) // 2)))
                image_2 = np.pad(image_2, (((x - image_2.shape[0]) // 2, (x - image_2.shape[0]) - (x - image_2.shape[0]) // 2),
                             ((y - image_2.shape[1]) // 2, (y - image_2.shape[1]) - (y - image_2.shape[1]) // 2),
                             ((z - image_2.shape[2]) // 2, (z - image_2.shape[2]) - (z - image_2.shape[2]) // 2)))
                D[i, j] = np.linalg.norm(image_1-image_2)

        D += D.T
        A = np.exp(- D ** 2 / (2. * np.std(np.triu(D))**2))
        
        print("Run Spectral Clustering for {} clusters ...".format(num_sample))
        model = SpectralClustering(n_clusters=num_sample,
                                        assign_labels="discretize",
                                        random_state=0, affinity='precomputed').fit(A)
        labels = list(model.labels_)
        samples_idx = []
        print("sample one data per cluster to have diverse samples!")
        for i in range(num_sample):
            samples_idx.append(labels.index(i))
      
    else:
        meshFilesList=[]
        for i in range(len(inDataList)):
            meshFilesList.append(inDataList[i].toMesh(0.5))          
        samples_idx = sample_meshes(meshFilesList,num_sample,domains_per_shape=domains_per_shape)

        

    
    print("\n###########################################\n")
    return samples_idx

def combine_domains(inMeshList,domains_per_shape=1):
    num_shapes = int(len(inMeshList)/domains_per_shape)
    newMeshList=[]
    for i in range(num_shapes):
        shape_d0 = inMeshList[i*domains_per_shape]
        for d in range(1,domains_per_shape):
            shape_d0+= inMeshList[(i*domains_per_shape)+d]
        newMeshList.append(shape_d0)
    return newMeshList



def sample_meshes(inMeshList, num_sample, printCmd=False,domains_per_shape=1):
    print("########## Sample subset of data #########")
    

    if(domains_per_shape>1):
        inMeshList = combine_domains(inMeshList,domains_per_shape)

    

    D = np.zeros((len(inMeshList), len(inMeshList)))
    for i in range(len(inMeshList)):
        for j in range(i, len(inMeshList)):
            mesh1 = inMeshList[i]
            mesh2 = inMeshList[j]
            dist = np.mean(mesh1.distance(mesh2)[0])
            D[i, j] = dist
    D += D.T
    A = np.exp(- D ** 2 / (2. * np.std(np.triu(D))**2))
    
    print("Run Spectral Clustering for {} clusters ...".format(num_sample))
    model = SpectralClustering(n_clusters=num_sample,
                                    assign_labels="discretize",
                                    random_state=0, affinity='precomputed').fit(A)
    labels = list(model.labels_)
    samples_idx = []
    print("sample one data per cluster to have diverse samples!")
    for i in range(num_sample):
        samples_idx.append(labels.index(i))
   
    
    print("\n###########################################\n")
    

    new_sample_idx =[]
    for i in range(len(samples_idx)):
        for d in range(domains_per_shape):
            new_sample_idx.append((samples_idx[i]*domains_per_shape)+d)

    return new_sample_idx

def get_optimize_input(distance_transform_files, mesh_mode=False):
    if mesh_mode:
        dt_dir = os.path.dirname(distance_transform_files[0])
        mesh_dir = dt_dir.replace("distance_transforms", "meshes")
        if not os.path.exists(mesh_dir):
            os.makedirs(mesh_dir)
        domain_type = 'mesh'
        files = []
        for file in distance_transform_files:
            mesh_file = file.replace(dt_dir, mesh_dir).replace(".nrrd", ".vtk")
            print("Writing: " + mesh_file)
            sw.Image(file).toMesh(0).write(mesh_file)
            files.append(mesh_file)
    else:
        domain_type = 'image'
        files = distance_transform_files
    return domain_type, files
