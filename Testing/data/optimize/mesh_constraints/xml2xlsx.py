import numpy as np
import pandas as pd

import re

def remove_empty(test_list):
    while("" in test_list) :
        test_list.remove("")
    return test_list

# Parses the disgusting ShapeWorks XML files
def parse_SWs_xml(fn):

    dict = {}

    f = open(fn)

    reading = False
    curr_param = ""
    curr_values = []

    lines = f.readlines()

    for line in lines:
        line = line.rstrip('\n')
        if(line == "<?xml version=\"1.0\" ?>"):
            continue
        # If not multiline parameter
        if(reading == False):
            if(line.startswith('<') and line.endswith('>')):
                splitline = re.split('<|>', line)
                splitline = remove_empty(splitline)
                curr_param = splitline[0]
                if(len(splitline) >= 3):
                    # Go through all values
                    for i in range(1, len(splitline)-1):
                        splitline[i] = splitline[i].rstrip(' ').lstrip(' ')
                        if(splitline[i].replace('-','',1).replace('.','',1).isdigit()):
                            curr_values.append(float(splitline[i]))
                        else:
                            curr_values.append(splitline[i])
                    # Write values to dict
                    if(len(curr_values) > 1):
                        dict[curr_param] = curr_values
                    elif(len(curr_values) > 0):
                        dict[curr_param] = curr_values[0]
                    reading = False
                    curr_values = []
                else:
                    reading = True
                    curr_values = []
        else: # Else multiline parameter
            if(line.startswith('</') and line.endswith('>')): # If closing xml tag
                # Write values to dict
                if(len(curr_values) > 1):
                    dict[curr_param] = curr_values
                else:
                    dict[curr_param] = curr_values[0]
                reading = False
                curr_values = []
            elif(line.endswith('>')): # Elif some data and closing xml tag
                # Check for end of format: data point
                splitline = re.split('<|>', line)
                splitline = remove_empty(splitline)
                if(len(splitline) >= 2):
                    splitline[1] = splitline[1].rstrip(' ').lstrip(' ')
                    if(splitline[1].replace('-','',1).replace('.','',1).isdigit()):
                        curr_values.append(float(splitline[1]))
                    else:
                        curr_values.append(splitline[1])
                # Add to dict
                if(len(curr_values) > 1):
                    dict[curr_param] = curr_values
                elif(len(curr_values) > 0):
                    dict[curr_param] = curr_values[0]
                reading = False
                curr_values = []
            else: # Else dataline without xml tags
                splitline = line.split(' ')
                splitline = remove_empty(splitline)
                if(len(splitline) > 1):
                    line_vals = []
                    for i in range(len(splitline)):
                        splitline[i] = splitline[i].rstrip(' ').lstrip(' ')
                        if(splitline[i]):
                            if(splitline[i].replace('-','',1).replace('.','',1).isdigit()):
                                line_vals.append(float(splitline[i]))
                            else:
                                line_vals.append(splitline[i])
                    curr_values.append(line_vals)
                elif(len(splitline) > 0):
                    if(splitline[0].replace('-','',1).replace('.','',1).isdigit()):
                        curr_values.append(float(splitline[0]))
                    else:
                        curr_values.append(splitline[0])
    f.close()
    return dict

def read_constraint_txt(fn):
    f = open(fn)

    lines = f.readlines()

    query_mode = False
    boundary_mode = False

    query = []
    boundary = []

    for line in lines:
        line = line.rstrip('\n')
        if(line == "query"):
            query_mode = True
            boundary_mode = False
        elif(line == "boundary_pts"):
            boundary_mode = True
            query_mode = False
        else:
            splitline = line.split(' ')
            splitline = remove_empty(splitline)
            line_vals = []
            for i in range(len(splitline)):
                splitline[i] = splitline[i].rstrip(' ').lstrip(' ')
                if(splitline[i]):
                    if(splitline[i].replace('-','',1).replace('.','',1).isdigit()):
                        line_vals.append(float(splitline[i]))
                    else:
                        line_vals.append(splitline[i])
            if(query_mode):
                query.append(line_vals)
            elif(boundary_mode):
                boundary.append(line_vals)

    f.close()

    return [query, boundary]

