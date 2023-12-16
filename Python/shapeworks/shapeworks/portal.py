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
    """Get ShapeWorks Cloud login details from user"""
    print("\nPlease enter your ShapeWorks Cloud login:\n")
    print("New ShapeWorks Cloud user please register an account:\n")
    print("   https://www.shapeworks-cloud.org\n")
    username = input("Username: ")
    password = getpass.getpass("Password: ")
    try:
        with swcc_session() as session:
            token = session.login(username, password)
        combined = username + ':' + password
        PasswordHash = base64.b64encode(password.encode()).decode("ascii")
        loginState = {'username': username, 'PasswordHash': PasswordHash}
        saveLogin(loginState)

    except Exception as e:
        print('Incorrect username or password.')
        quit()


def login():
    """Login to the ShapeWorks Cloud Data Portal"""
    printDataPortalWelcome()
    if loadLogin() is None:
        getLoginDetails()
    print("Login to ShapeWorks Data Portal successful")
    loginDetails = loadLogin()
    username = loginDetails['username']
    passwordHash = loginDetails['PasswordHash']
    password = base64.b64decode(passwordHash)
    return username, password


def download_dataset(datasetName, outputDirectory, force = False):
    """Download a dataset from the ShapeWorks Cloud Data Portal"""
    # check if dataset is already downloaded
    check_file = "Output/" + datasetName + ".downloaded"
    if (os.path.exists(check_file) and not force):
        print(f"Dataset {datasetName} already downloaded ({check_file} exists)")
        # check if env var SW_PORTAL_DOWNLOAD_ONLY is set to 1
        if 'SW_PORTAL_DOWNLOAD_ONLY' in os.environ and os.environ['SW_PORTAL_DOWNLOAD_ONLY'] == '1':
            quit()
        return
    username, password = login()
    # api as a context manager
    with swcc_session() as session:
        token = session.login(username, password)
        session = swcc_session(token=token).__enter__()
        # Download a full dataset in bulk
        print(f"Downloading files to {outputDirectory + datasetName}")
        dataset = Dataset.from_name(datasetName)
        download_path = Path(outputDirectory)
        for project in dataset.projects:
            project.download(Path(download_path))
            break

    # Mark as completed
    Path(check_file).touch()
    print(f"Dataset {datasetName} downloaded to {outputDirectory}")
    
    if 'SW_PORTAL_DOWNLOAD_ONLY' in os.environ:
        quit()
