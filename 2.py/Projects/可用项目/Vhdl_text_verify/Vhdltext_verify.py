# -*- coding: utf-8 -*-
"""
Created on Wed Oct 25 16:17:51 2017

@author: xupengtao
"""
import re

class Vhdltext_verify(object):
    
    entity_pattern = re.compile(r'\s*entity\s*(\S*)\s*is', re.I)
    arch_pattern = re.compile(r'\s*architecture\s*(\S*)\s*of', re.I)
    port_pattern = re.compile(r'.*\s*port\s*', re.I)
    end_pattern = re.compile(r'\s*end .*', re.I)
    compnt_pattern = re.compile(r'\s*component\s*(\S*)', re.I)
    proc_pattern = re.compile(r'.*process\s*\(\S*\)', re.I)
    procif_pattern = re.compile(r'\s*if(\s|\().*',re.I)
    procelsf_pattern = re.compile(r'\s*(elsif|else){1}(\n|\s|\()+',re.I)
    procendif_pattern = re.compile(r'\s*end\s+if',re.I)
    portmap_pattern = re.compile(r'.*\s*(port|generic) map\s*', re.I)
    signal_pattern = re.compile(r'\s*signal\s*',re.I)
    signalv_pattern = re.compile(r'.*[\(\)\'\"\w]+\s*<=\s*[\(\)\'\"\w]+')
    variablev_pattern = re.compile(r'.*[\(\)\'\"\w]+\s*:=\s*[\(\)\'\"\w]+')
    portmapv_pattern = re.compile(r'.*[\(\)\'\"\w]+\s*=>\s*[\(\)\'\"\w]+')
    variable_pattern = re.compile(r'\s*variable\s*',re.I)
    nosp_pattern = re.compile(r'.*[\w\(\)\:\;]{1}')
    entity_sign = 0
    arch_sign = 0
    port_sign = 0
    compnt_sign = 0
    proc_sign = 0
    proc_stack = []
    portmap_sign = 0
    
    def __init__(self, entity_name = '', arch_name = ''):
        self.entity_name = entity_name
        self.arch_name = arch_name
    
    def file_recomment(self, vhd_t, path = 'E:/Workspace/remove_comment/in/TOP_TWO.vhd'):
        if vhd_t is None:
            vhd_t = []
        with open(path,errors = 'ignore') as f_in:
            for line in f_in.readlines():
                if line[0:2] != '--':
                    match = self.nosp_pattern.match(line)
                    if match:
                        vhd_t.append(line)
    
    def file_out(self,vhd_t, path = 'E:/Workspace/remove_comment/out/out.vhd'):
        with open(path,'w') as f_out:
            f_out.writelines(vhd_t)
            
    def entity_verify(self, vhd_line):
        if self.entity_sign == 0:
            match = self.entity_pattern.match(vhd_line)
            if match:
                self.entity_name = match.group(1).upper()
                vhd_line = vhd_line.upper()
                self.entity_sign = 1
        elif self.entity_sign == 1:
            vhd_line = self.port_verify(vhd_line)
            if self.port_sign == 0:
                endentity_pattern = re.compile(r'\s*end\s+(%s|entity)' % self.entity_name, re.I)
                match = endentity_pattern.match(vhd_line)
                if match:
                    vhd_line = 'END '+ self.entity_name +';\n\n'
                    self.entity_sign = 2
                    self.arch_sign = 0
        return vhd_line
        
    def arch_verify(self, vhd_line):
        if self.arch_sign == 0:
            match = self.arch_pattern.match(vhd_line)
            if match:
                self.arch_name = match.group(1).upper()
                vhd_line = vhd_line.upper()
                self.arch_sign = 1
        elif self.arch_sign == 1:
            vhd_line = self.compnt_verify(vhd_line)
            if self.compnt_sign == 0:
                if ':' in vhd_line:
                    sign = vhd_line.find(':')
                    if sign != -1 and sign < 35:
                        vhd_line =' '*5 + vhd_line[:sign] + ' '*(35-sign) + ': ' + \
                        vhd_line[sign+1:].lstrip().upper()
                    else:
                        vhd_line = ' '*5 + vhd_line
                archbegin_pattern = re.compile(r'\s*begin\s*',re.I)
                match = archbegin_pattern.match(vhd_line)
                if match:
                    vhd_line = vhd_line.lstrip()+'\n'
                    self.arch_sign = 2
        elif self.arch_sign == 2:
            vhd_line = self.process_verify(vhd_line)
            vhd_line = self.portmap_verify(vhd_line)
            if self.proc_sign == 0 and self.portmap_sign == 0:
                match = self.signalv_pattern.match(vhd_line)                       #添加赋值语句对齐操作
                if match:
                    sign = vhd_line.find('<=')
                    if sign != -1 and sign < 25:
                        vhd_line = vhd_line[:sign] + ' '*(25-sign) + '<=' + ' '*5 + \
                        vhd_line[sign+2:].lstrip()
                endarch_pattern = re.compile(r'\s*end\s+(%s|architecture)' % self.arch_name, re.I)
                match = endarch_pattern.match(vhd_line)
                if match:
                    vhd_line = '\n' + 'END '+self.arch_name + ';\n\n'
                    self.arch_sign = 3
                    self.entity_sign = 0
        return vhd_line
        
    def port_verify(self,vhd_line):
        if self.port_sign == 0:
            match = self.port_pattern.match(vhd_line)
            if match:
                vhd_line = ' '*5 + vhd_line
                vhd_line = vhd_line.upper()
                self.port_sign = 1
        else:
            match = self.end_pattern.match(vhd_line)
            if match:
                self.port_sign = 0
            elif ':' in vhd_line:
                vhd_line = ' '*10 + vhd_line
                sign = vhd_line.find(':')
                if sign != -1 and sign < 45:
                    vhd_line = vhd_line[:sign] + ' '*(45-sign) + vhd_line[sign:].upper()
        return vhd_line

    def compnt_verify(self,vhd_line):
        if self.compnt_sign == 0:
            match = self.compnt_pattern.match(vhd_line)
            if match:
                vhd_line = ' '*5+'COMPONENT '+match.group(1)+'\n'
                self.compnt_sign = 1
        elif self.compnt_sign == 1:
            vhd_line = self.port_verify(vhd_line)
            if self.port_sign == 0:
                endcompnt_pattern = re.compile(r'\s*end\s+component', re.I)
                match = endcompnt_pattern.match(vhd_line)
                if match:
                    vhd_line = ' '*5+'END COMPONENT;\n\n'
                    self.compnt_sign = 0
        return vhd_line
    
    def portmap_verify(self, vhd_line):
        if self.portmap_sign == 0:
            match = self.portmap_pattern.match(vhd_line)
            if match:
                vhd_line = '\n' + ' '*5+vhd_line
                self.portmap_sign = 1
        elif self.portmap_sign == 1:
            match = self.portmapv_pattern.match(vhd_line)
            if match:
                vhd_line = ' '*10+vhd_line
                sign = vhd_line.find('=>')
                if sign != -1 and sign < 35:
                    vhd_line = vhd_line[:sign] + ' '*(35-sign) + '=>' + ' '*5 + \
                    vhd_line[sign+2:].lstrip()
            pattern = re.compile(r'(.*)\)\s*;\s*')
            match = pattern.match(vhd_line)
            if match:
                if len(match.group(1)) < 5:
                    vhd_line = ' '*5+vhd_line+'\n'
                else:
                    vhd_line = vhd_line + '\n'
                self.portmap_sign = 0
        return vhd_line        
    
    def process_verify(self, vhd_line):
        if self.proc_sign == 0:
            match = self.proc_pattern.match(vhd_line)
            if match:
                vhd_line = vhd_line[:8].upper()+vhd_line[8:]
                self.proc_sign = 1
        elif self.proc_sign == 1:
            if ':' in vhd_line:
                sign = vhd_line.find(':')
                if sign != -1 and sign < 35:
                    vhd_line =' '*5 + vhd_line[:sign] + ' '*(35-sign) + ': ' + \
                    vhd_line[sign+1:].lstrip().upper()
            procbegin_pattern = re.compile(r'\s*begin\s*',re.I)
            match = procbegin_pattern.match(vhd_line)
            if match:
                vhd_line = ' '*5 + vhd_line.lstrip()
                self.proc_sign = 2
        elif self.proc_sign == 2:
            match_signal = self.signalv_pattern.match(vhd_line)
            match_variable = self.variablev_pattern.match(vhd_line)
            match_if = self.procif_pattern.match(vhd_line)                      #添加条件控制匹配堆栈
            match_elsf = self.procelsf_pattern.match(vhd_line)
            match_endif = self.procendif_pattern.match(vhd_line)
            procend_pattern = re.compile(r'\s*end process\s*',re.I)
            match_endproc = procend_pattern.match(vhd_line)     
            if match_if:
                vhd_line = ' '*5*(len(self.proc_stack)+1) + vhd_line.lstrip()
                self.proc_stack.append('if')
            elif match_elsf:
                vhd_line = ' '*5*len(self.proc_stack) + vhd_line.lstrip()
            elif match_signal:
                sign = vhd_line.find('<=')
                if sign != -1 and sign < 25:
                    vhd_line = ' '*5*(len(self.proc_stack)+1) + vhd_line[:sign] + ' '*(25-sign) + \
                    '<=' + ' '*5 + vhd_line[sign+2:].lstrip()
            elif match_variable:
                sign = vhd_line.find(':=')
                if sign != -1 and sign < 25:
                    vhd_line = ' '*5*(len(self.proc_stack)+1) + vhd_line[:sign] + ' '*(25-sign) + \
                    ':=' + ' '*5 + vhd_line[sign+2:].lstrip()
            elif match_endif:
                vhd_line = ' '*5*len(self.proc_stack) + vhd_line.lstrip()
                self.proc_stack.pop()
            elif match_endproc:
                vhd_line = vhd_line.lstrip().upper()+'\n'
                self.proc_sign = 0
        return vhd_line

    def resp_verify(self, vhd_line):
        vhd_line = vhd_line.expandtabs(1)
        vhd_line = vhd_line.lstrip()
        return vhd_line

####################################################################################################
            
#a = Vhdltext_verify()
#vhd_t = []
#a.file_recomment(vhd_t,'E:/Workspace/14m148/FPGA/OED45/daban/spu_fpga/fpga.vhd')
#for i in range(len(vhd_t)):
#    vhd_ttemp = vhd_t[i]
#    a.resp_verify(vhd_t[i])
#    vhd_ttemp2 = vhd_t[i]
#    vhd_t[i] = a.entity_verify(vhd_t[i])
#    vhd_t[i] = a.arch_verify(vhd_t[i])
#    if vhd_ttemp2 == vhd_t[i]:
#        vhd_t[i] = vhd_ttemp
#a.file_out(vhd_t)