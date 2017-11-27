# @author Bianca Maciel
import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

conn = psycopg2.connect("user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
cur = conn.cursor()
cur.execute('DROP DATABASE IF EXISTS '  + dbname)
