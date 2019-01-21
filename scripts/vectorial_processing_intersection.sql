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

    monitored_str TEXT;
    additional_str TEXT;

    pk_monitored TEXT;
    pk_additional TEXT;
BEGIN

    monitored_str := format('%s.%s', monitored_dataseries, monitored_geometry_column);
    additional_str := format('%s.%s', additional_dataseries, additional_dataseries_geometry_column);

    query_helper := format('(%s, %s)', monitored_str, additional_str);

    EXECUTE 'SELECT get_primary_key($1)' INTO pk_monitored USING monitored_dataseries;
    EXECUTE 'SELECT get_primary_key($1)' INTO pk_additional USING additional_dataseries;

    query := format('SELECT %s.%s::VARCHAR as monitored_id, %s.%s::VARCHAR AS additional_id, ST_Intersection' || query_helper ||' FROM %s, %s', monitored_dataseries, pk_monitored, additional_dataseries, pk_additional, monitored_dataseries, additional_dataseries);
    query := format(query || ' WHERE ST_Intersects'|| query_helper ||' AND ', monitored_str, additional_str);
    query := query || condition;

    -- RAISE NOTICE 'SQL %', query;
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
    additional_dataseries_pk VARCHAR;
    monitored_geometry_column VARCHAR;
    temporary_table VARCHAR;
    query text;
    result RECORD;
    row_data RECORD;
BEGIN
    -- Retrieves Geometry Column Name from Monitored Data Series
    EXECUTE 'SELECT get_geometry_column($1)' INTO monitored_geometry_column USING monitored_dataseries;

    IF (array_length(additional_dataseries_list, 1) = 0) THEN
        RAISE EXCEPTION 'Invalid additional data series list argument.';
    END IF;

    additional_dataseries := additional_dataseries_list[1];

    -- Retrieves Geometry column name of current additional data series
    EXECUTE 'SELECT get_geometry_column($1)' INTO additional_dataseries_geometry_column USING additional_dataseries;

    temporary_table := format('%s_%s', monitored_dataseries, additional_dataseries);
    EXECUTE format('CREATE TABLE IF NOT EXISTS %s ( id SERIAL PRIMARY KEY, monitored_id VARCHAR, additional_id VARCHAR, intersection_geom GEOMETRY(MULTIPOLYGON, 4674) )', temporary_table);

    FOR result in EXECUTE 'SELECT * FROM retrieve_intersection($1, $2, $3, $4, ''1=1'')' USING monitored_dataseries, monitored_geometry_column, additional_dataseries, additional_dataseries_geometry_column LOOP
        EXECUTE 'SELECT ST_GeometryType($1) AS geom' INTO row_data USING result.intersection_geom ;

        IF row_data.geom = 'ST_MultiPolygon' OR row_data.geom = 'ST_Polygon' THEN
            EXECUTE format('INSERT INTO %s (monitored_id, additional_id, intersection_geom) VALUES ($1, $2, ST_Multi($3))', temporary_table) USING result.monitored_id, result.additional_id, result.intersection_geom;
        ELSE
            RAISE NOTICE 'Warning - Got %s in intersection. Skipping', row_data.geom;
        END IF;
    END LOOP;

    FOR i IN 2..array_length(additional_dataseries_list, 1)
    LOOP
        additional_dataseries = additional_dataseries_list[i];

        -- Retrieves Geometry column name of current additional data series
        EXECUTE 'SELECT get_geometry_column($1) as geom, get_primary_key($1) as pk' INTO row_data USING additional_dataseries;

        additional_dataseries_geometry_column := row_data.geom;
        additional_dataseries_pk := row_data.pk;

        EXECUTE format('CREATE TABLE IF NOT EXISTS %s_%s (
                            id SERIAL PRIMARY KEY,
                            monitored_id VARCHAR,
                            additional_id VARCHAR,
                            intersection_geom GEOMETRY(MultiPolygon, 4674)
                        )', monitored_dataseries, additional_dataseries);

        EXECUTE format('WITH dumped AS (
                            SELECT %s as id, (ST_Dump(%s)).geom as geom FROM %s
                        )
                        INSERT INTO %s_%s (monitored_id, additional_id, intersection_geom)
                            SELECT t.monitored_id, d.id as additional_id, ST_Multi(ST_Intersection(t.intersection_geom, d.geom)) as intersection_geom FROM %s t, dumped d
                             WHERE ST_Intersects(t.intersection_geom, d.geom) AND (ST_GeometryType(ST_Intersection(t.intersection_geom, d.geom)) = ''ST_MultiPolygon'' OR
                                                                                   ST_GeometryType(ST_Intersection(t.intersection_geom, d.geom)) = ''ST_Polygon'')',
                        additional_dataseries_pk, additional_dataseries_geometry_column, additional_dataseries,
                        monitored_dataseries, additional_dataseries,
                        temporary_table);
    END LOOP;

    RETURN temporary_table;
END;
$$
LANGUAGE 'plpgsql';

DROP TABLE IF EXISTS car_area_imovel_alertas_mpmt;
 SELECT vectorial_processing_intersection('car_area_imovel', ARRAY['alertas_mpmt', 'app', 'reserva_legal', 'area_topo_morro', 'uso_restrito'], '1=1');
