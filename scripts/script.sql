DROP TABLE IF EXISTS car_tmp;

CREATE TABLE car_tmp AS
WITH car_alerta AS (
  SELECT car_area_imovel.fid::VARCHAR AS monitored_id,
         alertas_mpmt.fid::VARCHAR AS alerta_id,
         car_area_imovel.ogr_geometry AS imovel_geom,
         ST_Intersection(car_area_imovel.ogr_geometry, ST_Transform(alertas_mpmt.ogr_geometry, 4674)) AS desmatamento_geom FROM car_area_imovel, alertas_mpmt
   WHERE ST_Intersects(car_area_imovel.ogr_geometry, ST_Transform(alertas_mpmt.ogr_geometry, 4674)) AND
         alertas_mpmt.view_date::date > '2018-01-01'
), car_reserva AS (
  SELECT  *,
          COALESCE(
            ST_Difference(imovel_reserva_legal_geom, desmatamento_reserva_legal_geom)
          ) AS desmatamento_reserva_legal_difference_geom
    FROM (
      SELECT  car_alerta.monitored_id AS imovel_fid,
              car_alerta.alerta_id AS alerta_fid,
              car_alerta.imovel_geom,
              car_alerta.desmatamento_geom,
              reserva_legal.fid AS reserva_legal_fid,
              ST_Multi(ST_Intersection(car_alerta.imovel_geom, reserva_legal.ogr_geometry)) AS imovel_reserva_legal_geom,
              ST_Multi(ST_Intersection(car_alerta.desmatamento_geom, reserva_legal.ogr_geometry)) AS desmatamento_reserva_legal_geom
        FROM  car_alerta
        LEFT JOIN reserva_legal
              ON ST_Intersects(car_alerta.imovel_geom, reserva_legal.ogr_geometry)
    ) AS tbl
), car_app AS (
  SELECT  *,
          COALESCE(
            ST_Difference(imovel_app_geom, desmatamento_app_geom)
          ) AS desmatamento_app_difference_geom
    FROM (
      SELECT  car_alerta.monitored_id AS imovel_fid,
              car_alerta.alerta_id AS alerta_fid,
              car_alerta.imovel_geom,
              car_alerta.desmatamento_geom,
              app.fid AS app_fid,
              ST_Multi(ST_Intersection(car_alerta.imovel_geom, app.ogr_geometry)) AS imovel_app_geom,
              ST_Multi(ST_Intersection(car_alerta.desmatamento_geom, app.ogr_geometry)) AS desmatamento_app_geom
        FROM  car_alerta
        LEFT JOIN app
              ON ST_Intersects(car_alerta.imovel_geom, app.ogr_geometry)
    ) AS tbl
), car_vegetacao AS (
  SELECT  *,
          COALESCE(
            ST_Difference(imovel_vegetacao_nativa_geom, desmatamento_vegetacao_nativa_geom)
          ) AS desmatamento_vegetacao_nativa_difference_geom
    FROM (
      SELECT  car_alerta.monitored_id AS imovel_fid,
              car_alerta.alerta_id AS alerta_fid,
              car_alerta.imovel_geom,
              car_alerta.desmatamento_geom,
              vegetacao_nativa.fid AS vegetacao_nativa_fid,
              ST_Multi(ST_Intersection(car_alerta.imovel_geom, vegetacao_nativa.ogr_geometry)) AS imovel_vegetacao_nativa_geom,
              ST_Multi(ST_Intersection(car_alerta.desmatamento_geom, vegetacao_nativa.ogr_geometry)) AS desmatamento_vegetacao_nativa_geom
        FROM  car_alerta
        LEFT JOIN vegetacao_nativa
              ON ST_Intersects(car_alerta.imovel_geom, vegetacao_nativa.ogr_geometry)
    ) AS tbl
)
SELECT car_app.alerta_fid,
       car_app.imovel_fid,
       car_app.imovel_geom,
       car_app.desmatamento_geom,
       car_app.app_fid,
       car_app.imovel_app_geom,
       car_app.desmatamento_app_geom,
       car_app.desmatamento_app_difference_geom,
       car_reserva.reserva_legal_fid,
       car_reserva.imovel_reserva_legal_geom,
       car_reserva.desmatamento_reserva_legal_geom,
       car_reserva.desmatamento_reserva_legal_difference_geom,
       car_vegetacao.vegetacao_nativa_fid,
       car_vegetacao.imovel_vegetacao_nativa_geom,
       car_vegetacao.desmatamento_vegetacao_nativa_geom,
       car_vegetacao.desmatamento_vegetacao_nativa_difference_geom
  FROM car_app
  LEFT JOIN car_reserva
     ON car_app.alerta_fid = car_reserva.alerta_fid
    AND car_app.imovel_fid = car_reserva.imovel_fid
  LEFT JOIN car_vegetacao
     ON car_app.alerta_fid = car_vegetacao.alerta_fid
    AND car_app.imovel_fid = car_vegetacao.imovel_fid;
