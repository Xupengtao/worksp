# -*- coding: utf-8 -*-
"""
Created on Sat Oct  6 00:25:04 2018

@author: wolai
"""
import os
import numpy as np
from pandas import Series,DataFrame
import pandas as pd
import matplotlib.pyplot as plt
import struct

class Pdw_Analysis:
    def __init__(self,InitFilePath):
        InitArray = pd.read_csv(InitFilePath,names = ['type','value'],index_col = 'type')
        TOA_unit = InitArray.loc['TOA_unit'].value
        PW_unit = InitArray.loc['PW_unit'].value
        RF_unit = InitArray.loc['RF_unit'].value
        PA_unit = InitArray.loc['PA_unit'].value
        AZ_unit = InitArray.loc['AZ_unit'].value
        EL_unit = InitArray.loc['EL_unit'].value
        self.unitSeries = Series([1,TOA_unit,PW_unit,RF_unit,PA_unit,AZ_unit,EL_unit,0],index = ['RadarSource','TOA','PW','RF','PA','AZ','EL','PRI'])
        
        self.Msg_Length = int(InitArray.loc['Msg_Length'].value)
        self.Msg_Start = int(InitArray.loc['Msg_Start'].value)
        self.Msg_Head = int(InitArray.loc['Msg_Head'].value)
        self.Msg_Tail = int(InitArray.loc['Msg_Tail'].value)
        
        RadarSource_start = int(InitArray.loc['RadarSource_start'].value)
        TOA_start = int(InitArray.loc['TOA_start'].value)
        PW_start = int(InitArray.loc['PW_start'].value)
        RF_start = int(InitArray.loc['RF_start'].value)
        PA_start = int(InitArray.loc['PA_start'].value)
        AZ_start = int(InitArray.loc['AZ_start'].value)
        EL_start = int(InitArray.loc['EL_start'].value)
        RadarSource_size = int(InitArray.loc['RadarSource_size'].value)
        TOA_size = int(InitArray.loc['TOA_size'].value)
        PW_size = int(InitArray.loc['PW_size'].value)
        RF_size = int(InitArray.loc['RF_size'].value)
        PA_size = int(InitArray.loc['PA_size'].value)
        AZ_size = int(InitArray.loc['AZ_size'].value)
        EL_size = int(InitArray.loc['EL_size'].value)
        
        self.LenType = np.zeros(int(self.Msg_Length/2)-1,dtype = 'u2')
        self.StartLoc = np.zeros(8,dtype = 'u2')
        
        self.StartLoc[0] = RadarSource_start
        self.StartLoc[1] = TOA_start
        self.StartLoc[2] = PW_start
        self.StartLoc[3] = RF_start
        self.StartLoc[4] = PA_start
        self.StartLoc[5] = AZ_start
        self.StartLoc[6] = EL_start
        self.StartLoc[7] = 0
        
        self.LenType[RadarSource_start] = RadarSource_size
        self.LenType[TOA_start] = TOA_size
        self.LenType[PW_start] = PW_size
        self.LenType[RF_start] = RF_size
        self.LenType[PA_start] = PA_size
        self.LenType[AZ_start] = AZ_size
        self.LenType[EL_start] = EL_size
        
        self.UnpackString = '>' + 'H'*int(self.Msg_Length/2)
        
        self.Buffer_Length = int(InitArray.loc['Buffer_Length'].value)
        self.DataFrame = DataFrame(np.zeros((self.Buffer_Length,8)),columns = ['RadarSource','TOA','PW','RF','PA','AZ','EL','PRI'])
        plt.rc('figure',figsize = (9,9))
        
    def file_init(self,PDWFilePath):
        self.filepath = PDWFilePath
        self.filename = os.path.basename(self.filepath)
        self.filename = self.filename[:self.filename.index('.')]
        self.FileSize = os.path.getsize(PDWFilePath)
        self.FileSeek = self.Msg_Start
        self.ReadLength = 0
        self.folderpath = os.path.join(os.path.dirname(self.filepath),'out')
        if not os.path.exists(self.folderpath):
            os.mkdir(self.folderpath)
        
    def file_read(self):
        with open(self.filepath, 'rb') as fp:
#           check
            if self.FileSeek + self.Buffer_Length*self.Msg_Length > self.FileSize:
                self.ReadLength = int((self.FileSize-self.FileSeek)/self.Msg_Length)
                rtn = 1
            else:
                self.ReadLength = self.Buffer_Length
                rtn = 0
            fp.seek(self.FileSeek)
            ByteData = fp.read(2)
            IntD = int.from_bytes(ByteData, byteorder='big')
            if IntD != self.Msg_Head:
                return -1
            fp.seek(self.FileSeek+(self.ReadLength-1)*self.Msg_Length)
            ByteData = fp.read(2)
            IntD = int.from_bytes(ByteData, byteorder='big')
            if IntD != self.Msg_Head:
                return -1
