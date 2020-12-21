#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Sep 10 14:32:27 2019

@author: shireen
"""

import numpy as np
import sys
import itk
from sklearn.cluster import KMeans
from sklearn.cluster import SpectralClustering
import io
import glob
import os
import platform
import shutil
import xml.etree.ElementTree as ET
from termcolor import colored, cprint
from zipfile import ZipFile
import subprocess
import GroomUtils

def download_and_unzip_dataset(datasetName, outputDirectory):
    # Check if the unzipped data is present
    if not os.path.exists(outputDirectory + datasetName + '/'):
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

def create_cpp_xml(filename, outputfilename):
    '''
        This creates a xml for cpp Shape warp binary
    '''
    opening_tag = "<"
    ending_tag = "</"
    closing_tag = ">"
    newline = "\n"
    tree = ET.parse(str(filename))
    root = tree.getroot()
    children = {}
    for child in root:
        children[child.tag] = child.text
    tags = children.keys()
    xml_text = ""
    for tag in tags:
        xml_text += opening_tag+tag+closing_tag+children[tag]+ending_tag+tag+closing_tag+newline+newline
    file = open(outputfilename,"w")
    file.write(xml_text)
    file.close()

def sampledata(inDataList, num_sample):
    D = np.zeros((len(inDataList), len(inDataList)))
    for i in range(len(inDataList)):
        image_1 = itk.GetArrayFromImage(itk.imread(inDataList[i], itk.F))
        x, y, z = image_1.shape
        for j in range(i, len(inDataList)):
            image_2 = itk.GetArrayFromImage(itk.imread(inDataList[j], itk.F))
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
    print("########## Sample subset of data #########")
    print("Run Spectral Clustering for {} clusters ...".format(num_sample))
    model = SpectralClustering(n_clusters=num_sample,
                                    assign_labels="discretize",
                                    random_state=0, affinity='precomputed').fit(A)
    labels = list(model.labels_)
    samples_idx = []
    print("sample one data per cluster to have diverse samples!")
    for i in range(num_sample):
        samples_idx.append(labels.index(i))

    print("###########################################\n")
    return samples_idx

def samplemesh(inMeshList, num_sample, printCmd=False):
    D = np.zeros((len(inMeshList), len(inMeshList)))
    inMeshList = GroomUtils.getVTKmeshes(inMeshList, printCmd)
    for i in range(len(inMeshList)):
        for j in range(i, len(inMeshList)):
            execCommand = ["SurfaceToSurfaceDistance", "-a", inMeshList[i],
                "-b", inMeshList[j], "-p"]
            if printCmd:
                print("CMD: " + " ".join(execCommand))
            process = subprocess.Popen(execCommand, stdout=subprocess.PIPE)
            stdout = process.communicate()[0]
            dist = float(str(stdout).split()[1])
            D[i, j] = dist
    D += D.T
    A = np.exp(- D ** 2 / (2. * np.std(np.triu(D))**2))
    print("########## Sample subset of data #########")
    print("Run Spectral Clustering for {} clusters ...".format(num_sample))
    model = SpectralClustering(n_clusters=num_sample,
                                    assign_labels="discretize",
                                    random_state=0, affinity='precomputed').fit(A)
    labels = list(model.labels_)
    samples_idx = []
    print("sample one data per cluster to have diverse samples!")
    for i in range(num_sample):
        samples_idx.append(labels.index(i))
    print("###########################################\n")
    return samples_idx

# make sure the shapeworks executables can be found, adding path to osx studio app bundle if necessary
def robustifyShapeworksPaths():
    swpath = shutil.which("shapeworks")
    if (not swpath):
        print("Error: cannot find ShapeWorks executables. Please pass their location using the --shapeworks_path argument")
        sys.exit(1)

    # OSX: ensure ShapeWorksStudio is also in the path (if it exists)
    if platform.system() == "Darwin":
        if (not shutil.which("ShapeWorksStudio")):
            studiodir = os.path.dirname(swpath) + "/ShapeWorksStudio.app/Contents/MacOS"
            os.environ["PATH"] = studiodir + os.pathsep + os.environ["PATH"]