-- SELECT *
--   FROM car_app JOIN car_reserva USING (alerta_fid, imovel_fid);
--  WHERE car_app.alerta_fid = car_reserva.alerta_fid
--    AND car_app.imovel_fid = car_reserva.imovel_fid;
-- SELECT  *,
--   FROM car_reserva
--  WHERE ST_Intersects(imovel_reserva_legal_geom, desmatamento_geom) OR
--        ST_Intersects(imovel_app_geom, desmatamento_geom);



------------------------------------- WORKING -------------------------------------------

DROP TABLE IF EXISTS car_tmp;

CREATE TABLE car_tmp AS
WITH car_alerta AS (
  SELECT car_area_imovel.fid::VARCHAR AS monitored_id,
         alertas_mpmt.fid::VARCHAR AS alerta_id,
         car_area_imovel.ogr_geometry AS imovel_geom,
         ST_Intersection(car_area_imovel.ogr_geometry, ST_Transform(alertas_mpmt.ogr_geometry, 4674)) AS desmatamento_geom FROM car_area_imovel, alertas_mpmt
   WHERE ST_Intersects(car_area_imovel.ogr_geometry, ST_Transform(alertas_mpmt.ogr_geometry, 4674)) AND
         alertas_mpmt.view_date::date > '2018-01-01'
), car_merge AS (
  SELECT  car_alerta.monitored_id AS imovel_fid,
            car_alerta.alerta_id AS alerta_fid,
            ST_Multi(car_alerta.imovel_geom) AS imovel_geom,
            ST_Multi(car_alerta.desmatamento_geom) AS desmatamento_geom,
            reserva_legal.fid AS reserva_legal_fid,
            ST_Multi(ST_Intersection(car_alerta.imovel_geom, reserva_legal.ogr_geometry)) AS imovel_reserva_legal_geom,
            ST_Multi(ST_Intersection(car_alerta.desmatamento_geom, reserva_legal.ogr_geometry)) AS desmatamento_reserva_legal_geom,
            app.fid AS app_fid,
            ST_Multi(ST_Intersection(car_alerta.imovel_geom, app.ogr_geometry)) AS imovel_app_geom,
            ST_Multi(ST_Intersection(car_alerta.desmatamento_geom, app.ogr_geometry)) AS desmatamento_app_geom
      FROM car_alerta
        LEFT JOIN reserva_legal
          ON ST_Intersects(car_alerta.imovel_geom, reserva_legal.ogr_geometry)
        LEFT JOIN app
          ON ST_Intersects(car_alerta.imovel_geom, app.ogr_geometry)
), car_union AS (
  SELECT ST_Union(desmatamento_geom) as geom
              FROM car_merge
             WHERE ST_Intersects(imovel_reserva_legal_geom, desmatamento_geom)
)
SELECT  *,
        COALESCE(
          ST_Difference(imovel_reserva_legal_geom, (
            SELECT * FROM car_union
          ))
        ) AS desmatamento_reserva_legal_difference_geom,
        COALESCE(
          ST_Difference(imovel_app_geom, (
            SELECT * FROM car_union
          ))
        ) AS desmatamento_app_difference_geom
  FROM car_merge
      WHERE ST_Intersects(imovel_reserva_legal_geom, desmatamento_geom)


---------------------------------- OLD --------------------------------------------------
DROP TABLE IF EXISTS car_tmp;

CREATE TABLE car_tmp AS
WITH car_alerta AS (
  SELECT car_area_imovel.fid::VARCHAR AS monitored_id,
         alertas_mpmt.fid::VARCHAR AS alerta_id,
         car_area_imovel.ogr_geometry AS imovel_geom,
         ST_Intersection(car_area_imovel.ogr_geometry, ST_Transform(alertas_mpmt.ogr_geometry, 4674)) AS desmatamento_geom FROM car_area_imovel, alertas_mpmt
   WHERE ST_Intersects(car_area_imovel.ogr_geometry, ST_Transform(alertas_mpmt.ogr_geometry, 4674)) AND
         alertas_mpmt.view_date::date > '2018-01-01'
)
SELECT car_alerta.monitored_id AS imovel_fid,
       car_alerta.alerta_id AS alerta_fid,
       car_alerta.imovel_geom AS imovel_geom,
       car_alerta.desmatamento_geom AS desmatamento_geom,
       reserva_legal.fid AS reserva_legal_fid,
       ST_Intersection(car_alerta.imovel_geom, reserva_legal.ogr_geometry) AS imovel_reserva_legal_geom,
       ST_Intersection(car_alerta.desmatamento_geom, reserva_legal.ogr_geometry) AS desmatamento_reserva_legal_geom,
       ST_Difference(car_alerta.desmatamento_geom, reserva_legal.ogr_geometry) AS desmatamento_reserva_legal_difference_geom
       app.fid AS app_fid,
       ST_Intersection(car_alerta.imovel_geom, app.ogr_geometry) AS imovel_app_geom,
       ST_Intersection(car_alerta.desmatamento_geom, app.ogr_geometry) AS desmatamento_app_geom
  FROM car_alerta
  LEFT JOIN reserva_legal
    ON ST_Intersects(car_alerta.imovel_geom, reserva_legal.ogr_geometry)
  LEFT JOIN app
    ON ST_Intersects(car_alerta.imovel_geom, app.ogr_geometry);