def write_constraints(dict, outdir):

    print(dict)

    for i in range(len(dict["inputs"])):
        if('ffcs' in dict or 'cuttin_planes' in dict):
            f = open(outdir + "constraint_"+str(i)+".json", 'w')

            f.write('{\n')

            if('ffcs' in dict):
                print("Processing FFCs")

                content = read_constraint_txt(dict['ffcs'][i])

                f.write("\t\"free_form_constraints\": {\n")
                f.write("\t\t\"boundaries\": [\n")
                f.write("\t\t\t{\n")
                f.write("\t\t\t\t\"points\": [\n")

                for i in range(len(content[1])):
                    point = content[1][i]
                    f.write("\t\t\t\t\t[\n")
                    f.write("\t\t\t\t\t\t" + str(point[0]) + ',\n')
                    f.write("\t\t\t\t\t\t" + str(point[1]) + ',\n')
                    f.write("\t\t\t\t\t\t" + str(point[2]) + '\n')
                    if(i != len(content[1])-1):
                        f.write("\t\t\t\t\t],\n")
                    else:
                        f.write("\t\t\t\t\t]\n")

                f.write("\t\t\t\t]\n")
                f.write("\t\t\t}\n")
                f.write("\t\t],\n")

                f.write("\t\t\"query_point\": [\n")
                f.write("\t\t\t" + str(content[0][0][0]) + ',\n')
                f.write("\t\t\t" + str(content[0][0][1]) + ',\n')
                f.write("\t\t\t" + str(content[0][0][2]) + '\n')
                f.write("\t\t]\n")

                if(not 'cuttin_planes' in dict):
                    f.write("\t}\n")


            if('cuttin_planes' in dict):
                print("Processing cutting planes")
                if('ffcs' in dict):
                    f.write("\t},\n")

                f.write("\t\"planes\": [\n")
                f.write("\t\t{\n")

                for i in range(len(dict['cutting_planes'])):
                    cp = dict['cutting_planes'][i]
                    f.write("\t\t\t\"points\": [\n")
                    f.write("\t\t\t\t[\n")
                    f.write("\t\t\t\t\t" + str(cp[0]) +",\n")
                    f.write("\t\t\t\t\t" + str(cp[1]) +",\n")
                    f.write("\t\t\t\t\t" + str(cp[2]) +"\n")
                    f.write("\t\t\t\t],\n")
                    f.write("\t\t\t\t[\n")
                    f.write("\t\t\t\t\t" + str(cp[3]) +",\n")
                    f.write("\t\t\t\t\t" + str(cp[4]) +",\n")
                    f.write("\t\t\t\t\t" + str(cp[5]) +"\n")
                    f.write("\t\t\t\t],\n")
                    f.write("\t\t\t\t[\n")
                    f.write("\t\t\t\t\t" + str(cp[6]) +",\n")
                    f.write("\t\t\t\t\t" + str(cp[7]) +",\n")
                    f.write("\t\t\t\t\t" + str(cp[8]) +"\n")
                    f.write("\t\t\t\t]\n")
                    if(not i == len(dict['cutting_planes'])-1):
                        f.write("\t\t\t],\n")
                    else:
                        f.write("\t\t\t]\n")

            f.write("}")

            f.close()

def write_xlsx(dict, outdir):

    # Data page
    data = pd.DataFrame({'shape_1': dict["inputs"]})
    constraint_files = ["constraint_" + str(x) + ".json" for x in range(len(dict["inputs"]))]
    constraints = pd.DataFrame({'constraints_1': constraint_files})

    # Optimize page
    keys = []
    vals = []
    for key in dict:
        if(type(dict[key]) != list):
            keys.append(key)
            vals.append(dict[key])

    ke = pd.DataFrame({'key': keys})
    va = pd.DataFrame({'value': vals})

    # Create a new excel workbook
    writer = pd.ExcelWriter(outdir+'optimize.xlsx', engine='xlsxwriter')

    # Write each dataframe to a different worksheet.
    data.to_excel(writer, sheet_name='data', index=False)
    constraints.to_excel(writer, sheet_name='data', index=False, startcol=1)

    ke.to_excel(writer, sheet_name='optimize', index=False)
    va.to_excel(writer, sheet_name='optimize', index=False, startcol=1)

    writer.save()

def main():
    xmlfile = 'mesh_constraints.xml'
    xmlfile = '../ffc/ffc.xml'
    #xmlfile = '../cutting_plane_multi/cutting_plane.xml'
    outdir = './'
    outdir = '../ffc/'

    shapeworks_sheets = ['data', 'groom', 'optimize', 'analysis', 'deepssm', 'studio', 'project', 'landmarks']

    dict = parse_SWs_xml(xmlfile) # Parses the disgusting ShapeWorks XML files

    write_constraints(dict, outdir)

    write_xlsx(dict, outdir)

main()
