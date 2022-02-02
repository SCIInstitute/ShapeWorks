import numpy as np
import io
from termcolor import colored, cprint
import glob
import os
import subprocess
import shutil
import xml.etree.ElementTree as ET

def get_parameter_text(parameterKey,parameterValue,domains_per_shape):
    if(type(parameterValue) is not list):
        parameterValue  = list([parameterValue])
    assert(len(parameterValue)==domains_per_shape),str(parameterKey)+" parameter should be an array of length = domains_per_shape= " + str(domains_per_shape)
    text = "\n"
    for i in range(domains_per_shape):
        text = text + str(parameterValue[i]) + "\n"
    return text
     
def get_attribute_scales_text(use_normals,normal_weight,domains_per_shape):

    text = "\n"
    if(type(use_normals) is not list):
        use_normals = list([use_normals])
    if(type(normal_weight) is not list):
        normal_weight = list([normal_weight])
    for i in range(domains_per_shape):

        if(use_normals[i]==0):
            text = text + "1.0 \n 1.0 \n 1.0\n"
        else:
            text = text + "1.0 \n 1.0 \n 1.0 \n "+ str(normal_weight[i])+ "\n "  + str(normal_weight[i])+ "\n " + str(normal_weight[i])+ "\n"
    return text


def create_SWRun_xml(xmlfilename, inDataFiles, parameterDictionary, outDir):
    root = ET.Element('sample')
    output_dir = ET.SubElement(root, 'output_dir')
    output_dir.text = "\n" + outDir + "\n"
    number_of_particles = ET.SubElement(root, 'number_of_particles')
    number_of_particles.text = get_parameter_text('number_of_particles',parameterDictionary['number_of_particles'],parameterDictionary['domains_per_shape'])
    use_normals = ET.SubElement(root, 'use_normals')
    use_normals.text = get_parameter_text('use_normals',parameterDictionary['use_normals'],parameterDictionary['domains_per_shape'])
    normal_weight = parameterDictionary['normal_weight']
    attribute_scales = ET.SubElement(root, 'attribute_scales')
    attribute_scales.text = get_attribute_scales_text(parameterDictionary['use_normals'],parameterDictionary['normal_weight'],parameterDictionary['domains_per_shape'])
    checkpointing_interval = ET.SubElement(root, 'checkpointing_interval')
    checkpointing_interval.text = "\n" + str(parameterDictionary['checkpointing_interval']) + "\n"
    keep_checkpoints = ET.SubElement(root, 'keep_checkpoints')
    keep_checkpoints.text = "\n" + str(parameterDictionary['keep_checkpoints']) + "\n"
    iterations_per_split = ET.SubElement(root, 'iterations_per_split')
    iterations_per_split.text = "\n" + str(parameterDictionary['iterations_per_split']) + "\n"
    optimization_iterations = ET.SubElement(root, 'optimization_iterations')
    optimization_iterations.text = "\n" + str(parameterDictionary['optimization_iterations']) + "\n"
    starting_regularization = ET.SubElement(root, 'starting_regularization')
    starting_regularization.text = "\n" + str(parameterDictionary['starting_regularization']) + "\n"
    ending_regularization = ET.SubElement(root, 'ending_regularization')
    ending_regularization.text = "\n" + str(parameterDictionary['ending_regularization']) + "\n"
    recompute_regularization_interval = ET.SubElement(root, 'recompute_regularization_interval')
    recompute_regularization_interval.text = "\n" + str(parameterDictionary['recompute_regularization_interval']) + "\n"
    domains_per_shape = ET.SubElement(root, 'domains_per_shape')
    domains_per_shape.text = "\n" + str(parameterDictionary['domains_per_shape']) + "\n"
    domain_type = ET.SubElement(root, 'domain_type')
    domain_type.text = "\n" + str(parameterDictionary['domain_type']) + "\n"
    relative_weighting = ET.SubElement(root, 'relative_weighting')
    relative_weighting.text = "\n" + str(parameterDictionary['relative_weighting']) + "\n"
    initial_relative_weighting = ET.SubElement(root, 'initial_relative_weighting')
    initial_relative_weighting.text = "\n" + str(parameterDictionary['initial_relative_weighting']) + "\n"
    procrustes_interval = ET.SubElement(root, 'procrustes_interval')
    procrustes_interval.text = "\n" + str(parameterDictionary['procrustes_interval']) + "\n"
    procrustes_scaling = ET.SubElement(root, 'procrustes_scaling')
    procrustes_scaling.text = "\n" + str(parameterDictionary['procrustes_scaling']) + "\n"
    save_init_splits = ET.SubElement(root, 'save_init_splits')
    save_init_splits.text = "\n" + str(parameterDictionary['save_init_splits']) + "\n"
    mesh_based_attributes = ET.SubElement(root, 'mesh_based_attributes')
    mesh_based_attributes.text = "\n" + str(1) + "\n"
    if 'use_shape_statistics_after' in parameterDictionary:
        use_shape_statistics_after = ET.SubElement(root, 'use_shape_statistics_after')
        use_shape_statistics_after.text = "\n" + str(parameterDictionary['use_shape_statistics_after']) + "\n"
    verbosity = ET.SubElement(root, 'verbosity')
    verbosity.text = "\n" + str(parameterDictionary['verbosity']) + "\n"
    use_xyz = ET.SubElement(root, 'use_xyz')
    parameterValue = [1 for _ in range(parameterDictionary['domains_per_shape'])]
    use_xyz.text = get_parameter_text('use_xyz',parameterValue,parameterDictionary['domains_per_shape'])
    if 'visualizer_enable' in parameterDictionary:
        visualizer_enable = ET.SubElement(root, 'visualizer_enable')
        visualizer_enable.text = "\n" + str(parameterDictionary['visualizer_enable']) + "\n"
    if 'visualizer_wireframe' in parameterDictionary:
        visualizer_wireframe = ET.SubElement(root, 'visualizer_wireframe')
        visualizer_wireframe.text = "\n" + str(parameterDictionary['visualizer_wireframe']) + "\n"
    if 'visualizer_screenshot_directory' in parameterDictionary:
        if not os.path.exists(parameterDictionary['visualizer_screenshot_directory']):
            os.makedirs(parameterDictionary['visualizer_screenshot_directory'])
        visualizer_screenshot_directory = ET.SubElement(root, 'visualizer_screenshot_directory')
        visualizer_screenshot_directory.text = "\n" + str(parameterDictionary['visualizer_screenshot_directory']) + "\n"
    if 'adaptivity_mode' in parameterDictionary:
        adaptivity_mode = ET.SubElement(root, 'adaptivity_mode')
        adaptivity_mode.text = "\n" + str(parameterDictionary['adaptivity_mode']) + "\n"
    inputs = ET.SubElement(root, 'inputs')
    inputs.text = "\n"
    for filename in inDataFiles:
        inputs.text = inputs.text + filename.replace('\\','/') + "\n"
    if 'cutting_plane_counts' in parameterDictionary and 'cutting_planes' in parameterDictionary:
        cutting_plane_counts = ET.SubElement(root, 'num_planes_per_input')
        cutting_plane_counts_var = parameterDictionary['cutting_plane_counts']
        cutting_planes = ET.SubElement(root, 'cutting_planes')
        cutting_planes_var = parameterDictionary['cutting_planes']

        cutting_plane_counts_text = "\n"
        cutting_planes_text = "\n"
        for i in range(len(cutting_planes_var)):
            cur_plane = cutting_planes_var[i]
            cutting_planes_text += "  " + str(cur_plane[0][0]) + " " + str(cur_plane[0][1]) + " " + str(cur_plane[0][2]) + " " + str(cur_plane[1][0]) + " " + str(cur_plane[1][1]) + " " + str(cur_plane[1][2]) + " " + str(cur_plane[2][0]) + " " + str(cur_plane[2][1]) + " " + str(cur_plane[2][2]) + "\n"
        for i in range(len(cutting_plane_counts_var)):
            cutting_plane_counts_text += "  " + str(cutting_plane_counts_var[i]) + "\n"
        cutting_plane_counts.text = cutting_plane_counts_text
        cutting_planes.text = cutting_planes_text
    if 'geodesics_enabled' in parameterDictionary:
        geodesics_enabled = ET.SubElement(root, 'geodesics_enabled')
        geodesics_enabled.text = "\n" + str(parameterDictionary['geodesics_enabled']) + "\n"
    if 'geodesics_cache_size_multiplier' in parameterDictionary:
        geodesics_cache_size_multiplier = ET.SubElement(root, 'geodesics_cache_size_multiplier')
        geodesics_cache_size_multiplier.text = "\n" + str(parameterDictionary['geodesics_cache_size_multiplier']) + "\n"

    data = ET.tostring(root, encoding='unicode')
    file = open(xmlfilename, "w+")
    file.write(data)