#           read
            fp.seek(self.FileSeek)
            for i in range(self.ReadLength):
                ByteData = fp.read(self.Msg_Length)
                PdwData = np.array(struct.unpack(self.UnpackString,ByteData),dtype = 'u4')
                PdwDataL1 = PdwData[1:]
                PdwDataR1 = PdwData[:-1]
                PdwData = np.where(self.LenType != 4,PdwDataR1,PdwDataR1 + PdwDataL1*65536)
                self.DataFrame.loc[i] = PdwData[self.StartLoc]
            self.DataFrame = self.DataFrame * self.unitSeries
            print(self.DataFrame)
            self.FileSeek = self.FileSeek + self.ReadLength * self.Msg_Length
            return rtn
        
    def file_write(self,type = 0):
        try:
            if type == 0:
                self.AnalysisDF.to_csv(os.path.join(self.folderpath,self.filename+'_signle.csv'),mode = 'w',index = False)
                self.Analysis.to_csv(os.path.join(self.folderpath,self.filename+'_signle_Analysis.csv'),mode = 'w',index = False)
            elif type == 1:
                self.AnalysisDF.to_csv(os.path.join(self.folderpath,self.filename+'_all.csv'),mode = 'w',index = False)
                self.Analysis.to_csv(os.path.join(self.folderpath,self.filename+'_all_Analysis.csv'),mode = 'w',index = False)
            elif type == 2:
                self.AnalysisDF.to_csv(os.path.join(self.folderpath,self.filename+'_all.csv'),mode = 'a',index = False,header = False)
                self.Analysis.to_csv(os.path.join(self.folderpath,self.filename+'_all_Analysis.csv'),mode = 'a',index = False)
        except:
            print('file_write failed!')
        finally:
            return 1
        
    def RadarSourceSelect(self,RadarSource = 600):
        self.AnalysisDF = self.DataFrame[:self.ReadLength]
        if RadarSource == 600:
            if self.ReadLength != self.Buffer_Length:
                self.AnalysisDF = self.AnalysisDF.copy()
        else:
            mask = self.AnalysisDF.RadarSource.isin([RadarSource])
            self.AnalysisDF = self.AnalysisDF[mask].copy()
        
    def Para_Analysis(self,type = 0):
        self.AnalysisDF['PRI'] = self.AnalysisDF['TOA'].diff()
        if type == 0:
            fig,axes = plt.subplots(2,2)
            self.AnalysisDF.hist('PRI',bins = 50,xrot = 30,ax = axes[0,0],grid = False)
            axes[0,0].set_title('PRI Analysis')
            axes[0,0].set_xlabel('Stages')
            self.AnalysisDF.hist('RF',bins = 50,xrot = 30,ax = axes[0,1],grid = False)
            axes[0,1].set_title('RF Analysis')
            axes[0,1].set_xlabel('Stages')
            self.AnalysisDF.hist('PW',bins = 50,xrot = 30,ax = axes[1,0],grid = False)
            axes[1,0].set_title('PW Analysis')
            axes[1,0].set_xlabel('Stages')
            self.AnalysisDF.hist('PA',bins = 50,xrot = 30,ax = axes[1,1],grid = False)
            axes[1,1].set_title('PA Analysis')
            axes[1,1].set_xlabel('Stages')
#           axes.set_xticks([0,2500,5000,7500,10000])
#           plt.subplots_adjust(wspace = 0.2,hspace = 0.2)
#           axes.set_xticklabels(['one','two','three','four','five'],rotation = 30,fontsize = 'small')
            plt.show()
        self.Analysis = DataFrame(np.zeros((100,8)),columns = ['PRI','PRI_Frq','RF','RF_Frq','PW','PW_Frq','PA','PA_Frq'])
        PRI_Frq = self.AnalysisDF.PRI.value_counts()
        RF_Frq = self.AnalysisDF.RF.value_counts()
        PW_Frq = self.AnalysisDF.PW.value_counts()
        PA_Frq = self.AnalysisDF.PA.value_counts()
        arraysize = PRI_Frq.count() if PRI_Frq.count() < 100 else 100
        self.Analysis['PRI'] = Series(PRI_Frq.index[:arraysize],index = np.arange(arraysize))
        self.Analysis['PRI_Frq'] = Series(PRI_Frq.values[:arraysize],index = np.arange(arraysize))
        arraysize = RF_Frq.count() if RF_Frq.count() < 100 else 100
        self.Analysis['RF'] = Series(RF_Frq.index[:arraysize],index = np.arange(arraysize))
        self.Analysis['RF_Frq'] = Series(RF_Frq.values[:arraysize],index = np.arange(arraysize))
        arraysize = PW_Frq.count() if PW_Frq.count() < 100 else 100
        self.Analysis['PW'] = Series(PW_Frq.index[:arraysize],index = np.arange(arraysize))
        self.Analysis['PW_Frq'] = Series(PW_Frq.values[:arraysize],index = np.arange(arraysize))
        arraysize = PA_Frq.count() if PA_Frq.count() < 100 else 100
        self.Analysis['PA'] = Series(PA_Frq.index[:arraysize],index = np.arange(arraysize))
        self.Analysis['PA_Frq'] = Series(PA_Frq.values[:arraysize],index = np.arange(arraysize))
        self.Analysis = self.Analysis.dropna(how = 'all').fillna(0)