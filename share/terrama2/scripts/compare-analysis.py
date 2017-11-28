import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT
import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

tableName = "analysis_dcp_result"

def compareQuery(tableName):
    #return "SELECT * FROM "+tableName+"_ref WHERE not EXISTS (SELECT * FROM "+tableName+" WHERE "+tableName+".* = "+tableName+"_ref.* )"
     return "SELECT result.*, reference.* FROM "+tableName+" result INNER JOIN "+tableName+"_ref reference ON (result.id = reference.id) WHERE not EXISTS (SELECT * FROM "+tableName+"_ref WHERE result.execution_date = reference.execution_date and result.max = reference.max)"





def checkTable(tableName):
    conn = psycopg2.connect("dbname='"+dbname+"' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
    conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
    cur = conn.cursor()
    cur.execute(compareQuery(tableName))
    rows = cur.fetchall()
    if rows:
	return 0
    else:
	return 1

status = checkTable(tableName)
