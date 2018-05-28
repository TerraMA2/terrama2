import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

def checkTable(query):
    conn = psycopg2.connect("dbname='"+dbname+"' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
    conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
    cur = conn.cursor()
    cur.execute(query)
    rows = cur.fetchall()
    cur.close()
    if rows:
	    return 0
    else:
	    return 1


typeAn = typeAnalysis
if(typeAn == "dcp_history" or  typeAn == "operator_history_interval"):
    tableName = "analysis_dcp_result"
    query = "SELECT result.*, reference.* FROM "+tableName+"  result INNER JOIN "+tableName+"_ref reference ON (result.id = reference.id) WHERE NOT EXISTS (SELECT * FROM analysis_dcp_result_ref WHERE result.id = reference.id AND result.execution_date = reference.execution_date AND result.max = reference.max AND result.count = reference.count AND result.mean = reference.mean AND result.median = reference.median AND result.min = reference.min AND result.sum = reference.sum AND result.value = reference.value AND result.standard_deviation = reference.standard_deviation AND result.variance = reference.variance)"
    status = checkTable(query)
elif(typeAn == "operator_dcp"):
    tableName = "analysis_dcp_result"
    query = "SELECT result.*, reference.* FROM "+tableName+"  result INNER JOIN "+tableName+"_ref reference ON (result.id = reference.id) WHERE NOT EXISTS (SELECT * FROM analysis_dcp_result_ref WHERE result.id = reference.id AND result.execution_date = reference.execution_date AND result.max = reference.max AND result.count = reference.count AND result.mean = reference.mean AND result.median = reference.median AND result.min = reference.min AND result.sum = reference.sum AND result.value = reference.value)"
    status = checkTable(query)
elif (typeAn == "occ"):
    tableName = "occurrence_analysis_result"
    #query = "SELECT result.*, reference.* FROM "+tableName+" result INNER JOIN "+tableName+"_ref reference ON (result.fid = reference.fid) WHERE NOT EXISTS (SELECT * FROM "+tableName+"_ref WHERE result.execution_date = reference.execution_date and result.count = reference.count)"
    query = "SELECT execution_date, count FROM "+tableName+"_ref WHERE NOT EXISTS (SELECT * FROM "+tableName+" WHERE "+tableName+".execution_date = "+tableName+"_ref.execution_date and "+tableName+".count = "+tableName+"_ref.count )"
    status = checkTable(query)




