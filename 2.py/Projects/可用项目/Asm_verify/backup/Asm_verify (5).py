# -*- coding: utf-8 -*-
"""
Created on Fri Jan 11 09:09:29 2019

@author: xupengtao
"""

import os,re
import numpy as np
import matplotlib.pyplot as plt
import networkx as nx

class Asm_verify(object):
    
    comment_pattern = re.compile(r'\s*;', re.I)
    segment_pattern = re.compile(r'\s*(\w+)\s+segment', re.I)
    segmentend_pattern = re.compile(r'\s*(\w+)\s+ends', re.I)
    proc_pattern = re.compile(r'\s*(\w+)\s+proc',re.I)
    procend_pattern = re.compile(r'\s*(\w+)\s+endp', re.I)
    grade_pattern = re.compile(r'\s*(\w+)\s*:\s*(\w*)',re.I)
    call_pattern = re.compile(r'\s*call\s+(\w+)', re.I)
    asminstruc = re.compile(r'\s*(\w+)\s*', re.I)
    gradeinstruc = re.compile(r'\s*\w+\s*:\s*(\w+)\s*', re.I)
    jplable = re.compile(r'\s*\w+\s*(\w+)\s*', re.I)
    indentation_dict = {'segment':0,'proc':0,'grade':1,'asm':10,'call':10,'instructionlen':6,'comment':50}
    jpinstruction = ['JB','JBE','JC','JG','JGE','JMP','JNC','JNZ','JZ']
    instrcomment = {'JB':';低于 跳转',
                 'JBE':';低于或等于 跳转',
                 'JC':';有进位 跳转',
                 'JG':';> 跳转',
                 'JGE':';>= 跳转',
                 'JMP':'',
                 'JNC':';没有进位 跳转',
                 'JNZ':';!=0 跳转',
                 'JZ':';== 0 跳转'}
    def __init__(self,asmfilepath):
        self.asm_t = []
        self.fpath = asmfilepath
        self.file_recomment(self.asm_t,self.fpath)
        self.asmlinetype = ['']*len(self.asm_t)
        self.segmentdict = {}
        self.procdict = {}
        self.procdict['main'] = []
        self.asmmain_t = []
        self.asmproc_t = []
        self.asminstruction = {}
        self.segnametemp = ''
        self.procnametemp = ''
        self.segin = 0
        self.procin = 0
        self.layerH = 2
        self.layerW = 2
        plt.rc('figure',figsize = (10,150))
        
    def file_recomment(self, asm_t, path):                                          #去注释
        if asm_t is None:
            asm_t = []
        with open(path,errors = 'ignore') as f_in:
            for line in f_in.readlines():
                line = self.resp_verify(line)
                if line != '':
                    match = self.comment_pattern.match(line)
                    if not match:
                        asm_t.append(line)
                        
    def typedetect(self):                                                           #类型检测
        for i in range(len(self.asm_t)):
            if (self.segin == 0) & self.segment_verify(i):
                continue
            elif (self.segin == 1) & self.segmentend_verify(i):
                continue
            elif self.segnametemp == 'CODE':
                if (self.procin == 0) & self.proc_verify(i):
                    continue
                elif (self.procin == 1) & self.procend_verify(i):
                    continue
                elif self.grade_verify(i):
                    continue
                elif self.call_verify(i):
                    continue
                else:
                    self.asm_verify(i)
            else:
                self.asm_verify(i)
                
    def procdiliver(self):                                                          #过程分离
        self.asmmain_t = []
        self.asmproc_t = []
        for i in range(self.segmentdict['CODE'][0],self.segmentdict['CODE'][1]):
            if self.asmlinetype[i] in ['asm','grade','call']:
                self.asmmain_t.append(self.asm_t[i])
            else:
                self.asmproc_t.append(self.asm_t[i])

    def segment_verify(self,line_num):                                              #段匹配
        match = self.segment_pattern.match(self.asm_t[line_num])
        if match:
            segname = match.group(1).upper()
            self.segnametemp = segname
            self.segin = 1
            self.asm_t[line_num] = ' '*self.indentation_dict['segment'] + self.asm_t[line_num]
            self.asmlinetype[line_num] = 'segment'
            self.segmentdict[segname] = [line_num,0]
            return 1
        return 0

    def segmentend_verify(self,line_num):                                           #段结束
        match = self.segmentend_pattern.match(self.asm_t[line_num])
        if match:
            self.segnametemp = ''
            self.segin = 0
            segname = match.group(1).upper()
            self.asm_t[line_num] = ' '*self.indentation_dict['segment'] + self.asm_t[line_num]
            self.asmlinetype[line_num] = 'segment'
            self.segmentdict[segname][1] = line_num
            return 1
        return 0
            
    def proc_verify(self,line_num):                                                 #过程匹配
        match = self.proc_pattern.match(self.asm_t[line_num])
        if match:
            procname = match.group(1).lower()
            self.procnametemp = procname
            self.procin = 1
            self.asm_t[line_num] = ' '*self.indentation_dict['proc'] + self.asm_t[line_num]
            self.asmlinetype[line_num] = 'proc'
            self.procdict[procname] = [line_num,0]
            return 1
        return 0

    def procend_verify(self,line_num):                                              #过程结束匹配
        match = self.procend_pattern.match(self.asm_t[line_num])
        if match:
            self.procnametemp = ''
            self.procin = 0
            procname = match.group(1).lower()
            self.asm_t[line_num] = ' '*self.indentation_dict['proc'] + self.asm_t[line_num]
            self.asmlinetype[line_num] = 'proc'
            self.procdict[procname][1] = line_num
            return 1
        return 0
                        
    def grade_verify(self,line_num):                                                #标号匹配
        match = self.grade_pattern.match(self.asm_t[line_num])
        if match:
            graname = match.group(1).lower()
            gradeinstr = match.group(2)
            gradeasmline = self.asm_t[line_num][self.asm_t[line_num].find(gradeinstr):]
            gradestr = self.asm_t[line_num][:self.asm_t[line_num].find(gradeinstr)]
            gradestr = gradestr.replace(' ','',gradestr.count(' '))
            self.asm_t[line_num] = gradestr + ' '*(self.indentation_dict['asm'] - self.indentation_dict['grade'] - len(gradestr))
            self.asm_t[line_num] = self.asm_t[line_num] + gradeasmline
            self.asm_t[line_num] = ' '*self.indentation_dict['grade'] + self.asm_t[line_num]
            if self.procnametemp != '':
                self.asmlinetype[line_num] = self.procnametemp+'_'+'grade'
                self.procdict[self.procnametemp].append(['grade',graname,line_num])
            else:
                self.asmlinetype[line_num] = 'grade'
                self.procdict['main'].append(['grade',graname,line_num])
            return 1
        return 0
    
    def call_verify(self,line_num):                                                 #调用匹配
        match = self.call_pattern.match(self.asm_t[line_num])
        if match:
            callname = match.group(1).lower()
            self.asm_t[line_num] = ' '*self.indentation_dict['call'] + self.asm_t[line_num]
            if self.procnametemp != '':
                self.asmlinetype[line_num] = self.procnametemp+'_'+'call'
                self.procdict[self.procnametemp].append(['call',callname,line_num])
            else:
                self.asmlinetype[line_num] = 'call'
                self.procdict['main'].append(['call',callname,line_num])
            return 1
        return 0
    
    def asm_verify(self,line_num):
        self.asm_t[line_num] = ' '*self.indentation_dict['asm'] + self.asm_t[line_num]
        if self.procnametemp == '':
            if self.segnametemp == 'CODE':
                self.asmlinetype[line_num] = 'asm'
            else:
                self.asmlinetype[line_num] = 'notcode'
        else:
             self.asmlinetype[line_num] = self.procnametemp+'_'+'asm'

    def resp_verify(self, asm_line):                                                #去空格
        asm_line = asm_line.expandtabs(1)
        asm_line = asm_line.lstrip()
        return asm_line
        
    def findpregrade(self,proc,ind):                                                #返回call对应label
        pregrade = ''                    
        for i in range(ind,0,-1):
            if self.procdict[proc][i][0] == 'grade':
                pregrade = self.procdict[proc][i][1]
                break
        return pregrade
    
    def instrlinetab_revise(self):                                                  #指令对齐
        for i in range(self.segmentdict['CODE'][0],self.segmentdict['CODE'][1]):
            dorevise = 0
            if ('asm' in self.asmlinetype[i]) | ('call' in self.asmlinetype[i]):
                if ('DB' not in self.asm_t[i]) or ('DW' not in self.asm_t[i]):
                    match = self.asminstruc.match(self.asm_t[i])
                    if match:
                        instruc = match.group(1)
                        dorevise = 1
                    else:
                        dorevise = 0
            elif 'grade' in self.asmlinetype[i]:
                match = self.gradeinstruc.match(self.asm_t[i])
                if match:
                    instruc = match.group(1)
                    dorevise = 1
                else:
                    dorevise = 0
            if dorevise == 1:
                self.asm_t[i] = self.asm_t[i].replace(instruc, instruc + ' ' * (self.indentation_dict['instructionlen'] - len(instruc)))
        
    def instruction_comment(self, type = 0):                                        #添加注释 0:主程序 1:代码段
        for i in range(self.segmentdict['CODE'][0],self.segmentdict['CODE'][1]):
            docomment = 0
            if type == 0:
                if self.asmlinetype[i] in ['asm','call']:
                    if ('DB' not in self.asm_t[i]) and ('DW' not in self.asm_t[i]):
                        match = self.asminstruc.match(self.asm_t[i])
                        if match:
                            instruc = match.group(1).upper()
                            if instruc in self.instrcomment.keys():
                                docomment = 1
                            else:
                                docomment = 2
                elif self.asmlinetype[i] in ['grade']:
                    match = self.gradeinstruc.match(self.asm_t[i])
                    if match:
                        instruc = match.group(1).upper()
                        if instruc in self.instrcomment.keys():
                            docomment = 1
                        else:
                            docomment = 2
            else:
                if ('asm' in self.asmlinetype[i]) | ('call' in self.asmlinetype[i]):
                    if ('DB' not in self.asm_t[i]) or ('DW' not in self.asm_t[i]):
                        match = self.asminstruc.match(self.asm_t[i])
                        if match:
                            instruc = match.group(1).upper()
                            if instruc in self.instrcomment.keys():
                                docomment = 1
                            else:
                                docomment = 2
                elif 'grade' in self.asmlinetype[i]:
                    match = self.gradeinstruc.match(self.asm_t[i])
                    if match:
                        instruc = match.group(1).upper()
                        if instruc in self.instrcomment.keys():
                            docomment = 1
                        else:
                            docomment = 2
            if docomment == 1:    
                if ';' not in self.asm_t[i]:
                    self.asm_t[i] = self.asm_t[i][:-1] + ' ' * (self.indentation_dict['comment']-len(self.asm_t[i])+1)    \
                    + self.instrcomment[instruc] + '\n'
                else:
                    instrline = self.asm_t[i][:self.asm_t[i].find(';')]
                    commentline = self.asm_t[i][self.asm_t[i].find(';'):-1]
                    self.asm_t[i] = instrline + ' ' * (self.indentation_dict['comment']-len(instrline)) + commentline + '\n'
            elif docomment == 2:
                if ';' not in self.asm_t[i]:
                    self.asm_t[i] = self.asm_t[i][:-1] + ' ' * (self.indentation_dict['comment']-len(self.asm_t[i])+1) + '\n'
                else:
                    instrline = self.asm_t[i][:self.asm_t[i].find(';')]
                    commentline = self.asm_t[i][self.asm_t[i].find(';'):-1]
                    self.asm_t[i] = instrline + ' ' * (self.indentation_dict['comment']-len(instrline)) + commentline + '\n'
                                        
    def instruction_analysis(self, type = 0):                                       #0:主程序指令集分析 1:代码段指令集分析
        self.asminstruction = {}
        for i in range(self.segmentdict['CODE'][0],self.segmentdict['CODE'][1]):
            if type == 0:
                if self.asmlinetype[i] in ['asm','call']:
                    if ('DB' not in self.asm_t[i]) and ('DW' not in self.asm_t[i]):
                        match = self.asminstruc.match(self.asm_t[i])
                        if match:
                            instruc = match.group(1).upper()
                            if instruc in self.asminstruction.keys():
                                self.asminstruction[instruc] = self.asminstruction[instruc]+1
                            else:
                                self.asminstruction[instruc] = 1
                elif self.asmlinetype[i] in ['grade']:
                    match = self.gradeinstruc.match(self.asm_t[i])
                    if match:
                        instruc = match.group(1).upper()
                        if instruc in self.asminstruction.keys():
                            self.asminstruction[instruc] = self.asminstruction[instruc]+1
                        else:
                            self.asminstruction[instruc] = 1
            else:
                if ('asm' in self.asmlinetype[i]) | ('call' in self.asmlinetype[i]):
                    if ('DB' not in self.asm_t[i]) or ('DW' not in self.asm_t[i]):
                        match = self.asminstruc.match(self.asm_t[i])
                        if match:
                            instruc = match.group(1).upper()
                            if instruc in self.asminstruction.keys():
                                self.asminstruction[instruc] = self.asminstruction[instruc]+1
                            else:
                                self.asminstruction[instruc] = 1
                elif 'grade' in self.asmlinetype[i]:
                    match = self.gradeinstruc.match(self.asm_t[i])
                    if match:
                        instruc = match.group(1).upper()
                        if instruc in self.asminstruction.keys():
                            self.asminstruction[instruc] = self.asminstruction[instruc]+1
                        else:
                            self.asminstruction[instruc] = 1
          
    def call_hierarchy(self,proclist = ['main'],layer_step = 0):                    #生成过程调用关系图, proclist：本layer过程名列表，layer_step：layer数
        if proclist == ['main']:                                                        #初始化
            self.mdgraph = nx.DiGraph()
            self.mdgraphpos = {}
