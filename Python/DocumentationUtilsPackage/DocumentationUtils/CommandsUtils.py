#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jun 19 12:55:11 2020

@author: shireen

utility functions to parse shapeworks comands help
"""


#%%
import subprocess
from mdutils.mdutils import MdUtils
from mdutils import Html

#%% misc util functions
def spacestr(nspaces):
    spstr = ''
    for n in range(nspaces):
        spstr += '&nbsp;'
    return spstr

def bytes2string(bstr):
    return str(bstr.decode("utf-8"))
  
def byteslist2string(bstrlist):
    strlist = list()
    for line in bstrlist:
        strlist.append(bytes2string(line))
    return strlist

def split_and_filter(line, spacedelim = ' '):
    line = line.split(spacedelim)
    line_out = []
    for piece in line:
        if piece == '':
            continue
        line_out.append(piece.strip())
    return line_out
                        
#%%
def getCommandHelp(cmd, verbose = False):
    out = subprocess.Popen(cmd + " --help", stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell = True)
    stdout,stderr = out.communicate()
    
    stdout        = stdout.splitlines()
    if stderr is not None:
        stderr        = stderr.splitlines()
    
    stdout = byteslist2string(stdout)
    if stderr is not None:
        stderr = byteslist2string(stderr)
    
    if verbose:
        print('----------- STDOUT -----------')
        print("\n".join(stdout))
        
        if stderr is not None:
            print('----------- STDERR -----------')
            print("\n".join(stderr))
        
    return stdout, stderr

#%%
# handle multiline description of subcommands and options
def getNameAndDescription(stdout, lid, spacedelim = ' '):
    nlines = len(stdout)
    line   = stdout[lid]
    line   = split_and_filter(line, spacedelim)
    
    name   = line[0]
    if len(line) < 2:
        desc = ''
    else:
        desc   = line[1]
        
    while True:
        lid += 1
        if lid >= nlines:
            break
        line = stdout[lid]
        if (len(line.strip()) > 0) and (line.strip()[0] == '-'):
            lid -= 1 # new subcommand/option
            break
        line = split_and_filter(line, spacedelim)    
        if len(line) == 1:
            desc = desc + ' ' + line[0]
        else:
            lid -= 1 # new subcommand/option
            break
    return name, desc, lid

#%%
def addUsage(mdFile, stdout, lid, cmd_prefix = ''):
    line = stdout[lid]
    line = line.split(':')
    
    mdFile.new_paragraph('**Usage:**')
    mdFile.insert_code(cmd_prefix + ' ' + line[1])
    mdFile.new_line('')
    
    lid += 1
    return lid

#%%
def addDescription(mdFile, stdout, lid):
    nlines = len(stdout)
    desc = stdout[lid]
    
    # handle multiline description
    lid += 1
    while True:
        if lid >= nlines:
            break
        line = stdout[lid]
        if line == '' :
            break
        desc = desc + ' ' + line
        lid += 1
    
    mdFile.new_paragraph('**Description:** ' + desc)
    mdFile.new_line('')
    
    lid += 1
    return lid
       
#%%
def addOptions(mdFile, stdout, lid, spacedelim = ' '): 
    nlines = len(stdout)
    mdFile.new_paragraph('**Options:**')
    lid += 1
    while True:
        if lid >= nlines:
            break
        line = stdout[lid]
        if line == '':
            lid += 1
            break
        
        optname, optdesc, lid = getNameAndDescription(stdout, lid, spacedelim = spacedelim)
        if optname.split()[0][0] is not '-':
            lid += 1
            break
        mdFile.new_paragraph("**" + optname + ":** " + optdesc)   
        lid += 1
    mdFile.new_line('')
    return lid

#%%       
def getSubCommands(stdout, lid, spacedelim = ' '):
    nlines = len(stdout)
    line = stdout[lid]
    line = line.split(':')
        
    subcommands = dict()
    lid += 1
    while True:
        if lid >= nlines:
            break
        line = stdout[lid]
        if (line == '') or (line[0] == '-'):
            lid += 1
            continue 
        if line[0] != ' ':
            cmdtype = line
            subcommands[cmdtype] = dict()
            lid += 1
        else:
            cmdname, desc, lid = getNameAndDescription(stdout, lid, spacedelim = spacedelim)
            subcommands[cmdtype][cmdname] = desc
            lid += 1
    return subcommands, lid

#%%
def addCommand(mdFile, cmd, level, spacedelim = ' ', verbose = True): # from Executable.cpp

    stdout, stderr = getCommandHelp(cmd, verbose = verbose )
    mdFile.new_header(level = level, title = cmd.split()[-1])
    
    nlines = len(stdout)
    lid    = 0
    while lid < nlines:
    
        line = stdout[lid]
        
        if stdout[lid] is '':
            lid += 1
            continue
        
        if ':' not in line: # description
            lid = addDescription(mdFile, stdout, lid)
            continue
        
        if 'Usage' in line:
            cmdsplit = cmd.split()[:-1]
            if len(cmdsplit) == 0:
                cmd_prefix = ''
            else:
                cmd_prefix = ' '.join(cmdsplit) # remove the last command as it is included in the usage
            lid = addUsage(mdFile, stdout, lid, cmd_prefix = cmd_prefix)
            continue
        
        if 'Options' in line:
            lid = addOptions(mdFile, stdout, lid, spacedelim = spacedelim)
            mdFile.new_line('<a href="#top">Back to Top</a>\n')
            continue
        
        if 'Available commands' in line:
            subcommands, lid = getSubCommands(stdout, lid, spacedelim = spacedelim)
            for cmdtype in subcommands.keys():
                mdFile.new_header(level=level, title= cmdtype + ' Commands')
                for cmdname in subcommands[cmdtype].keys():
                    addCommand(mdFile, cmd + " " + cmdname, level = level+1, spacedelim = spacedelim, verbose = verbose)
                    mdFile.new_line('[Back to ' + cmdtype + ' Commands' +'](#' + cmdtype.lower() + '-commands)')
        lid +=1
#%%