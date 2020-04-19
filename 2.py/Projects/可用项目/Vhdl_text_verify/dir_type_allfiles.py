# -*- coding: utf-8 -*-
"""
Created on Thu Dec 28 12:15:45 2017

@author: xupengtao
"""

import os
import re

entity_pattern = re.compile(r'\s*entity\s*(\S*)\s*is', re.I)

def list_allfiles(path,files_path = None):
    if files_path == None:
        files_path = []
    dirlist = os.listdir(path)
    file_path = ''
    for path_tmp in dirlist:
        file_path = os.path.join(path,path_tmp)
        if os.path.isfile(file_path):
            files_path.append(file_path)
        elif os.path.isdir(file_path):
            list_allfiles(file_path,files_path)
    return files_path

def list_vhdfiles(path,files_path = None):
    if files_path == None:
        files_path = []
    dirlist = os.listdir(path)
    file_path = ''
    for path_tmp in dirlist:
        file_path = os.path.join(path,path_tmp)
        if os.path.isfile(file_path) & (file_path[-4:] == '.vhd'):
            files_path.append(file_path)
        elif os.path.isdir(file_path):
            list_vhdfiles(file_path,files_path)
    return files_path

def list_vfiles(path,files_path = None):
    if files_path == None:
        files_path = []
    dirlist = os.listdir(path)
    file_path = ''
    for path_tmp in dirlist:
        file_path = os.path.join(path,path_tmp)
        if os.path.isfile(file_path) & (file_path[-2:] == '.v'):
            files_path.append(file_path)
        elif os.path.isdir(file_path):
            list_vfiles(file_path,files_path)
    return files_path

def dict_vhdfiles(path,files_dict = None):
    if files_dict == None:
        files_dict = {}
    for file_path in list_vhdfiles(path):
        entity_name = ''
        with open(file_path,errors = 'ignore') as f_in:
            for line in f_in.readlines():
                match = entity_pattern.match(line)
                if match:
                    entity_name = match.group(1).strip()
                    if entity_name not in files_dict:
                        files_dict[entity_name] = file_path
                    elif isinstance(files_dict[entity_name],str):
                        print('entity name: \'%s\' has found in many files' % entity_name)
                        files_dict[entity_name] = [files_dict[entity_name],file_path]
                    else:
                        files_dict[entity_name].append(file_path)
                    break
    return files_dict