'''
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
'''

# coding: utf-8

import click
import psycopg2
from collections import defaultdict

@click.command()
@click.option('--host', default='localhost')
@click.option('--port', default=5432)
@click.option('--username', default='postgres')
@click.option('--password', default='postgres')
@click.option('--schema', default='public')
@click.option('--table')
@click.argument('database')
def database_model(host, port, username, password, schema, table, database):

    conn = psycopg2.connect("dbname='%s' user='%s' host='%s' port='%s' password='%s'" % (database, username, host, port, password))
    cursor = conn.cursor()


    select = """SELECT relname as table_name, obj_description(oid) as description
        FROM pg_class, pg_catalog.pg_tables tb
        WHERE relkind = 'r' and  tb.schemaname = '%s' and tb.tablename = relname""" % schema

    if table:
        select += " AND tb.tablename = '%s'" % table

    cursor.execute(select)

    tables = cursor.fetchall()

    for table in tables:
        describe_table(cursor, table[0], table[1])


def describe_table(cursor, table_name, description):
    cursor.execute("""select pa.attname as column, format_type(atttypid, atttypmod) AS type, attnotnull as not_null, pd.description
                           from pg_description pd, pg_class pc, pg_attribute pa
                           where pa.attrelid = pc.oid and pd.objoid = pc.oid and pd.objsubid = pa.attnum and  pc.relname = '%s'""" % table_name)
    columns = cursor.fetchall()


    cursor.execute("""SELECT kcu.column_name, constraint_type
    FROM information_schema.table_constraints AS tc
    JOIN information_schema.key_column_usage AS kcu
    ON tc.constraint_name = kcu.constraint_name
    where tc.table_name = '%s';""" % table_name)

    result = cursor.fetchall()
    keys = defaultdict(list)

    for c in result:
        keys[c[0]].append(c)

    print(" " + table_name + "::")
    print(description)

    print("\n||||||= Tabela: **%s** =||" % table_name)
    print("||=  **Coluna** =||= **Tipo** =||= **Modificador** =||")

    separator = ' || '
    for column in columns:
        desc = "|| " + column[0] + separator + column[1] + separator
        if keys.has_key(column[0]):
            for k in keys[column[0]]:
                desc += " " + k[1]
        if column[2]:
            desc += " NOT NULL"
        desc += separator
        print(desc)

    cursor.execute("""SELECT kcu.column_name, ccu.table_name, ccu.column_name, rc.update_rule, rc.delete_rule
    FROM information_schema.table_constraints AS tc JOIN information_schema.key_column_usage
    AS kcu ON tc.constraint_name = kcu.constraint_name
    JOIN information_schema.constraint_column_usage
    AS ccu ON ccu.constraint_name = tc.constraint_name
    JOIN information_schema.referential_constraints as rc
    ON rc.constraint_name = tc.constraint_name
    WHERE constraint_type = 'FOREIGN KEY' and tc.table_name = '%s';""" % (table_name))

    result = cursor.fetchall()

    if result:
        print("\nChaves Estrangeiras:\n")
    for r in result:
        print("  * %s: referencia //%s(%s)//" % (r[0], r[1], r[2]))
        print("    * ON UPDATE %s ON DELETE %s" % (r[3], r[4]))
    print("\n-------\n")

if __name__ == '__main__':
    database_model()