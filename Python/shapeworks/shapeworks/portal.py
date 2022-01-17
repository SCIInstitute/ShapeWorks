#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Common utility functions
"""
import os
import re
from shutil import rmtree
from pathlib import Path
import shapeworks as sw
import base64
import getpass
import json
import os
from swcc.api import swcc_session
from swcc.models import (Dataset, GroomedSegmentation, OptimizedParticles,OptimizedShapeModel, Project, Segmentation, Subject)
from itertools import islice
_LOGIN_FILE_NAME = 'shapeworksPortalLogin_new.txt'

def printDataPortalWelcome():
    print(' _____    ___      .     ')
    print('|     |  /   \    / \    ')
    print('| ShapeWorks Portal  \   ')
    print('|_____|  \___/  /_____\  ')
    print()

def saveLogin(loginState):
    with open(_LOGIN_FILE_NAME, 'w') as outfile:
        json.dump(loginState, outfile)

def loadLogin():
    if not os.path.exists(_LOGIN_FILE_NAME):
        return None
    with open(_LOGIN_FILE_NAME) as json_file:
        loginState = json.load(json_file)
        return loginState
    
def getLoginDetails():
    
    print('')
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
        print("Getting login")
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
            
def download_subset(use_case,datasetName,outputDirectory):
    import DatasetUtils
    import re
    username,password = login()
    # fileList = DatasetUtils.getFileList(datasetName)
    outputDirectory = outputDirectory #+ datasetName+"/"
    print('Downloading subset')
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
        if(use_case in ["femur_cut","left_atrium"]):
            if(generate_download_flag(outputDirectory_1+"/","images")):
                for image in islice(zip(dataset.images), 3):
                    image_dir = outputDirectory_1+ "/images"
                    image.file.download(outputDirectory)
        elif(use_case in ["ellipsoid_mesh","femur_cut","lumps","thin_cavity_bean"]):
            if(generate_download_flag(outputDirectory_1+"/","meshes")):
                for mesh in islice(zip(dataset.meshes), 3):
                    mesh_dir = outputDirectory_1+ "/meshes"
                    mesh[0].file.download(outputDirectory)

        
    
#         if(generate_download_flag(outputDirectory,"segmentations")):
#             segFilesList = sorted([files for files in fileList if re.search("^segmentations(?:/|\\\).*nrrd$",files)])[:3]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = segFilesList)
#     elif(use_case in ["ellipsoid_mesh","femur","femur_cut","lumps","thin_cavity_bean"]):
#         if(generate_download_flag(outputDirectory,"meshes")):
#             meshFilesList = sorted([files for files in fileList if re.search("^meshes(?:/|\\\).*ply$",files)])[:3]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = meshFilesList)
#     if(use_case in ["femur","femur_cut","left_atrium"]):
#         if(generate_download_flag(outputDirectory,"images")):
#             imageFilelist = sorted([files for files in fileList if re.search("^images(?:/|\\\).*nrrd$",files)])[:3]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = imageFilelist)
#     elif(use_case=="femur_mesh"):
#         if(generate_download_flag(outputDirectory,"groomed/meshes/")):
#             meshFilesList = sorted([files for files in fileList if re.search("^groomed(?:/|\\\)meshes(?:/|\\\).*ply$",files)])[:3]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = meshFilesList)
#     elif(use_case=="deep_ssm"):
#         if(generate_download_flag(outputDirectory,"groomed/images/")):
#             imageFilesList = sorted([files for files in fileList if re.search("^groomed(?:/|\\\)images(?:/|\\\).*nrrd$",files)])[:7]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = imageFilesList)
#         if(generate_download_flag(outputDirectory,"groomed/distance_transforms/")):
#             dtFilesList = sorted([files for files in fileList if re.search("^groomed(?:/|\\\)distance_transforms(?:/|\\\).*nrrd$",files)])[:7]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = dtFilesList)
#         if(generate_download_flag(outputDirectory,"shape_models/femur/1024/")):
#             wolrdFilesList = sorted([files for files in fileList if re.search("^shape_models(?:/|\\\)femur(?:/|\\\)1024(?:/|\\\).*world.particles$",files)])[:7]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = wolrdFilesList)
#             localFilesList = sorted([files for files in fileList if re.search("^shape_models(?:/|\\\)femur(?:/|\\\)1024(?:/|\\\).*local.particles$",files)])[:7]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = localFilesList)
#         if(generate_download_flag(outputDirectory,"shape_models/femur/mean/")):
#             meanFilesList = sorted([files for files in fileList if re.search("^shape_models(?:/|\\\)femur(?:/|\\\)mean(?:/|\\\).*",files)])
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = meanFilesList)
#     elif(use_case=="ellipsoid_multiple_domain"):
#         if(generate_download_flag(outputDirectory,"segmentations")):
#             segFilesList = sorted([files for files in fileList if re.search("^segmentations(?:/|\\\).*nrrd$",files)])[:6]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = segFilesList)
#     elif(use_case=="ellipsoid_multiple_domain_mesh"):
#         if(generate_download_flag(outputDirectory,"meshes")):
#             meshFilesList = sorted([files for files in fileList if re.search("^meshes(?:/|\\\).*vtk$",files)])[:6]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = meshFilesList)
#     elif(use_case=="supershapes_1mode_contour"):
#         if(generate_download_flag(outputDirectory,"contours")):
#             contourFilesList = sorted([files for files in fileList if re.search("^contours(?:/|\\\).*vtp$",files)])[:3]
#             DatasetUtils.downloadDataset(datasetName,destinationPath=outputDirectory,fileList = contourFilesList)


def download_and_unzip_dataset(datasetName, outputDirectory):

    username,password = login()

    #api as a context manager
    with swcc_session()  as session:
        
        token = session.login(username, password)
        session = swcc_session(token=token).__enter__()
        # Check if the unzipped data is present and number of files are more than 3 for full use case
        if generate_download_flag(outputDirectory,datasetName):
            # Download a full dataset in bulk
            dataset = Dataset.from_name(datasetName)
            print(outputDirectory+datasetName+"/")
            download_path = Path(outputDirectory)
            if not download_path.exists():
                rmtree(str(download_path))
                
            dataset.download(download_path)



   