#            self.mdgraph.add_node('main')
            for key in self.procdict.keys():
                self.mdgraph.add_node(key)
            self.mdgraphpos['main'] = np.array([0,25*(5-layer_step)])
            self.notfindproc = []
        sub_proc = []                                                                   #nextlayer子过程列表初始化
        px = 0                                                                          #labelx坐标
        for proc in proclist:
            if proc in self.procdict.keys():                                            #proc存在于过程字典中
                procL = self.procdict[proc]
            else:                                                                       #否则在为寻找到过程列表中添加该proc，此处主要为call label
                self.notfindproc.append(proc)
                continue
            if proc != 'main':                                                          #procdict['main']未存储范围
                procL = procL[2:]
            pxtemp = px                                                                 #暂存x坐标
            for i in range(len(procL)):
                try:
                    if procL[i][0] == 'call':
                        self.mdgraph.add_edge(proc,procL[i][1])
                        if procL[i][1] not in self.mdgraphpos.keys():
                            sub_proc.append(procL[i][1])
                            self.mdgraphpos[procL[i][1]] = np.array([px,25*(4-layer_step)])
                            px = px + 50
                        else:
                            self.mdgraphpos[procL[i][1]][1] = self.mdgraphpos[procL[i][1]][1] + 1
                except:
                    print(procL)
            if (px-pxtemp != 0) & (proc in self.mdgraphpos.keys()):
                self.mdgraphpos[proc][0] = (px-pxtemp)/2+pxtemp
        if sub_proc != []:
            self.call_hierarchy(sub_proc,layer_step+1)
        
    def jp_analysis(self):                                                          #生成跳转关系图
        self.jpdgraph = nx.DiGraph()
        self.jpdgraphpos = {}
        self.gradelist = {}
        self.notusedgradelist = []
        self.gradestack = []
        self.analysisline = [0]*self.segmentdict['CODE'][1]
        self.jplayerindex = [0]*1000
        self.gradelayer = {}
        jplinesign = 0
        currgrade = 'procstart'
        currlayer = 0
        self.jpdgraphpos['procstart'] = np.array([self.layerH * 1, self.layerH * 100])
        for i in range(self.segmentdict['CODE'][0],self.segmentdict['CODE'][1]):        #存当前范围代码段所有lable
            if self.asmlinetype[i] in ['grade']:
                match = self.grade_pattern.match(self.asm_t[i])
                if match:
                    gradetemp = match.group(1).upper()
                    if gradetemp not in self.gradelist.keys():
                        self.gradelist[gradetemp] = i
        i = self.segmentdict['CODE'][0]
        while i < self.segmentdict['CODE'][1]:
            jplinesign = 0
            if self.asmlinetype[i] in ['asm','call']:
                if ('DB' not in self.asm_t[i]) and ('DW' not in self.asm_t[i]):
                    match = self.asminstruc.match(self.asm_t[i])
                    if match:
                        instruc = match.group(1).upper()
                        if instruc in self.jpinstruction:
                            match = self.jplable.match(self.asm_t[i])
                            jpgrade = match.group(1).upper()
                            jplinesign = 1
            elif self.asmlinetype[i] in ['grade']:
                match = self.grade_pattern.match(self.asm_t[i])
                if match:
                    gradetemp = match.group(1).upper()
                    if ((gradetemp in self.gradelist.keys()) and (not self.jpdgraph.has_node(gradetemp))):
                        self.jpdgraph.add_node(gradetemp)
                        self.jplayerindex[currlayer + 1] = self.jplayerindex[currlayer + 1] + 1
                        self.jpdgraphpos[gradetemp] = np.array([self.layerW * self.jplayerindex[currlayer + 1], self.layerH * (100 - (currlayer + 1))])
                        self.gradelayer[gradetemp] = currlayer + 1
                        self.jpdgraph.add_edge(currgrade,gradetemp)
                        currgrade = gradetemp
                        currlayer = self.gradelayer[gradetemp]
                    elif self.jpdgraph.has_node(gradetemp):
                        currgrade = gradetemp
                        currlayer = self.gradelayer[gradetemp]
                match = self.gradeinstruc.match(self.asm_t[i])
                if match:
                    instruc = match.group(1).upper()
                    if instruc in self.jpinstruction:
                        instructline = self.asm_t[i][self.asm_t[i].find(':')+1:]
                        match = self.jplable.match(instructline)
                        jpgrade = match.group(1).upper()
                        jplinesign = 1
            if jplinesign == 1:
