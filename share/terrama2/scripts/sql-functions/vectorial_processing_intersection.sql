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

CREATE OR REPLACE FUNCTION vectorial_processing_intersection(analysis_id INTEGER,
                                                             monitored_dataseries VARCHAR,
                                                             intersect_dataseries VARCHAR,
                                                             additional_dataseries_list VARCHAR[],
                                                             condition VARCHAR)
    RETURNS TABLE(table_name VARCHAR, affected_rows BIGINT) AS
$$
DECLARE
    additional_dataseries VARCHAR;
    additional_dataseries_geometry_column VARCHAR;
    affected_rows BIGINT;

    fields_name TEXT;
    join_query TEXT;

    intersect_geometry_column VARCHAR;
    monitored_geometry_column VARCHAR;
    monitored_srid INTEGER;

    pk_monitored TEXT;
    pk_intersect TEXT;
    pk_additional TEXT;

    query TEXT;
    result RECORD;
    final_table VARCHAR;
    intersection_table VARCHAR;
BEGIN
    -- Retrieves Geometry Column Name from Monitored Data Series
    EXECUTE 'SELECT get_geometry_column($1)' INTO monitored_geometry_column USING monitored_dataseries;

    -- Retrieves Geometry Column Name from Intersect Data Series
    EXECUTE 'SELECT get_geometry_column($1)' INTO intersect_geometry_column USING intersect_dataseries;

    EXECUTE format('SELECT ST_SRID(%s) FROM %s LIMIT 1', monitored_geometry_column, monitored_dataseries) INTO monitored_srid;

    IF monitored_srid <= 0 THEN
        RAISE NOTICE 'The table "%" has no SRID "%". Using "4326" as default.', monitored_dataseries, monitored_srid;

        monitored_srid := 4326;
    END IF;

    -- Check condition. When empty, set 1 = 1 as default
    IF (coalesce(condition, '') = '') THEN
        condition := '1 = 1';
    END IF;

    final_table := format('%s_%s_%s', monitored_dataseries, intersect_dataseries, analysis_id);
    intersection_table := format('%s_%s', monitored_dataseries, intersect_dataseries);

    EXECUTE 'SELECT get_primary_key($1)' INTO pk_monitored USING monitored_dataseries;
    EXECUTE 'SELECT get_primary_key($1)' INTO pk_intersect USING intersect_dataseries;

    query := format('
        DROP TABLE IF EXISTS %s;
        CREATE TABLE %s AS
            WITH %s AS (
                SELECT  %s.%s::VARCHAR AS monitored_id,
                        %s.%s::VARCHAR AS intersect_id,
                        %s.%s AS monitored_geom,
                        ST_Intersection(%s.%s, ST_Transform(%s.%s, %s)) AS intersection_geom
                  FROM %s, %s
                 WHERE ST_Intersects(%s.%s, ST_Transform(%s.%s, %s)) AND
                        %s
            )',
            final_table,
            final_table,
            intersection_table,
            monitored_dataseries, pk_monitored,
            intersect_dataseries, pk_intersect,
            monitored_dataseries, monitored_geometry_column,
            monitored_dataseries, monitored_geometry_column, intersect_dataseries, intersect_geometry_column, monitored_srid,
            monitored_dataseries, intersect_dataseries,
            monitored_dataseries, monitored_geometry_column, intersect_dataseries, intersect_geometry_column, monitored_srid,
            condition);

    join_query := '';
    fields_name := format('%s.monitored_id,
                           %s.intersect_id,
                           %s.monitored_geom,
                           %s.intersection_geom',
                           intersection_table, intersection_table, intersection_table, intersection_table, intersection_table, intersection_table);

    FOR i IN 1..coalesce(array_length(additional_dataseries_list, 1), 0)
    LOOP
        additional_dataseries := additional_dataseries_list[i];

        -- Retrieves Primary Key from Additional Data Series
        EXECUTE 'SELECT get_primary_key($1)' INTO pk_additional USING additional_dataseries;
        -- Retrieves Geometry Column Name from Additional Data Series
        EXECUTE 'SELECT get_geometry_column($1)' INTO additional_dataseries_geometry_column USING additional_dataseries;

        fields_name := fields_name || format(', %s_%s.%s AS %s_pk,
                %s_%s.%s_geom,
                %s_%s.%s_intersection_geom,
                %s_%s.%s_difference_geom
            ',
            monitored_dataseries, additional_dataseries, pk_additional, additional_dataseries,
            monitored_dataseries, additional_dataseries, additional_dataseries,
            monitored_dataseries, additional_dataseries, additional_dataseries,
            monitored_dataseries, additional_dataseries, additional_dataseries);

        join_query := join_query || format('
            LEFT JOIN %s_%s
              ON %s.monitored_id = %s_%s.monitored_id
             AND %s.intersect_id = %s_%s.intersect_id
        ', monitored_dataseries, additional_dataseries,
           intersection_table, monitored_dataseries, additional_dataseries,
           intersection_table, monitored_dataseries, additional_dataseries);

        query := query || format('
            , %s_%s AS (
                SELECT *,
                       ST_Difference(%s_geom, (SELECT ST_Union(intersection_geom) FROM %s))
                       AS %s_difference_geom
                FROM (
                    SELECT %s.monitored_id AS monitored_id, %s.intersect_id AS intersect_id,
                           %s.%s,
                           ST_Multi(ST_Intersection(%s.monitored_geom, %s.%s)) AS %s_geom,
                           ST_Multi(ST_Intersection(%s.intersection_geom, %s.%s)) AS %s_intersection_geom
                      FROM %s
                      LEFT JOIN %s
                        ON ST_Intersects(%s.monitored_geom, %s.%s)
                ) AS tbl
            )',
            monitored_dataseries, additional_dataseries,
            additional_dataseries, intersection_table,
            additional_dataseries,
            intersection_table, intersection_table,
            additional_dataseries, pk_additional,
            intersection_table, additional_dataseries, additional_dataseries_geometry_column, additional_dataseries,
            intersection_table, additional_dataseries, additional_dataseries_geometry_column, additional_dataseries,
            intersection_table,
            additional_dataseries,
            intersection_table, additional_dataseries, additional_dataseries_geometry_column);
    END LOOP;

    query := query || format('
        SELECT %s
          FROM %s
               %s
    ', fields_name, intersection_table, join_query);

    EXECUTE query;

    query := format('
        CREATE INDEX %s_geom_idx ON %s USING GIST(intersection_geom);
        CREATE INDEX %s_monitored_id_intersect_id_idx ON %s USING BTREE(monitored_id, intersect_id);
    ', final_table, final_table,
       final_table, final_table);

    EXECUTE query;

    query := format('SELECT $1 as table_name, count(*) as affected_rows FROM %s', final_table);

    RETURN QUERY EXECUTE query USING final_table;

    -- RETURN QUERY EXECUTE format('SELECT * FROM %s', temporary_matched_table);
END;
$$
LANGUAGE 'plpgsql';

-- SELECT vectorial_processing_intersection(1, 'car_area_imovel', 'alertas_mpmt', ARRAY['app', 'reserva_legal', 'area_topo_morro', 'uso_restrito'], 'view_date::date > ''2018-01-01''');
