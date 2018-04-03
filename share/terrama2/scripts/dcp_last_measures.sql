-- Function: dcp_last_measures(text, text)

-- DROP FUNCTION dcp_last_measures(text, text);

CREATE OR REPLACE FUNCTION public.dcp_last_measures(dcp_name text, var_name text)
 RETURNS SETOF record
 LANGUAGE plpgsql
AS $function$
DECLARE
    in_tuple RECORD;
    mid_tuple RECORD;
    out_tuple RECORD;
    table_info RECORD;
	dcp_info RECORD;
    table_name TEXT;
    dcp_id TEXT;
BEGIN
    FOR in_tuple IN EXECUTE format('SELECT * FROM %I', dcp_name) LOOP

        table_name := in_tuple.table_name;
        dcp_id := in_tuple.id;

        EXECUTE 'SELECT column_name FROM information_schema.columns WHERE table_name=''' || table_name || ''' and column_name=''' || var_name || ''''
          INTO table_info;
        EXECUTE 'SELECT column_name FROM information_schema.columns WHERE table_name=''' || table_name || ''' and column_name=''dcp_id'''
          INTO dcp_info;

        IF table_info.column_name IS NULL THEN
          out_tuple := (in_tuple.id, in_tuple.geom, NULL::TIMESTAMPTZ, NULL::FLOAT);
          RETURN NEXT out_tuple;
        ELSIF dcp_info.column_name IS NOT NULL THEN
          EXECUTE 'SELECT datetime, '|| var_name ||' as var FROM ' || table_name || ' WHERE dcp_id=''' || dcp_id || ''' ORDER BY datetime DESC LIMIT 1'
                    INTO mid_tuple;
          IF mid_tuple.datetime IS NOT NULL THEN
            out_tuple := (in_tuple.id, in_tuple.geom, mid_tuple.datetime, mid_tuple.var);
            RETURN NEXT out_tuple;
          END IF;
        ELSE
          EXECUTE 'SELECT datetime, '|| var_name ||' as var FROM ' || table_name || ' ORDER BY datetime DESC LIMIT 1'
          		    INTO mid_tuple;

          IF mid_tuple.datetime IS NOT NULL THEN
            out_tuple := (in_tuple.id, in_tuple.geom, mid_tuple.datetime, mid_tuple.var);
            RETURN NEXT out_tuple;
          END IF;
        END IF;
    END LOOP;

    RETURN;
END;
$function$
