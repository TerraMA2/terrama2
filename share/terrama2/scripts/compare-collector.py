import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

tablesName = ["pcd_picinguaba", "pcd_cunha", "pcd_guaratingueta", "pcd_itanhaem", "pcd_ubatuba"]

def compareQuery(tableName):
    return "SELECT * FROM "+tableName+"_ref WHERE not EXISTS (SELECT * FROM "+tableName+" WHERE "+tableName+".* = "+tableName+"_ref.* )"

def checkTable(tableName):
    conn = psycopg2.connect("dbname='"+dbname+"' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
    conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
    cur = conn.cursor()
    cur.execute(compareQuery(tableName))
    rows = cur.fetchall()
    if rows:
        status = 0       
    else:
        status = 1
    return status


count = 0
for tableName in tablesName:
    st = checkTable(tableName)
    if st == 1:
        count = count + 1



