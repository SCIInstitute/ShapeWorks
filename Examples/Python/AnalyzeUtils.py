import numpy as np
import io
import glob
import os
import subprocess
import shutil
import xml.etree.ElementTree as ET
from termcolor import colored, cprint
import OptimizeUtils
import shapeworks as sw


def create_analyze_xml(xmlfilename, dtFiles, localPointFiles, worldPointFiles, domains_per_shape=1):
    worldPointFiles = sorted(worldPointFiles)
    dtFiles = sorted(dtFiles)
    localPointFiles = sorted(localPointFiles)

    root = ET.Element('sample')

    domains_per_shape_elem = ET.SubElement(root, 'domains_per_shape')
    domains_per_shape_elem.text = "\n" + str(domains_per_shape) + "\n"

    input_points = ET.SubElement(root, 'point_files')
    input_points.text = "\n"
    for i in range(len(worldPointFiles)):
        t1 = input_points.text
        t1 = t1 + worldPointFiles[i] + '\n'
        input_points.text = t1

    input_points = ET.SubElement(root, 'world_point_files')
    input_points.text = "\n"
    for i in range(len(worldPointFiles)):
        t1 = input_points.text
        t1 = t1 + worldPointFiles[i] + '\n'
        input_points.text = t1

    distance_transform = ET.SubElement(root, 'distance_transform_files')
    distance_transform.text = "\n"
    for i in range(len(dtFiles)):
        t1 = distance_transform.text
        t1 = t1 + dtFiles[i] + '\n'
        distance_transform.text = t1

    local_point = ET.SubElement(root, 'local_point_files')
    local_point.text = "\n"
    for i in range(len(localPointFiles)):
        t1 = local_point.text
        t1 = t1 + localPointFiles[i] + '\n'
        local_point.text = t1

    data = ET.tostring(root, encoding='unicode')
    file = open(xmlfilename, "w+")
    file.write(data)
    file.close()
    OptimizeUtils.create_cpp_xml(xmlfilename, xmlfilename)


def launch_shapeworks_studio(filename):
    command = ["ShapeWorksStudio", filename]
    subprocess.check_call(command)
    print("\n\nTo re-run ShapeWorksStudio, run:\n")
    print(f" cd {os.getcwd()}")
    print(f" {' '.join(command)}\n\n")


def verify(args, world_point_files):
    # compare against baseline particles
    files = world_point_files
    ps1 = sw.ParticleSystem(files)

    print(f"Comparing results in:")
    for file in files:
        print(file)

    # Copy to results dir
    check_dir = f"Output/Results/{args.use_case.lower()}/{args.option_set}/"
    if not os.path.exists(check_dir):
        os.makedirs(check_dir)

    for file in files:
        shutil.copy(file, check_dir)

    verification_dir = f"Data/Verification/{args.use_case.lower()}/{args.option_set}/"

    baseline = [verification_dir + os.path.basename(f) for f in files]

    print(f"\nBaseline:")
    for file in baseline:
        print(file)

    for file in baseline:
        if not os.path.exists(file):
            print(f"Error: baseline file {file} does not exist")
            return False

    ps2 = sw.ParticleSystem(baseline)

    if not ps1.EvaluationCompare(ps2):
        print("Error: particle system did not match ground truth")
        return False
    return True


def check_results(args, world_point_files):
    # If tiny test or verify, check results and exit
    if args.tiny_test or args.verify:
        print("Verifying shape model")
        if not verify(args, world_point_files):
            exit(-1)
        print("Done with test, verification succeeded.")
        exit()
