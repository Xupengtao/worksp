# -*- coding: utf-8 -*-
from sTon_mongodb import sTon_mongodb

datas = [
    {'_id':10, 'data':88},        
    {'_id':20, 'data':99},
    {'_id':19, 'data':3366}      
]

mongoDb = sTon_mongodb()
mongoDb.check_connected()

#插入,'_id' 的值必须不存在，否则报错
data={'_id':9, 'data':66}
mongoDb.remove_collection('mytest')
mongoDb.insert('mytest', data)
mongoDb.insert_many('mytest', datas)
mongoDb.save('mytest',data)
#更新数据
mongoDb.update('mytest',{'_id':6},{'$set':{'data':'888'}}, False, False)
#更新或插入    
data={'_id':36, 'data':'dsd'}
mongoDb.upsert_one('mytest',data)  
#查找。相对于 select _id from mytest
res=mongoDb.select_colum('mytest',{},'_id')
for k in res:
    for key, value in k.items():
        print(key,":",value)
#查找。相对于 select * from mytest
res=mongoDb.find('mytest',{})
for k in res:
    for key, value in k.items():
        print(key,":",value)
#查找。相对于 select * from mytest limit 1
res=mongoDb.find_one('mytest',{})
for k in res:
    print(key,":",value)