def create_SWRun_fixed_domains(xmlfilename, inDataFiles, parameterDictionary, outDir, numFixedDomains, fixedPointFiles):
    root = ET.Element('sample')
    output_dir = ET.SubElement(root, 'output_dir')
    output_dir.text = "\n" + outDir + "\n"
    number_of_particles = ET.SubElement(root, 'number_of_particles')
    number_of_particles.text = "\n" + str(parameterDictionary['number_of_particles']) + "\n"
    use_normals = ET.SubElement(root, 'use_normals')
    use_normals.text = "\n" + str(parameterDictionary['use_normals']) + "\n"
    normal_weight = parameterDictionary['normal_weight']
    if parameterDictionary['use_normals'] == 0:
        attribute_scales = ET.SubElement(root, 'attribute_scales')
        attribute_scales.text = "\n 1.0 \n 1.0 \n 1.0\n"
    else:
        attribute_scales = ET.SubElement(root, 'attribute_scales')
        attribute_scales.text = "\n 1.0 \n 1.0 \n 1.0 \n "+ str(normal_weight)+ "\n "  + str(normal_weight)+ "\n " + str(normal_weight)+ "\n"
    checkpointing_interval = ET.SubElement(root, 'checkpointing_interval')
    checkpointing_interval.text = "\n" + str(parameterDictionary['checkpointing_interval']) + "\n"
    keep_checkpoints = ET.SubElement(root, 'keep_checkpoints')
    keep_checkpoints.text = "\n" + str(parameterDictionary['keep_checkpoints']) + "\n"
    iterations_per_split = ET.SubElement(root, 'iterations_per_split')
    iterations_per_split.text = "\n" + str(parameterDictionary['iterations_per_split']) + "\n"
    optimization_iterations = ET.SubElement(root, 'optimization_iterations')
    optimization_iterations.text = "\n" + str(parameterDictionary['optimization_iterations']) + "\n"
    starting_regularization = ET.SubElement(root, 'starting_regularization')
    starting_regularization.text = "\n" + str(parameterDictionary['starting_regularization']) + "\n"
    ending_regularization = ET.SubElement(root, 'ending_regularization')
    ending_regularization.text = "\n" + str(parameterDictionary['ending_regularization']) + "\n"
    recompute_regularization_interval = ET.SubElement(root, 'recompute_regularization_interval')
    recompute_regularization_interval.text = "\n" + str(parameterDictionary['recompute_regularization_interval']) + "\n"
    domains_per_shape = ET.SubElement(root, 'domains_per_shape')
    domains_per_shape.text = "\n" + str(parameterDictionary['domains_per_shape']) + "\n"
    domain_type = ET.SubElement(root, 'domain_type')
    domain_type.text = "\n" + str(parameterDictionary['domain_type']) + "\n"
    relative_weighting = ET.SubElement(root, 'relative_weighting')
    relative_weighting.text = "\n" + str(parameterDictionary['relative_weighting']) + "\n"
    initial_relative_weighting = ET.SubElement(root, 'initial_relative_weighting')
    initial_relative_weighting.text = "\n" + str(parameterDictionary['initial_relative_weighting']) + "\n"
    procrustes_interval = ET.SubElement(root, 'procrustes_interval')
    procrustes_interval.text = "\n" + str(parameterDictionary['procrustes_interval']) + "\n"
    procrustes_scaling = ET.SubElement(root, 'procrustes_scaling')
    procrustes_scaling.text = "\n" + str(parameterDictionary['procrustes_scaling']) + "\n"
    save_init_splits = ET.SubElement(root, 'save_init_splits')
    save_init_splits.text = "\n" + str(parameterDictionary['save_init_splits']) + "\n"
    mesh_based_attributes = ET.SubElement(root, 'mesh_based_attributes')
    mesh_based_attributes.text = "\n" + str(1) + "\n"
    verbosity = ET.SubElement(root, 'verbosity')
    verbosity.text = "\n" + str(parameterDictionary['verbosity']) + "\n"
    use_xyz = ET.SubElement(root, 'use_xyz')
    use_xyz.text = "\n" + str(1) + "\n"
    init_stats = ET.SubElement(root, 'use_shape_statistics_in_init')
    init_stats.text = "\n" + str(1.0) + "\n"

    inputs = ET.SubElement(root, 'inputs')
    inputs.text = "\n"

    if 'geodesics_enabled' in parameterDictionary:
        geodesics_enabled = ET.SubElement(root, 'geodesics_enabled')
        geodesics_enabled.text = "\n" + str(parameterDictionary['geodesics_enabled']) + "\n"
    if 'geodesics_cache_size_multiplier' in parameterDictionary:
        geodesics_cache_size_multiplier = ET.SubElement(root, 'geodesics_cache_size_multiplier')
        geodesics_cache_size_multiplier.text = "\n" + str(parameterDictionary['geodesics_cache_size_multiplier']) + "\n"

    for filename in inDataFiles:
        inputs.text = inputs.text + filename.replace('\\','/') + "\n"

    # add in the pointfiles

    points = ET.SubElement(root, 'point_files')
    points.text = "\n"
    for i in range(len(inDataFiles)):
            t1 = points.text
            t1 = t1 + fixedPointFiles[i].replace('\\','/') + '\n'
            points.text = t1

    # add in the fixed domains
    fd = ET.SubElement(root, 'fixed_domains')
    fd.text = '\n'
    for i in range(numFixedDomains):
        t1 = fd.text
        t1 = t1 + str(i) + '\n'
        fd.text = t1

    data = ET.tostring(root, encoding='unicode')
    file = open(xmlfilename, "w+")
    file.write(data)

