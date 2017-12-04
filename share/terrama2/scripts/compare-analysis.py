import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT
import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

tableName = "analysis_dcp_result"

def compareQuery(tableName):
     return "SELECT result.*, reference.* FROM "+tableName+" result INNER JOIN "+tableName+"_ref reference ON (result.id = reference.id) WHERE not EXISTS (SELECT * FROM "+tableName+"_ref WHERE result.execution_date = reference.execution_date and result.max = reference.max and result.count = reference.count and result.mean = reference.mean and result.median = reference.median and result.min = reference.min and result.standard_deviation = reference.standard_deviation and result.sum = reference.sum and result.value = reference.value and result.variance = reference.variance)"





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
