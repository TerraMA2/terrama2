CREATE OR REPLACE FUNCTION dcp_last_measures(dcp_name text,
                                             var_name text)
  RETURNS SETOF record AS
$BODY$
DECLARE
    in_tuple RECORD;
    mid_tuple RECORD;
    out_tuple RECORD;
    table_name TEXT;
BEGIN
    FOR in_tuple IN EXECUTE format('SELECT * FROM %I', dcp_name) LOOP

        table_name := in_tuple.table_name;

        EXECUTE 'SELECT datetime, '|| var_name ||' as var FROM ' || table_name || ' ORDER BY datetime DESC LIMIT 1'
		      INTO mid_tuple;

        IF mid_tuple.datetime IS NOT NULL THEN
          out_tuple := (in_tuple.id, in_tuple.geom, mid_tuple.datetime, mid_tuple.var);
          RETURN NEXT out_tuple;
        END IF;
    END LOOP;

    RETURN;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100
  ROWS 1000;
ALTER FUNCTION dcp_last_measures(text, text)
  OWNER TO postgres;