def runShapeWorksOptimize(parentDir, inDataFiles, parameterDictionary):
    outDir = parentDir
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    parameterFile = parentDir + "/correspondence.xml"
    create_SWRun_xml(parameterFile, inDataFiles, parameterDictionary, outDir)
    create_cpp_xml(parameterFile, parameterFile)
    print(parameterFile)
    execCommand = ["shapeworks", "optimize", "--name=" + parameterFile]
    subprocess.check_call(execCommand )

    outPointsLocal, outPointsWorld = _convertFilenamesToPointFilenames(inDataFiles, outDir)
    return [outPointsLocal, outPointsWorld]

def _convertFilenamesToPointFilenames(files, outDir):
    outPointsWorld = []
    outPointsLocal = []
    for i in range(len(files)):
        inname = files[i].replace('\\','/')
        inpath = os.path.dirname(files[i])
        outname = inname.replace(inpath, outDir)
        filename, fileextension = os.path.splitext(outname)
        wrdname = filename + '_world.particles'
        lclname = filename + '_local.particles'
        outPointsWorld.append(wrdname)
        outPointsLocal.append(lclname)
    return [outPointsLocal, outPointsWorld]


def findMeanShape(shapeModelDir):
    fileList = sorted(glob.glob(shapeModelDir + '/*local.particles'))
    for i in range(len(fileList)):
        if i == 0:
            meanShape = np.loadtxt(fileList[i])
        else:
            meanShape += np.loadtxt(fileList[i])
    meanShape = meanShape / len(fileList)
    nmMS = shapeModelDir + '/meanshape_local.particles'
    np.savetxt(nmMS, meanShape)



