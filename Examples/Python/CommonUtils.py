#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Sep 10 14:32:27 2019

@author: shireen
"""

import numpy as np
import itk
from sklearn.cluster import KMeans
import io
import glob
import os
import shutil
import xml.etree.ElementTree as ET
from termcolor import colored, cprint


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
    IMG = []
    DIM = []
    for i in range(len(inDataList)):
        tmp = itk.GetArrayFromImage(itk.imread(inDataList[i]))
        IMG.append(tmp)
        DIM.append(tmp.shape)

    ref_dim = np.max(DIM, axis=0)
    for i in range(len(inDataList)):
        IMG[i] = np.pad(IMG[i], ((0,ref_dim[0]-DIM[i][0]), (0,ref_dim[1]-DIM[i][1]), (0,ref_dim[2]-DIM[i][2])), mode ='constant' , constant_values = 0)
        IMG[i] = np.ndarray.flatten(IMG[i], 'C')
    IMG = np.asarray(IMG)
    model = KMeans(n_clusters=num_sample)
    clusters = model.fit_predict(IMG)
    labels = list(model.labels_)
    samples_idx = []
    for i in range(num_sample):
        samples_idx.append(labels.index(i))

    return samples_idx

