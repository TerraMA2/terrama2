import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

conn = psycopg2.connect("dbname='test' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
cur = conn.cursor()

cur.execute("""SELECT *
FROM analysis_dcp_result_ref
WHERE not EXISTS
(SELECT *
FROM analysis_dcp_result_code
WHERE analysis_dcp_result_ref.id = analysis_dcp_result_code.id )""")
rowsAnalysis = cur.fetchall();

if rowsAnalysis:
    #print("Failed Test Analysis!")
    status = "Failed"
    for rowAnalysis in rowsAnalysis:
        print(rowAnalysis);
if not rowAnalysis:
    #print("Passed Test Analysis!")
    status = "Passed"