def runShapeWorksOptimize_FixedDomains(parentDir, inDataFiles, parameterDictionary):
    # outDir, numFixedDomains, fixedPointFiles
    numP = parameterDictionary['number_of_particles']
    numFD = parameterDictionary['number_fixed_domains']
    shapeModelDir = parameterDictionary['fixed_domain_model_dir']
    meanShapePath = parameterDictionary['mean_shape_path']
    outDir = parentDir
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    inparts = []
    for j in range(len(inDataFiles)):
        if j < numFD:
            inname = inDataFiles[j].replace('\\','/')
            inpath = os.path.dirname(inDataFiles[j]) + '/'
            outname = inname.replace(inpath, shapeModelDir)
            lclname = outname.replace('.nrrd', '_local.particles')
            inparts.append(lclname)
        else:
            # check the validity of this file existing everytime
            lclname = meanShapePath
            inparts.append(lclname)

    parameterFile = parentDir + "/correspondence_" + str(numP) + ".xml"
    create_SWRun_fixed_domains(parameterFile, inDataFiles, parameterDictionary, outDir, numFD, inparts)
    create_cpp_xml(parameterFile, parameterFile)
    print(parameterFile)
    execCommand = ["shapeworks", "optimize", "--name=" + parameterFile]
    subprocess.check_call(execCommand )
    outPointsLocal, outPointsWorld = _convertFilenamesToPointFilenames(inDataFiles, outDir)
    return [outPointsLocal, outPointsWorld]

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
