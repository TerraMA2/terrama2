import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

conn = psycopg2.connect("dbname='test' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
cur = conn.cursor()



cur.execute("""SELECT *
FROM pcd_picinguaba_ref
WHERE not EXISTS
(SELECT *
FROM pcd_picinguaba
WHERE pcd_picinguaba.* = pcd_picinguaba_ref.* )""")
rowsPicinguaba = cur.fetchall();
if rowsPicinguaba:
    #print("Failed Test Collector Picinguaba!")
    status = "Failed"
    for rowsPicinguaba in rowsPicinguaba:
        print(rowsPicinguaba);
if  not rowsPicinguaba:
    #print("Passed Test Collector Picinguaba!")
    status = "Passed"


cur.close()

