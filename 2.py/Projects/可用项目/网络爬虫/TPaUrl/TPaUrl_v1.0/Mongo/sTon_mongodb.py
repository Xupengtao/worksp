# -*- coding: utf-8 -*-
import pymongo
import sys
import traceback

class Singleton(object):
    def __new__(cls, *args, **kwargs):
        if not hasattr(cls, '_instance'):
            orig = super(Singleton, cls)
            cls._instance = orig.__new__(cls, *args, **kwargs)
        return cls._instance

class sTon_mongodb(Singleton):    
    def __init__(self,db_name = 'myDB',db_host = '127.0.0.1',db_port = 27017,username = None,password = None):
        # 连接
        try:
            self.conn = pymongo.MongoClient(db_host, db_port)
            self.db = self.conn[db_name]  # connect db
            self.username=username
            self.password=password
            if self.username and self.password:
                self.connected = self.db.authenticate(self.username, self.password)
            else:
                self.connected = True
        except Exception:
            self.connected = False
            print(traceback.format_exc())
            print('Connect Statics Database Fail.')
            sys.exit(1)  

    def check_connected(self):
        #检查是否连接成功
        if not self.connected:
            raise(NameError, 'stat:connected Error')
    
    def change_database(self,db_name):
        #更改数据库
        self.db = self.conn[db_name]  # connect db
    
    def insert(self, collection, value):
        # 可以使用insert直接一次性向mongoDB插入单条记录，'_id'重复会报错
        try:
            self.db[collection].insert_one(value)
        except Exception:
            print(traceback.format_exc())
    
    def insert_many(self, collection, filelist):
        # 可以使用insert_many直接一次性向mongoDB插入整个列表
        try:
            self.db[collection].insert_many(filelist)
        except Exception:
            print(traceback.format_exc())
        
    def save(self, collection, value):
        # 一次操作一条记录，根据‘_id’是否存在，决定插入或更新记录
        try:
            self.db[collection].save(value)
        except Exception:
            print(traceback.format_exc())
    
    def update(self,collection, conditions, value, s_upsert=False, s_multi=False):
        try:
            self.db[collection].update(conditions, value, upsert=s_upsert, multi=s_multi)
        except Exception:
            print(traceback.format_exc())
    
    def upsert_one(self,collection, data):
        #更新插入，根据‘_id’更新一条记录，如果‘_id’的值不存在，则插入一条记录
        try:
            query = {'_id': data.get('_id','')}
            if not self.db[collection].find_one(query):
                self.db[collection].insert(data)
            else:
                data.pop('_id') #删除'_id'键
                self.db[collection].update(query, {'$set': data})
        except Exception:
            print(traceback.format_exc())
    
    def upsert_mary(self,collection, datas):
        #批量更新插入，根据‘_id’更新或插入多条记录。
        #把'_id'值不存在的记录，插入数据库。'_id'值存在，则更新记录。
        #如果更新的字段在mongo中不存在，则直接新增一个字段
        try:
            bulk = self.db[collection].initialize_ordered_bulk_op()
            for data in datas:
                _id=data['_id']
                bulk.find({'_id': _id}).upsert().update({'$set': data})
            bulk.execute()
        except Exception:
            print(traceback.format_exc())
    
    def find_one(self,collection, value):
        #根据条件进行查询，返回一条记录
        try:
            return self.db[collection].find_one(value)
        except Exception:
            print(traceback.format_exc())
    
    def find(self,collection, value):
        #根据条件进行查询，返回所有记录
        try:
            return self.db[collection].find(value)
        except Exception:
            print(traceback.format_exc())
    
    def select_colum(self,collection, value, colum):
        #查询指定列的所有值
        try:
            return self.db[collection].find(value, {colum:1})
        except Exception:
            print(traceback.format_exc())
            
    def remove_collection(self,collection):
        #删除指定的collection
        try:
            return self.db[collection].remove()
        except Exception:
            print(traceback.format_exc())