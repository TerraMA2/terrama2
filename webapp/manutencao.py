#!/usr/bin/python

import json
import glob
import psycopg2
import os
import string

hidro1_id = 73

terrama2_basepath = "../../.."
armazenamento_table = "storages"

hidro1_sql = """insert into %s.%s (
    name,
    active, 
    schedule_type,
    filter, 
    backup,
    project_id,
    data_provider_id,
    data_series_id,
    schedule_id) values 
    ('teste hidro1', true, 1, true, true, 3, 16, 73, %d)"""

schedule_sql = """insert into %s.schedules
    (frequency,
    frequency_unit,
    frequency_start_time) values
    (24, 'hours', '00:00:00-00:00') returning id"""

select_uri = """SELECT 
  data_providers.uri
FROM 
  %s.data_providers, 
  %s.data_series, 
  %s.storages
WHERE 
  data_series.data_provider_id = data_providers.id AND
  storages.data_series_id = data_series.id AND
  storages.data_series_id = %d"""

select_mask = """SELECT 
  data_set_formats.value
FROM 
  %s.data_series, 
  %s.data_sets, 
  %s.data_set_formats
WHERE 
  data_sets.data_series_id = data_series.id AND
  data_set_formats.data_set_id = data_sets.id AND
  data_set_formats.key = 'mask' AND 
  data_series.id = %d"""

config_path = terrama2_basepath + "/webapp/config/instances/"
instances = glob.glob(config_path + "*.json")
for config in instances:
    print("Current config file: " + config)
    if os.path.isfile(config):
    # read all configured instances and update all databases
        with open(config) as data_file:
            data = json.load(data_file)
            # get database config for this terrama2 instance
            database_uri = data["db"]["database"]
            username = data["db"]["username"]
            password = data["db"]["password"]
            host = data["db"]["host"]

            print("Current database: " + database_uri)

            sql_create = """CREATE TABLE %s.storages (
                id serial NOT NULL,
                name character varying(255),
                description text,
                active boolean,
                schedule_type integer,
                filter boolean,
                backup boolean,
                project_id integer NOT NULL,
                data_provider_id integer NOT NULL,
                data_series_id integer NOT NULL,
                schedule_id integer,
                automatic_schedule_id integer,
                CONSTRAINT storages_pkey PRIMARY KEY (id),
                CONSTRAINT storages_automatic_schedule_id_fkey FOREIGN KEY (automatic_schedule_id)
                    REFERENCES %s.automatic_schedules (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE,
                CONSTRAINT storages_data_provider_id_fkey FOREIGN KEY (data_provider_id)
                    REFERENCES %s.data_providers (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE,
                CONSTRAINT storages_data_series_id_fkey FOREIGN KEY (data_series_id)
                    REFERENCES %s.data_series (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE,
                CONSTRAINT storages_project_id_fkey FOREIGN KEY (project_id)
                    REFERENCES %s.projects (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE,
                CONSTRAINT storages_schedule_id_fkey FOREIGN KEY (schedule_id)
                    REFERENCES %s.schedules (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE
                )""" % (database_uri,database_uri,database_uri,database_uri,database_uri,database_uri)

            print(sql_create)

            try:
                conn = psycopg2.connect(
                    host=host,
                    database=database_uri,
                    user=username,
                    password=password)
            except psycopg2.OperationalError:
                print("ERRO ")
                continue

            cur = conn.cursor()
            conn.autocommit = True
            try:
                #Verifies if table storages exists
                cur.execute("select exists(select * from information_schema.tables where table_name='%s')"%(armazenamento_table))
                if not cur.fetchone()[0]:
                    #Creates table storages
                    cur.execute(sql_create)

                #Verifies if there is a rule to shelve this data_serie
                cur.execute("select * from %s.%s where data_series_id = %d"%(database_uri, armazenamento_table, hidro1_id))
                regra = cur.fetchone()
                if not regra:
                    cur.execute(schedule_sql%(database_uri))
                    new_schedule_id = cur.fetchone()[0]
                    print (hidro1_sql%(database_uri, armazenamento_table, new_schedule_id))
                    cur.execute(hidro1_sql%(database_uri, armazenamento_table, new_schedule_id))
                
                print regra
                #Gets a path where data is stored
                cur.execute(select_uri % (database_uri,database_uri,database_uri,hidro1_id))
                uri = cur.fetchone()[0]
                print uri
                cur.execute(select_mask % (database_uri,database_uri,database_uri,hidro1_id))
                mask = cur.fetchone()[0]
                print mask
                path = uri[uri.find('//')+2:len(uri)] + "/" + mask[0:mask.find("/")]
                print path
                date_mask = mask[mask.find('%')]

                if (regra[6]):
                    cur.execute(("select uri from %s.data_providers where id = %d") % (database_uri, regra[8]))
                    uriout = cur.fetchone()[0]
                    print uriout
                    pathout = uriout[uriout.find('//')+2:len(uriout)]
                    print pathout
                
                #if backup
                tiffs = []
                if (regra[6]):
                    files = os.listdir(path)
                    print files


            except psycopg2.OperationalError as e:
                # no database found, assume 4.0-0
                print ("Except " + e)

        data_file.close()
