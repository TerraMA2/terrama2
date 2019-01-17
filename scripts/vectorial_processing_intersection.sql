--SELECT vectorial_processing(MONITORED_DATASERIES, LIST<ADDIONAL_DS>, OPERATION_TYPE, [WHERE_CONDITION]);


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
LANGUAGE 'plpgsql'

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


CREATE OR REPLACE FUNCTION retrieve_intersection(monitored_dataseries VARCHAR,
                                                 monitored_geometry_column VARCHAR,
                                                 additional_dataseries VARCHAR,
                                                 additional_dataseries_geometry_column VARCHAR,
                                                 condition VARCHAR)
    RETURNS TABLE(monitored_id VARCHAR, additional_id VARCHAR, intersection_geom GEOMETRY) AS
$$
DECLARE
    query TEXT;
    query_helper TEXT;

    pk_monitored TEXT;
    pk_additional TEXT;
BEGIN
    query_helper := format('(%s.%s, %s.%s)', monitored_dataseries, monitored_geometry_column, additional_dataseries, additional_dataseries_geometry_column);

    EXECUTE 'SELECT get_primary_key($1)' INTO pk_monitored USING monitored_dataseries;
    EXECUTE 'SELECT get_primary_key($1)' INTO pk_additional USING additional_dataseries;

    query := format('SELECT %s.%s::VARCHAR as monitored_id, %s.%s::VARCHAR AS additional_id, ST_Intersection' || query_helper ||' FROM %s, %s', monitored_dataseries, pk_monitored, additional_dataseries, pk_additional, monitored_dataseries, additional_dataseries);
    query := format(query || ' WHERE ST_Intersects'|| query_helper ||' AND ', monitored_geometry_column, additional_dataseries_geometry_column);
    query := query || condition;

    RAISE NOTICE 'SQL %', query;
    RETURN QUERY EXECUTE query USING monitored_dataseries, additional_dataseries;
END;
$$
LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION vectorial_processing_intersection(monitored_dataseries VARCHAR,
                                                             additional_dataseries_list VARCHAR[],
                                                             condition VARCHAR)
    RETURNS text AS
$$
DECLARE
    additional_dataseries VARCHAR;
    additional_dataseries_geometry_column VARCHAR;
    monitored_geometry_column VARCHAR;
    temporary_table VARCHAR;
    query text;
    query_helper text;
    result RECORD;
    row_data RECORD;

    monitored_id VARCHAR;
    additional_id VARCHAR;
    intersection_geom GEOMETRY;
BEGIN
    query_helper := '';

    -- Retrieves Geometry Column Name from Monitored Data Series
    EXECUTE 'SELECT get_geometry_column($1)' INTO monitored_geometry_column USING monitored_dataseries;

    IF (array_length(additional_dataseries_list, 1) = 0) THEN
        RAISE EXCEPTION 'Invalid additional data series list argument.';
    END IF;

    additional_dataseries := additional_dataseries_list[1];

    -- Retrieves Geometry column name of current additional data series
    EXECUTE 'SELECT get_geometry_column($1)' INTO additional_dataseries_geometry_column USING additional_dataseries;

    -- query_helper := format('(%s.%s, %s.%s)', monitored_dataseries, monitored_geometry_column, additional_dataseries, additional_dataseries_geometry_column);

    -- RAISE NOTICE 'Checking intersection with % AND %', monitored_dataseries, additional_dataseries;

    -- query := format('SELECT get_primary_key($1), get_primary_key($2), ST_Intersection' || query_helper ||' FROM %s, %s', monitored_dataseries, additional_dataseries);
    -- query := format(query || ' WHERE ST_Intersects'|| query_helper ||' AND ', monitored_geometry_column, additional_dataseries_geometry_column);
    -- query := query || condition;

    -- EXECUTE query INTO result USING monitored_dataseries, additional_dataseries;
    temporary_table := format('%s_%s', monitored_dataseries, additional_dataseries);
    EXECUTE format('CREATE TABLE %s ( monitored_id VARCHAR, additional_id VARCHAR, intersection_geom GEOMETRY(MULTIPOLYGON, 4674) )', temporary_table);


    FOR result in EXECUTE 'SELECT * FROM retrieve_intersection($1, $2, $3, $4, ''1=1'')' USING monitored_dataseries, monitored_geometry_column, additional_dataseries, additional_dataseries_geometry_column LOOP
        EXECUTE format('INSERT INTO %s VALUES ($1, $2, ST_Multi($3))', temporary_table) USING result.monitored_id, result.additional_id, result.intersection_geom;
    END LOOP;

    -- IF result IS NULL THEN
    --     RAISE EXCEPTION 'No intersection';
    -- END IF;
    -- RAISE NOTICE 'Ok. Got intersection. Creating temporary table %', temporary_table;



    -- FOR row_data IN SELECT d FROM unnest(result)
    -- LOOP
    --     EXECUTE format('INSERT INTO %s VALUES ($1, $2, ST_GeomFromWKT(%3))', temporary_table) USING monitored_id, additional_id, intersection_geom;
    -- END LOOP;

    FOREACH additional_dataseries IN ARRAY additional_dataseries_list
    LOOP
        RAISE NOTICE '%', additional_dataseries;

        -- Retrieves Geometry column name of current additional data series
        EXECUTE 'SELECT get_geometry_column($1)' INTO additional_dataseries_geometry_column USING additional_dataseries;

        query_helper := format('(%s.%s, %s.%s)', monitored_dataseries, monitored_geometry_column, additional_dataseries, additional_dataseries_geometry_column);

        query := format('SELECT ST_Intersection' || query_helper ||' FROM %s, %s', monitored_dataseries, additional_dataseries);
        query := format(query || ' WHERE ST_Intersects'|| query_helper ||' AND ', monitored_geometry_column, additional_dataseries_geometry_column);
        query := query || condition;

        RAISE NOTICE 'Query %', query;

        -- EXECUTE 'ALTER TABLE temporary_table ADD COLUMN '

        EXECUTE query INTO result;

        -- IF EXISTS result THEN
        --     RAISE NOTICE 'Tem interseção dos atributos %', additional_dataseries;
        -- END IF;

    END LOOP;

    RETURN temporary_table;
END;
$$
LANGUAGE 'plpgsql'
