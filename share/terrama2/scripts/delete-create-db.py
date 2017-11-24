# @author Bianca Maciel
import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

conn = psycopg2.connect("user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
cur = conn.cursor()
dbname = 'test'
cur.execute('DROP DATABASE IF EXISTS ' + dbname)
cur.execute('CREATE DATABASE ' + dbname)
cur.close()

conn2 = psycopg2.connect("dbname='test' user='postgres' password='postgres' host = '127.0.0.1' port= '5432' ")
conn2.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
cur2 = conn2.cursor()
cur2.execute('create extension postgis')
cur2.close()

