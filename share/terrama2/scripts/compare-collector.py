import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

tablesName = ["pcd_picinguaba", "pcd_cunha", "pcd_guaratingueta", "pcd_itanhaem", "pcd_ubatuba"]

def compareQuery(tableName):
    return "SELECT * FROM "+tableName+"_ref WHERE not EXISTS (SELECT * FROM "+tableName+" WHERE "+tableName+".* = "+tableName+"_ref.* )"

def checkTable(tableName):
    conn = psycopg2.connect("dbname='test' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
    conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
    cur = conn.cursor()
    cur.execute(compareQuery(tableName))
    rows = cur.fetchall()
    st = status_compare(tableName, rows)
    return st


def status_compare(tableName, rows):
    if rows:
        print("Failed Test Collector " + tableName)
        status = "Failed"
        for row in rows:
            print(row);
    else:
        print("Passed Test Collector " + tableName)
        status = "Passed"
    return status

cont = 0
for tableName in tablesName:
    st = checkTable(tableName)
    if st == "Passed":
        cont = cont + 1



