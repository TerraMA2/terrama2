import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

def checkTable(query):
    conn = psycopg2.connect("dbname='"+dbname+"' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
    conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
    cur = conn.cursor()
    cur.execute(query)
    rows = cur.fetchall()
    if rows:
        status = 0
    else:
        status = 1
    return status


count = 0
typeAn = typeAnalysis
if (typeAn == "dcp_history" or  typeAn == "operator_dcp" or  typeAn == "operator_history_interval"):
    tablesName = ["pcd_picinguaba", "pcd_cunha", "pcd_guaratingueta", "pcd_itanhaem", "pcd_ubatuba"]
    for tableName in tablesName:
        query = "SELECT * FROM "+tableName+"_ref WHERE NOT EXISTS (SELECT * FROM "+tableName+" WHERE "+tableName+".* = "+tableName+"_ref.* )"
        st = checkTable(query)
        if st == 1:
            count = count + 1
elif (typeAn == "occ"):
    tableName = "queimadas_test_table"
    query = "SELECT * FROM "+tableName+"_ref WHERE NOT EXISTS (SELECT * FROM "+tableName+" WHERE "+tableName+".data_pas = "+tableName+"_ref.data_pas and "+tableName+".satelite = "+tableName+"_ref.satelite and "+tableName+".geom = "+tableName+"_ref.geom)"
    st = checkTable(query)
    if st == 1:
        count = count + 1
