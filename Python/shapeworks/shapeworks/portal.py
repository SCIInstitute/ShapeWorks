#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Common utility functions
"""
import os
import re
from shutil import rmtree
import filecmp
from pathlib import Path
import base64
import getpass
import json
import os
from swcc.api import swcc_session
from swcc.models import Dataset
from itertools import islice
_LOGIN_FILE_NAME = 'Output/shapeworksCloudLogin.txt'

def printDataPortalWelcome():
    print(' _____    ___      .     ')
    print('|     |  /   \    / \    ')
    print('|  ShapeWorks Cloud  \   ')
    print('|_____|  \___/  /_____\  ')
    print()

def saveLogin(loginState):
    with open(_LOGIN_FILE_NAME, 'w') as outfile:
        json.dump(loginState, outfile)

def loadLogin():
    if not os.path.exists(_LOGIN_FILE_NAME):
        if 'SW_CLOUD_LOGIN' in os.environ:
            print('Using cloud login from $SW_CLOUD_LOGIN')
            combined = os.environ['SW_CLOUD_LOGIN']
            username = combined.split(":")[0]
            password = combined.split(":")[1]
            PasswordHash = base64.b64encode(password.encode()).decode("ascii")
            loginState = {'username': username, 'PasswordHash': PasswordHash}
            saveLogin(loginState)
            return loginState
        else:
            return None
    with open(_LOGIN_FILE_NAME) as json_file:
        loginState = json.load(json_file)
        return loginState
    
def getLoginDetails():
    print("\nPlease enter your ShapeWorks Cloud login:\n")
    print("New ShapeWorks Cloud user please register an account:\n")
    print("   https://www.shapeworks-cloud.org\n")
    username = input("Username: ")
    password = getpass.getpass("Password: ")
    try:
        with swcc_session()  as session:
            token = session.login(username, password)
        combined = username + ':' + password
        PasswordHash = base64.b64encode(password.encode()).decode("ascii")
        loginState = {'username': username, 'PasswordHash': PasswordHash}
        saveLogin(loginState)

    except Exception as e:
        print('Incorrect username or password.')
        quit()
    


def login():
    printDataPortalWelcome()
    if loadLogin() is None:
        getLoginDetails()
    print("Login to ShapeWorks Data Portal successful")
    loginDetails = loadLogin()
    username = loginDetails['username']
    passwordHash = loginDetails['PasswordHash']
    password = base64.b64decode(passwordHash)
    return username,password


def dataset_exists_check(use_case):
    existsFlag = False
    OutputDirectory = "Output/"
    for filename in os.listdir(OutputDirectory):
        if(use_case == filename):
            existsFlag = True
    return existsFlag

def count_existing_files(directory):
    total = 0
    for root, _, files in os.walk(directory):
        if len(files):
            print(f"{len(files)} files found in {root}")
        # excluding file in particle folders and in root directory (swproj file)
        if not("particle" in root) and root != directory:
            total += len(files)
    print(f"\nTotal shape files found in {directory}: {total}\n")
    return total

def generate_download_flag(outputDirectory, folder):
    download_flag = False
    #if output/dataset + subfolders exits 

    dataset = Dataset.from_name(folder)
    if os.path.exists(outputDirectory):
        # if the folder is empty or has less than 3 files then download
        file_count = count_existing_files(outputDirectory)
        if file_count < 6 and dataset:
            download_flag = True
        elif file_count == 6 and dataset and not("tiny_test" in folder):
            download_flag = True
        else:
            print("Data available in " + outputDirectory + "  is sufficient, no new data will be downloaded")
    #if the subfolder folder does not exists then download
    else:
        download_flag = True        
    return download_flag
            
def download_subset(use_case,datasetName,outputDirectory):
    username,password = login()
    print('Downloading subset')
    print(outputDirectory)
    with swcc_session()  as session:
        token = session.login(username, password)
        session = swcc_session(token=token).__enter__()
        dataset = Dataset.from_name(datasetName)
        outputDirectory_1 = outputDirectory + "/" + datasetName 
        if(use_case in ["ellipsoid","ellipsoid_cut","left_atrium"]):
            if(generate_download_flag(outputDirectory_1+"/","segmentations")):
                for segmentation, image in islice(zip(dataset.segmentations, dataset.images), 3):
                    seg_dir = outputDirectory_1+ "/segmentations"
                    segmentation.file.download(seg_dir)
        elif(use_case in ["femur_cut","left_atrium"]):
            if(generate_download_flag(outputDirectory_1+"/","images")):
                for image in islice(dataset.images, 3):
                    image_dir = outputDirectory_1+ "/images"
                    image.file.download(image_dir)
        elif(use_case in ["ellipsoid_mesh","femur_cut","lumps","thin_cavity_bean"]):
            print(outputDirectory_1 +"/meshes/")
            if(generate_download_flag(outputDirectory_1+"/","meshes")):
                for mesh in islice(dataset.images, 3):
                    print(mesh)
                    mesh_dir = outputDirectory_1+ "/meshes"
                    print(mesh_dir)
                    mesh.file.download(mesh_dir)

class dircmp(filecmp.dircmp):
    def phase3(self):
        fcomp = filecmp.cmpfiles(self.left, self.right, self.common_files,
                                 shallow=False)
        self.same_files, self.diff_files, self.funny_files = fcomp


def is_same(dir1, dir2):
    compared = dircmp(dir1, dir2)
    different = compared.left_only or compared.right_only or compared.diff_files or compared.funny_files
    if different:
        print(dir1, dir2, different)
        return False
    for subdir in compared.common_dirs:
        if not is_same(os.path.join(dir1, subdir), os.path.join(dir2, subdir)):
            return False
    return True

def download_and_unzip_dataset(datasetName, outputDirectory):
    username,password = login()
    #api as a context manager
    with swcc_session()  as session:
        token = session.login(username, password)
        session = swcc_session(token=token).__enter__()
        # Check if the unzipped data is present and number of files are more than 3 for full use case
        if generate_download_flag(outputDirectory,datasetName):
            # Download a full dataset in bulk
            print(f"Downloading files to {outputDirectory+datasetName}")
            dataset = Dataset.from_name(datasetName)
            download_path = Path(outputDirectory)
            if not download_path.exists():
                rmtree(str(download_path))
            for project in dataset.projects:
                project.download(Path(download_path))
                break

        print('Done. \n')

