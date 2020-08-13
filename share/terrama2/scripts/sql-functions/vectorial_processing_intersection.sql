--
-- This file describes the Vector Processing funcionality of TerraMA2.
-- In order to do that, it must have:
-- - The dynamic data series table must TIMESTAMP temporal field type;
-- - Both static and dynamic data series table must have a valid primary key;

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
    number_of_rows INTEGER;
    automatic_schedule_id_selected INTEGER;
BEGIN

IF array_length(list_id, 1) > 0 THEN

    FOR indice IN 1 .. array_upper(list_id, 1)
    LOOP
        -- Check if the attribute from first iteration exists
        EXECUTE format('SELECT COUNT(*) FROM terrama2.analysis_metadata WHERE key = ''dynamicDataSeries'' AND analysis_id = %s', CAST(list_id[indice] AS INTEGER)) INTO number_of_rows;

        IF number_of_rows > 0 THEN
            EXECUTE format('SELECT value FROM terrama2.analysis_metadata
                    WHERE key = ''dynamicDataSeries'' AND analysis_id = %s', CAST(list_id[indice] AS INTEGER) ) INTO id_analysis;

            EXECUTE format('SELECT automatic_schedule_id FROM terrama2.analysis
                        WHERE automatic_schedule_id is not null AND id = %s', CAST(list_id[indice] AS INTEGER) ) INTO automatic_schedule_id_selected;

            UPDATE terrama2.automatic_schedules
                SET data_ids = ARRAY[id_analysis]
                WHERE id = automatic_schedule_id_selected;
        END IF;       

    END LOOP;

ELSE

    RETURN FALSE;

END IF;

RETURN TRUE;

END;
$$
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_date_column_from_dynamic_table(table_name VARCHAR)
    RETURNS VARCHAR AS
$$
DECLARE
    query TEXT;
    result RECORD;
BEGIN
    query := format('SELECT column_name FROM information_schema.columns WHERE table_name = ''%s'' AND data_Type IN (''timestamp with time zone'', ''timestamp without time zone'')', table_name);

    EXECUTE query INTO result USING table_name;

    RETURN result.column_name;
END;
$$
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION update_attributes_from_analysis(column_attributes VARCHAR)
	RETURNS TEXT AS
$$
DECLARE
    attributes_updated TEXT;
	current_attribute TEXT;
	column_attributes_array TEXT[];
	column_attributes_array_handler TEXT[];
BEGIN

	EXECUTE format('SELECT string_to_array(''%s'', '','')', column_attributes) INTO column_attributes_array;

	FOR indice IN 1 .. array_upper(column_attributes_array, 1)

	LOOP

	   EXECUTE format('SELECT split_part(''%s'', ''AS'', 2)', column_attributes_array[indice]) INTO current_attribute;

	   column_attributes_array_handler:= array_append(column_attributes_array_handler, current_attribute);

	END LOOP;

	EXECUTE format('SELECT array_to_string(''%s''::TEXT[], '',''::TEXT)', column_attributes_array_handler) INTO attributes_updated;

    RETURN attributes_updated;
END;
$$
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION vectorial_processing_intersection(analysis_id INTEGER,
                                                             output_table_name VARCHAR,
                                                             static_table_name VARCHAR,
                                                             dynamic_table_name VARCHAR,
                                                             output_attributes VARCHAR,
                                                             date_filter VARCHAR,
                                                             class_name_filter VARCHAR,
                                                             attribute_fields VARCHAR)
    RETURNS TABLE(table_name VARCHAR, affected_rows BIGINT) AS
$$
DECLARE
    static_table_name_column VARCHAR;
    dynamic_table_name_column VARCHAR;
    dynamic_table_name_handler VARCHAR;

    dynamic_table_srid INTEGER;
    number_of_rows INTEGER;
    number_of_rows_metadata INTEGER;

	is_table_exists BOOLEAN;
	affected_rows BIGINT;

    pk_static_table TEXT;
    pk_dynamic_table TEXT;
    start_date_filter TEXT;
    end_date_filter TEXT;

	final_table VARCHAR;
    date_column_from_dynamic_table VARCHAR;
    last_analysis VARCHAR;
    analysis_filter VARCHAR;
	output_attributes_updated VARCHAR;

	new_analysis_id INTEGER;
	number_of_columns INTEGER;
    number_of_attributes INTEGER;
	number_of_affected_rows BIGINT;

    finaltable_last_id_inserted INTEGER;
    is_dynamictable_info_exists BOOLEAN;
    pk_output_table TEXT;

    query TEXT;
    result RECORD;
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

    EXECUTE format('SELECT ST_SRID(%s) FROM %s LIMIT 1', dynamic_table_name_column, dynamic_table_name_handler) INTO dynamic_table_srid;

    -- Getting date_column_from_dynamic_table
    EXECUTE format('SELECT get_date_column_from_dynamic_table(''%s'')', dynamic_table_name_handler) INTO date_column_from_dynamic_table;

    final_table := format('%s_%s', output_table_name, analysis_id);

    EXECUTE format('SELECT EXISTS (SELECT 1 FROM information_schema.tables WHERE  table_schema = ''public'' AND table_name = ''%s'')', output_table_name) INTO is_dynamictable_info_exists;

    IF (is_dynamictable_info_exists) THEN
        EXECUTE 'SELECT get_primary_key($1)' INTO pk_output_table USING output_table_name;
        EXECUTE format('SELECT last_id FROM (SELECT * FROM public.%s ORDER BY %s DESC LIMIT 1) AS last_data',output_table_name,pk_output_table) INTO finaltable_last_id_inserted;
    ELSE
        finaltable_last_id_inserted := 0;
    END IF;

    EXECUTE 'SELECT get_primary_key($1)' INTO pk_static_table USING static_table_name;
    EXECUTE 'SELECT get_primary_key($1)' INTO pk_dynamic_table USING dynamic_table_name_handler;

	EXECUTE format('SELECT EXISTS (SELECT 1 FROM information_schema.tables WHERE  table_schema = ''public'' AND table_name = ''%s'')', final_table) INTO is_table_exists;

    EXECUTE format('SELECT COUNT(*) FROM information_schema.columns WHERE table_name = ''%s'' ', final_table) INTO number_of_columns;

    EXECUTE format('SELECT REPLACE (''%s'', ''"'', '''' ) ', output_attributes) INTO output_attributes;

    EXECUTE format('SELECT array_length(string_to_array(''%s'', '',''), 1) ', output_attributes) INTO number_of_attributes;

    number_of_attributes := number_of_attributes + 6;
    -- 6 this is the default number of attributes: mo   nitored_id, intersect_id, execution_date, intersection_geom, calculated_area_ha, final_table_id;

    IF (is_table_exists AND number_of_columns = number_of_attributes) THEN
        analysis_filter := format(' %s.%s  >  %s ', dynamic_table_name_handler, pk_dynamic_table, finaltable_last_id_inserted);

        date_filter := '1 = 1';

		    EXECUTE format('SELECT update_attributes_from_analysis(''%s'')', output_attributes) INTO output_attributes_updated;

        query := format('INSERT INTO %s (monitored_id, intersect_id, execution_date, intersection_geom, calculated_area_ha, %s)
                              SELECT monitored_id,
                                     intersect_id,
                                     execution_date,
                                     CASE WHEN ST_GeometryType(intersection_geom) = ''ST_Polygon'' THEN
                                       ST_Multi(intersection_geom)
                                     ELSE intersection_geom
                                     END AS intersection_geom,
                                     ST_AREA(intersection_geom::GEOGRAPHY) / 10000 AS calculated_area_ha,
                                     %s
                                FROM (
                                  SELECT %s.%s::VARCHAR AS monitored_id,
                                         %s.%s::VARCHAR AS intersect_id,
                                         %s.%s::TIMESTAMPTZ AS execution_date,
                                         ST_Intersection(ST_Transform(%s.%s, %s), %s.%s) AS intersection_geom,
                                         ST_GeometryType(%s.%s) AS table_left_geometry_type,
                                         ST_GeometryType(%s.%s) AS table_right_geometry_type,
                                         %s
                                    FROM %s, %s
                                   WHERE ST_Intersects(ST_Transform(%s.%s, %s), %s.%s)
                                         AND %s
                                         AND %s
                                         AND (%s)
                                ) res
                                WHERE CASE WHEN ST_GeometryType(intersection_geom) = ''ST_Polygon'' OR ST_GeometryType(intersection_geom) = ''ST_MultiPolygon'' THEN true
                                           WHEN table_left_geometry_type <> ''ST_Point'' AND table_right_geometry_type = ''ST_Point'' THEN true
                                      ELSE false
                                      END;
                    ',
                    final_table, output_attributes_updated,
                    attribute_fields,
                    static_table_name, pk_static_table,
                    dynamic_table_name_handler, pk_dynamic_table,
                    dynamic_table_name_handler, date_column_from_dynamic_table,
                    static_table_name, static_table_name_column, dynamic_table_srid, dynamic_table_name_handler, dynamic_table_name_column,
                    static_table_name, static_table_name_column,
                    dynamic_table_name_handler, dynamic_table_name_column,
                    output_attributes,
                    dynamic_table_name_handler, static_table_name,
                    static_table_name, static_table_name_column, dynamic_table_srid, dynamic_table_name_handler, dynamic_table_name_column,
                    analysis_filter, date_filter, class_name_filter);
        
        RAISE NOTICE 'Query %', query;
        EXECUTE query;

        GET DIAGNOSTICS number_of_affected_rows = ROW_COUNT;
    ELSE

        analysis_filter := format(' %s.%s  >  %s ', dynamic_table_name_handler, pk_dynamic_table, finaltable_last_id_inserted);

        date_filter := '1 = 1';

        query := format('
            DROP TABLE IF EXISTS %s;
            CREATE TABLE %s AS
              SELECT monitored_id,
                     intersect_id,
                     execution_date,
                     CASE WHEN ST_GeometryType(intersection_geom) = ''ST_Polygon'' THEN
                       ST_Multi(intersection_geom)
                     ELSE intersection_geom
                     END AS intersection_geom,
                     ST_AREA(intersection_geom::GEOGRAPHY) / 10000 AS calculated_area_ha,
                     %s
                FROM (
                  SELECT %s.%s::VARCHAR AS monitored_id,
                         %s.%s::VARCHAR AS intersect_id,
                         %s.%s::TIMESTAMPTZ AS execution_date,
                         ST_Intersection(ST_Transform(%s.%s, %s), %s.%s) AS intersection_geom,
                         ST_GeometryType(%s.%s) AS table_left_geometry_type,
                         ST_GeometryType(%s.%s) AS table_right_geometry_type,
                         %s
                    FROM %s, %s
                   WHERE ST_Intersects(ST_Transform(%s.%s, %s), %s.%s)
                     AND %s
                     AND %s
                     AND (%s)
                ) res
                WHERE CASE WHEN ST_GeometryType(intersection_geom) = ''ST_Polygon'' OR ST_GeometryType(intersection_geom) = ''ST_MultiPolygon'' THEN true
                           WHEN table_left_geometry_type <> ''ST_Point'' AND table_right_geometry_type = ''ST_Point'' THEN true
                      ELSE false
                      END;
                    ',
                    final_table,
                    final_table,
                    attribute_fields,
                    static_table_name, pk_static_table,
                    dynamic_table_name_handler, pk_dynamic_table,
                    dynamic_table_name_handler, date_column_from_dynamic_table,
                    static_table_name, static_table_name_column, dynamic_table_srid, dynamic_table_name_handler, dynamic_table_name_column,
                    static_table_name, static_table_name_column,
                    dynamic_table_name_handler, dynamic_table_name_column,
                    output_attributes,
                    dynamic_table_name_handler, static_table_name,
                    static_table_name, static_table_name_column, dynamic_table_srid, dynamic_table_name_handler, dynamic_table_name_column,
                    analysis_filter, date_filter, class_name_filter);

        EXECUTE query;

        RAISE NOTICE 'Query Create %', query;

        -- Getting affected_rows
        GET DIAGNOSTICS number_of_affected_rows = ROW_COUNT;

        query := format('
            CREATE INDEX %s_geom_idx ON %s USING GIST(intersection_geom);
            CREATE INDEX %s_static_table_id_dynamic_table_id_idx ON %s USING BTREE(monitored_id, intersect_id);
        ', final_table, final_table,
            final_table, final_table);

        EXECUTE query;

        query := format('ALTER TABLE %s ADD COLUMN %s_id SERIAL PRIMARY KEY', final_table, final_table);

        EXECUTE query;
    END IF;

	query := format('ALTER TABLE %s ALTER COLUMN intersection_geom TYPE geometry(GEOMETRY, %s)
				USING ST_Transform(ST_SetSRID(intersection_geom,%s), %s)',final_table, dynamic_table_srid, dynamic_table_srid, dynamic_table_srid);
	EXECUTE query;

    EXECUTE 'SELECT get_id_from_first_analysis(get_automatic_schedule_id_list())';

    -- Getting last time collected from dynamic table
    EXECUTE format('SELECT MAX(%s.%s) FROM %s', dynamic_table_name_handler, date_column_from_dynamic_table, dynamic_table_name_handler) INTO last_analysis;

    EXECUTE format('INSERT INTO terrama2.analysis_metadata(key, value, analysis_id) VALUES (''last_analysis'', ''%s'', %s)', last_analysis, analysis_id);

    query := format('SELECT $1 as table_name, %s::BIGINT as affected_rows FROM %s LIMIT 1',number_of_affected_rows, final_table);

    RETURN QUERY EXECUTE query USING final_table;

END;
$$
LANGUAGE 'plpgsql';