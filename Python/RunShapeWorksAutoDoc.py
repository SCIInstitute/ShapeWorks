#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jun 19 19:55:55 2020

@author: shireen
"""

import DocumentationUtils
import argparse

parser = argparse.ArgumentParser(description='ShapeWorks auto-documentation')
parser.add_argument("--md_filename", 
                    help="the markdown file name for the documentation file to be generated (default: ../docs/tools/ShapeWorksCommands.md )", 
                    nargs='?', type=str, default='../docs/tools/ShapeWorksCommands.md')

args = parser.parse_args()

DocumentationUtils.generateShapeWorksCommandDocumentation(mdFilename = args.md_filename)
