--SELECT vectorial_processing(MONITORED_DATASERIES, LIST<ADDIONAL_DS>, OPERATION_TYPE, [WHERE_CONDITION]);

CREATE OR REPLACE FUNCTION vectorial_processing_intersection(monitoredDataseries VARCHAR,
                                                             additionalDataSeriesList VARCHAR[],
                                                             condition VARCHAR)
    RETURNS text AS
$$
DECLARE
    strresult text;
    query text;
BEGIN
    strresult := '';
    
    FOREACH i IN ARRAY a
      LOOP 
        RAISE NOTICE '%', i;
        
        query := 'SELECT ST_Intersection(geom, geom2) FROM tbl1, tbl2 WHERE ST_Intersects(geom, geom2)';
        query := query || quote_ident(condition);
        --EXECUTE query
        
        IF result
      END LOOP;

    RETURN strresult;
END;
$$
LANGUAGE 'plpgsql'
