

import json
import psycopg2

#
# On purge remove the TerraMA2 database and terrama2 folder
#

config = '../webapp/config/db.json'
if os.path.isfile(config):
    with open(config) as data_file:
        data = json.load(data_file)

        for key in data:
            database = data[key]["db"]["database"]
            username = data[key]["db"]["username"]
            password = data[key]["db"]["password"]
            host = data[key]["db"]["host"]

            conn = psycopg2.connect(host=host,database="postgres", user=username, password=password)
            cur = conn.cursor()
            conn.autocommit = True   #  Explains why we do this - we cannot drop or create from within a DB transaction. http://initd.org/psycopg/docs/connection.html#connection.autocommit
            cur.execute("DROP DATABASE IF EXISTS "+database+";")
            pass
