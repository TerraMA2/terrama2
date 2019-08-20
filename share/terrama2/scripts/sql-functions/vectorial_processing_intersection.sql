CREATE OR REPLACE FUNCTION get_geometry_column(table_name VARCHAR)
    RETURNS VARCHAR AS
$$
DECLARE
    query TEXT;
    result RECORD;
BEGIN
    query := 'SELECT f_geometry_column::VARCHAR as column_name FROM geometry_columns WHERE f_table_name = $1 LIMIT 1';

    EXECUTE query INTO result USING table_name;

    RETURN result.column_name;
END;
$$
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_primary_key(table_name VARCHAR)
    RETURNS VARCHAR AS
$$
DECLARE
    query TEXT;
    result RECORD;
BEGIN
    query := 'SELECT a.attname as column_name FROM pg_index i';
    query := query || '  JOIN pg_attribute a';
    query := query || '    ON a.attrelid = i.indrelid';
    query := query || '   AND a.attnum = ANY(i.indkey)';
    query := query || ' WHERE i.indrelid = $1::regclass';
    query := query || '   AND i.indisprimary';

    EXECUTE query INTO result USING table_name;

    RETURN result.column_name;
END;
$$
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_automatic_schedule_id_list()
    RETURNS text[] AS
$$
  SELECT ARRAY(SELECT id::text FROM terrama2.analysis WHERE automatic_schedule_id is not null)
$$
LANGUAGE SQL IMMUTABLE;

CREATE OR REPLACE FUNCTION get_id_from_first_analysis(list_id text[])
    RETURNS BOOLEAN AS
$$
DECLARE
    indice INTEGER;
    id_analysis INTEGER;
    automatic_schedule_id_selected INTEGER;
BEGIN

IF array_length(list_id, 1) > 0 THEN

    FOR indice IN 1 .. array_upper(list_id, 1)
    LOOP
        EXECUTE format('SELECT value FROM terrama2.analysis_metadata
                    WHERE key = ''dynamicDataSeries'' AND analysis_id = %s', CAST(list_id[indice] AS INTEGER) ) INTO id_analysis;

        EXECUTE format('SELECT automatic_schedule_id FROM terrama2.analysis
                    WHERE automatic_schedule_id is not null AND id = %s', CAST(list_id[indice] AS INTEGER) ) INTO automatic_schedule_id_selected;

        UPDATE terrama2.automatic_schedules
            SET data_ids = ARRAY[id_analysis]
            WHERE id = automatic_schedule_id_selected AND array_length(data_ids, 1) = 0;

    END LOOP;

ELSE

    RETURN FALSE;

END IF;

RETURN TRUE;

END;
$$
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION vectorial_processing_intersection(analysis_id INTEGER,
                                                             output_table_name VARCHAR,
                                                             static_table_name VARCHAR,
                                                             dynamic_table_name VARCHAR,
                                                             output_attributes VARCHAR,
                                                             date_filter VARCHAR)
    RETURNS TABLE(table_name VARCHAR, affected_rows BIGINT) AS
$$
DECLARE
    static_table_name_column VARCHAR;
    dynamic_table_name_column VARCHAR;
    dynamic_table_name_handler VARCHAR;

    affected_rows BIGINT;
    static_table_srid INTEGER;
    number_of_rows INTEGER;

    pk_static_table TEXT;
    pk_dynamic_table TEXT;

    query TEXT;
    result RECORD;
    final_table VARCHAR;
BEGIN
    -- Retrieves Geometry Column Name from Monitored Data Series
    EXECUTE 'SELECT get_geometry_column($1)' INTO static_table_name_column USING static_table_name;

    -- Check if the attribute from first iteration exists
    EXECUTE format('SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = ''%s'' AND column_name = ''table_name''',dynamic_table_name) INTO number_of_rows;

    IF number_of_rows > 0 THEN
        EXECUTE format('SELECT table_name FROM %s', dynamic_table_name) INTO dynamic_table_name_handler;
    ELSE
        dynamic_table_name_handler := dynamic_table_name;
    END IF;

    -- Retrieves Geometry Column Name from Intersect Data Series
    EXECUTE 'SELECT get_geometry_column($1)' INTO dynamic_table_name_column USING dynamic_table_name_handler;

    EXECUTE format('SELECT ST_SRID(%s) FROM %s LIMIT 1', static_table_name_column, static_table_name) INTO static_table_srid;

    IF static_table_srid <= 0 THEN
        RAISE NOTICE 'The table "%" has no SRID "%". Using "4326" as default.', static_table_name, static_table_srid;

        static_table_srid := 4326;
    END IF;

    -- Check date_filter. When empty, set 1 = 1 as default
    IF (coalesce(date_filter, '') = '') THEN
        date_filter := '1 = 1';
    END IF;

    final_table := format('%s_%s', output_table_name, analysis_id);

    EXECUTE 'SELECT get_primary_key($1)' INTO pk_static_table USING static_table_name;
    EXECUTE 'SELECT get_primary_key($1)' INTO pk_dynamic_table USING dynamic_table_name_handler;

    query := format('
        DROP TABLE IF EXISTS %s;
        CREATE TABLE %s AS
                SELECT  %s.%s::VARCHAR AS monitored_id,
                        %s.%s::VARCHAR AS intersect_id,
                        ST_Intersection(%s.%s, ST_Transform(%s.%s, %s)) AS intersection_geom,
                        ST_AREA(ST_Intersection(%s.%s, ST_Transform(%s.%s, %s))) / 10000 AS calculated_area_ha,
                        %s
                  FROM %s, %s
                 WHERE ST_Intersects(%s.%s, ST_Transform(%s.%s, %s)) AND %s
            ',
            final_table,
            final_table,
            static_table_name, pk_static_table,
            dynamic_table_name_handler, pk_dynamic_table,

                -- SELECT  %s.%s::VARCHAR AS monitored_id,
                --         %s.%s::VARCHAR AS intersect_id,
                --         %s.%s AS monitored_geom,
            -- static_table_name, static_table_name_column,

            static_table_name, static_table_name_column, dynamic_table_name_handler, dynamic_table_name_column, static_table_srid,
            static_table_name, static_table_name_column, dynamic_table_name_handler, dynamic_table_name_column, static_table_srid,
            output_attributes,
            dynamic_table_name_handler, static_table_name,
            static_table_name, static_table_name_column, dynamic_table_name_handler, dynamic_table_name_column, static_table_srid,
            date_filter);

    EXECUTE query;

    query := format('
        CREATE INDEX %s_geom_idx ON %s USING GIST(intersection_geom);
        CREATE INDEX %s_static_table_id_dynamic_table_id_idx ON %s USING BTREE(monitored_id, intersect_id);
    ', final_table, final_table,
       final_table, final_table);

    EXECUTE query;

    query := format('ALTER TABLE %s ADD COLUMN %s_id SERIAL PRIMARY KEY', final_table, final_table);

    EXECUTE query;

    EXECUTE 'SELECT get_id_from_first_analysis(get_automatic_schedule_id_list())';

    query := format('SELECT $1 as table_name, count(*) as affected_rows FROM %s', final_table);

    RETURN QUERY EXECUTE query USING final_table;

END;
$$
LANGUAGE 'plpgsql';