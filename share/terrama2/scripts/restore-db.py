import psycopg2
import os, sys
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

conn = psycopg2.connect("dbname='"+dbname+"' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
cur = conn.cursor()

command = "/usr/bin/pg_restore -h 'localhost' -p '5432' -U 'postgres' -d 'test' -v '/home/bianca/unittest_serramar_ref.backup'"
os.system(command)
cur.close()