#                if currgrade == 'SDI2':
#                    j = 1
#                if jpgrade == 'SDI2':
#                    j = 1
                if instruc != 'JMP':                                                #非无条件跳转
                    if self.jpdgraph.has_node(jpgrade):
                        self.jpdgraph.add_edge(currgrade,jpgrade)
                        if self.analysisline[self.gradelist[jpgrade]] != 1:
                            self.gradestack.append(jpgrade)
                    else:
                        if jpgrade in self.gradelist.keys():
                            self.jpdgraph.add_node(jpgrade)
                            self.jplayerindex[currlayer + 1] = self.jplayerindex[currlayer + 1] + 1
                            self.jpdgraphpos[jpgrade] = np.array([self.layerW * self.jplayerindex[currlayer + 1], self.layerH * (100 - (currlayer + 1))])
                            self.gradelayer[jpgrade] = currlayer + 1
                            self.jpdgraph.add_edge(currgrade,jpgrade)
                            if self.analysisline[self.gradelist[jpgrade]] != 1:
                                self.gradestack.append(jpgrade)
                        else:
                            self.notusedgradelist.append(jpgrade)
                else:                                                               #无条件跳转
                    if self.jpdgraph.has_node(jpgrade):
                        self.jpdgraph.add_edge(currgrade,jpgrade)
                        currgrade = jpgrade
                        currlayer = self.gradelayer[jpgrade]
                        self.analysisline[i] = 1
                        i = self.gradelist[jpgrade]
                        if self.analysisline[i] == 1:
                            dostop = 1
                            while len(self.gradestack) != 0:
                                currgrade = self.gradestack.pop()
                                currlayer = self.gradelayer[currgrade]
                                i = self.gradelist[currgrade]
                                if self.analysisline[i] == 0:
                                    dostop = 0
                                    break
                            if dostop == 0:
                                continue
                            else:
                                break
                        else:
                            continue
                    else:
                        if jpgrade in self.gradelist.keys():
                            self.jpdgraph.add_node(jpgrade)
                            self.jplayerindex[currlayer + 1] = self.jplayerindex[currlayer + 1] + 1
                            self.jpdgraphpos[jpgrade] = np.array([self.layerW * self.jplayerindex[currlayer + 1], self.layerH * (100 - (currlayer + 1))])
                            self.gradelayer[jpgrade] = currlayer + 1
                            self.jpdgraph.add_edge(currgrade,jpgrade)
                            currgrade = jpgrade
                            currlayer = self.gradelayer[jpgrade]
                            self.analysisline[i] = 1
                            i = self.gradelist[jpgrade]
                            if self.analysisline[i] == 1:
                                dostop = 1
                                while len(self.gradestack) != 0:
                                    currgrade = self.gradestack.pop()
                                    currlayer = self.gradelayer[currgrade]
                                    i = self.gradelist[currgrade]
                                    if self.analysisline[i] == 0:
                                        dostop = 0
                                        break
                                if dostop == 0:
                                    continue
                                else:
                                    break
                            else:
                                continue
                        else:
                            self.notusedgradelist.append(jpgrade)
            self.analysisline[i] = 1
            i = i+1

    def dgraph_repair(self,dgraph,type):                                            #图修正 type：0去除单通路节点
        if type == 0:
            edges = dgraph.edges()
            nodes = dgraph.nodes()
            nodedict = {}
            repairnode = []
            rmnode = []
            rmedge = []
            for edge in edges:                                                          #移除自引用的边
                if edge[0] == edge[1]:
                    rmedge.append(edge)
                    dgraph.remove_edges_from(edge)
            for edge in rmedge:
                edges.remove(edge)
            for edge in dgraph.edges():
                if edge[0] == edge[1]:
                    print('find similar edge')
            for node in nodes:                                                          #计算节点的非自引用边的个数
                for edge in edges:
                    if node in edge:
                        if node in nodedict.keys():
                            nodedict[node][0] = nodedict[node][0]+1
                            nodedict[node].append(edge)
                        else:
                            nodedict[node] = [1,]
                            nodedict[node].append(edge)
            for node in nodedict.keys():                                                #检索需移除节点
                if nodedict[node][0] == 2:
                    if (nodedict[node][1][0] != nodedict[node][2][0]) & (nodedict[node][1][1] != nodedict[node][2][1]):
                        repairnode.append(node)
            for node in repairnode:                                                     #完成节点移除
                if node not in rmnode:
                    rmnode.append(node)
                    if (nodedict[node][1][0] != node):
                        pre_node = nodedict[node][1][0]
                        next_node = nodedict[node][2][1]
                        pre_edge = nodedict[node][1]
                        next_edge = nodedict[node][2]
                    else:
                        pre_node = nodedict[node][2][0]
                        next_node = nodedict[node][1][1]
                        pre_edge = nodedict[node][2]
                        next_edge = nodedict[node][1]
                    while (pre_node in repairnode) | (next_node in repairnode):
                        if pre_node in repairnode:
                            rmnode.append(pre_node)
                            if (nodedict[pre_node][1][1] == pre_node):
                                pre_edge = (nodedict[pre_node][1][0],node)
                                pre_node = nodedict[pre_node][1][0]
                            else:
                                pre_edge = (nodedict[pre_node][2][0],node)
                                pre_node = nodedict[pre_node][2][0]
                        if next_node in repairnode:
                            rmnode.append(next_node)
                            if (nodedict[next_node][1][0] == next_node):
                                next_edge = (node,nodedict[next_node][1][1])
                                next_node = nodedict[next_node][1][1]
                            else:
                                next_edge = (node,nodedict[next_node][2][1])
                                next_node = nodedict[next_node][2][1]
                    newedge = (pre_edge[0],next_edge[1])
                    dgraph.add_edges_from((newedge,))
            for node in rmnode:
                dgraph.remove_node(node)
            return dgraph
            
    def file_out(self):                                                             #输出文件
        foderpath = self.fpath[:self.fpath.rfind('\\')]
        fname = self.fpath[self.fpath.rfind('\\')+1:self.fpath.rfind('.')]
        outpath = os.path.join(foderpath,fname)
        if not os.path.exists(outpath):
            os.mkdir(outpath)
        asmmainpath = outpath + '\\' + fname +'_asmmain.asm'
        asmprocpath = outpath + '\\' + fname +'_asmproc.asm'
        recommentpath = outpath + '\\' + fname +'_recomment.asm'
        with open(asmmainpath,'w') as f_out:
            f_out.writelines(self.asmmain_t)
        with open(asmprocpath,'w') as f_out:
            f_out.writelines(self.asmproc_t)
        with open(recommentpath,'w') as f_out:
            f_out.writelines(self.asm_t)
            
    def run(self):
        self.typedetect()
        self.procdiliver()
        self.instrlinetab_revise()
        self.instruction_comment()
        self.file_out()

####################################################################################################

Asmverify = Asm_verify(r'E:\Workspace\xxj6\主处理程序汇总\T50HZY.asm')
Asmverify.run()
#Asmverify.call_hierarchy()
#for k,v in Asmverify.mdgraph.degree().items():
#    if v == 0:
#        Asmverify.mdgraphpos[k] = np.array([0,0])
#        print(k)
#nx.draw(Asmverify.mdgraph,Asmverify.mdgraphpos,arrows=True,node_size=2000,node_color='w',with_labels=True)
#print(Asmverify.mdgraph.degree())
#print(nx.edges(Asmverify.mdgraph,['main']))

#Asmverify.jp_analysis()
#nx.draw(Asmverify.jpdgraph,Asmverify.jpdgraphpos,arrows=True,node_size=2000,node_color='w',with_labels=True)
#for k,v in Asmverify.jpdgraph.degree().items():
#    if v == 1:
#        print(k)