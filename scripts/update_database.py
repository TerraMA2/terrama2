import json
import psycopg2
import glob

# open version file
with open("../share/terrama2/version.json") as version_file:
    version = json.load(version_file)
    major = version["major"]
    minor = version["minor"]
    patch = version["patch"]
    tag = version["tag"]
    database = version["database"]

# error reading version file
if major is None:
    return 1

config = '../webapp/config/webapp.json'
if os.path.isfile(config):
    with open(config) as data_file:
        data = json.load(data_file)

        for key in data:
            database = data[key]["db"]["database"]
            username = data[key]["db"]["username"]
            password = data[key]["db"]["password"]
            host = data[key]["db"]["host"]

            conn = psycopg2.connect(host=host, database=database, user=username, password=password)
            cur = conn.cursor()
            conn.autocommit = True
            # Explains why we do this - we cannot drop or create from
            # within a DB transaction.
            # http://initd.org/psycopg/docs/connection.html#connection.autocommit
            try:
                cursor.execute("")
                record = cursor.fetchone()
            except psycopg2.ProgrammingError:
                # no database found, assume 4.0-0
                current_major = 4
                current_minor = 0
                current_database = 0

            while :
                converters = glob.glob("../scripts/bd_update/"+current_major+"."+current_minor"-"+current_database"_*.sql")
                if converters is None:
                    return 1;


            cursor.execute(open("schema.sql", "r").read())
            pass
