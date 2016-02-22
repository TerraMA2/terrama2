--prepare database
INSERT INTO terrama2.version (version, tag, description) VALUES (2, 'alpha_2', 'Segunda version from ALPHA series');
INSERT INTO terrama2.instance_type(name) VALUES('COLLECTOR'), ('ANALYSIS'), ('ALERT');
INSERT INTO terrama2.project_version (version, tag) VALUES ( (SELECT id from terrama2.version WHERE version=2) , 'alpha_2');
INSERT INTO terrama2.data_provider_scheme(name, description) VALUES
  ('FTP', 'File Transfer Protocol'),
    ('WCS', 'Web Coverage Service'),
      ('FILE', 'Local file'),
        ('POSTGIS', 'PostgreSQL/PostGIS SGDB');
INSERT INTO terrama2.log_status (status) VALUES
  ('DOWNLOADED'),
    ('STORED'),
      ('ANALIZED');

INSERT INTO terrama2.data_provider_type(name, description) VALUES('COLLECTOR', 'Remote aquisition DataProvider'), ('PROCESS', 'Direct data DataProvider');
INSERT INTO terrama2.data_series_type(name, description) VALUES
('DCP_INPE', 'INPE DCP DataSeries, Text CSV files with timestamp information'),
 ('WILDFIRE_INPE', 'WildFire occurrences from INPE, Text CSV files with positon and timestamp'),
  ('POSTGIS', 'Spatial data stored in a PostgreSQL/PostGIS SGDB');

--new project
INSERT INTO terrama2.project (name, description, project_version) VALUES
  ('Queimadas', 'Previsão de queimadas na mata atlântica', (SELECT id from terrama2.project_version WHERE tag='alpha_2') );

--nova instancia
INSERT INTO terrama2.instance (name, description, instance_type) VALUES
  ('Serviço de coleta inpe_01', 'Serviço de coleta alocado na maquina 001', (SELECT id from terrama2.instance_type WHERE name='COLLECTOR'));
INSERT INTO terrama2.instance (name, description, instance_type) VALUES
  ('Serviço de análise inpe_01', 'Serviço de análise alocado na maquina 001', (SELECT id from terrama2.instance_type WHERE name='ANALYSIS'));
INSERT INTO terrama2.instance (name, description, instance_type) VALUES
  ('Serviço de análise inpe_02', 'Serviço de análise alocado na maquina 002', (SELECT id from terrama2.instance_type WHERE name='ANALYSIS'));

--novo DataProvider
INSERT INTO terrama2.data_provider (project, name, description, data_provider_scheme, data_provider_type, uri, active) VALUES
 ((SELECT id from terrama2.project WHERE name='Queimadas'), 'Servidor FTP INPE', 'Servidor de dados de pcd do inpe',
      (SELECT id from terrama2.data_provider_scheme WHERE name='FTP'), (SELECT id from terrama2.data_provider_type WHERE name='COLLECTOR'), 'FTP://127.0.0.1', TRUE);

--novo DataSeries
INSERT INTO terrama2.data_series (data_provider, name, description, data_series_type) VALUES
  ((SELECT id from terrama2.data_provider WHERE name='Servidor FTP INPE'), 'PCD de sao paulo', 'PCDs do estado de sao paulo', 1);

--novo dataset
WITH dataset_id AS (
  INSERT INTO terrama2.dataset (data_series, mask, path, active, srid, timezone) VALUES
    ((SELECT id from terrama2.data_series WHERE name='PCD de sao paulo'), '4856.txt', NULL, TRUE, 4326, '+03:00') RETURNING id
)
INSERT INTO terrama2.dataset_properties (dataset, attribute, alias, position) VALUES
  ((SELECT id from dataset_id), 'sat', 'satélite', 1);

--Novo collector
--*****************************************************************
INSERT INTO terrama2.data_provider (project, name, description, data_provider_scheme, data_provider_type, uri, active) VALUES
  ((SELECT id from terrama2.project WHERE name='Queimadas'), 'Servidor FTP queimadas', NULL,
    (SELECT id from terrama2.data_provider_scheme WHERE name='FTP'),
      (SELECT id from terrama2.data_provider_type WHERE name='COLLECTOR'), 'FTP://127.0.0.1', TRUE),

    ((SELECT id from terrama2.project WHERE name='Queimadas'), 'PostGIS local', NULL,
      (SELECT id from terrama2.data_provider_scheme WHERE name='POSTGIS'),
        (SELECT id from terrama2.data_provider_type WHERE name='COLLECTOR'), 'POSTGIS://user:password@localhost/database', TRUE);

INSERT INTO terrama2.data_series (data_provider, name, description, data_series_type) VALUES
  ((SELECT id from terrama2.data_provider WHERE name='Servidor FTP queimadas'), 'queimadas sao paulo_system_only', 'Queimadas no estado de sao paulo', 2),
    ((SELECT id from terrama2.data_provider WHERE name='PostGIS local'), 'queimadas sao paulo', 'Queimadas no estado de sao paulo', 2);

WITH input_id AS
(
  INSERT INTO terrama2.dataset (data_series, mask, path, active, srid, timezone) VALUES
    ((SELECT id from terrama2.data_series WHERE name='queimadas sao paulo_system_only'), 'exporta_%A%M%d_%h%m.csv', 'focos_operacao', TRUE, 4326, '+00') RETURNING id
),
 output_id AS
(
  INSERT INTO terrama2.dataset (data_series, mask, path, active, srid, timezone) VALUES
      ((SELECT id from terrama2.data_series WHERE name='queimadas sao paulo'), 'terrama2.focos_sp', NULL, TRUE, 4326, '+00') RETURNING id
),
 schedule_id AS
(
  INSERT INTO terrama2.schedule (frequency, frequency_unit, schedule, schedule_retry, schedule_retry_unit, schedule_timeout, schedule_timeout_unit) VALUES
    (10, 'min', NULL, NULL, NULL, NULL, NULL) RETURNING id
),
 collector_id AS
(
  INSERT INTO terrama2.collector (data_series_input, data_series_output, project, instance, schedule, active) VALUES
    ((SELECT id from terrama2.data_series WHERE name='queimadas sao paulo_system_only'),
      (SELECT id from terrama2.data_series WHERE name='queimadas sao paulo'),
        (SELECT id from terrama2.project WHERE name='Queimadas'),
          (SELECT id from terrama2.instance WHERE name='Serviço de coleta inpe_01'),
            (SELECT id from schedule_id),
              TRUE) RETURNING id
)
INSERT INTO terrama2.collector_input_output (collector, input_dataset, output_dataset) VALUES
  ((SELECT id from collector_id),
      (SELECT id from input_id),
        (SELECT id from output_id));

--INSERT INTO terrama2.filter (collector, discard_before, discard_after, region, by_value, value_comparison_operation) VALUES
--  ((SELECT id from collector_id), '2015-12-31 19:15:00', NULL, NULL, NULL, NULL);
--****************************************************************************************

--TODO: Com problemas, revisar


--log de arquivo baixado
INSERT INTO terrama2.data_collection_log (status, origin_uri, storage_uri, data_timestamp, collect_timestamp, collector_input_output) VALUES
  ((SELECT id FROM terrama2.log_status WHERE status='DOWNLOADED'), 'FTP://user:password@ftp.dgi.inpe.br/focos_operacao/exporta_20160101_0130.csv', NULL, '2015-12-31 19:15:00', '2015-12-31 23:00:00',1);
--log de arquivo armazenado
UPDATE terrama2.data_collection_log SET status = (SELECT id FROM terrama2.log_status WHERE status='STORED'), storage_uri = 'POSTGIS://user:password@127.0.0.1:port/basedeteste/terrama2.focos_sp' WHERE storage_uri = NULL AND origin_uri = 'FPT://user:password@ftp.dgi.inpe.br/focos_operacao/exporta_20160101_0130.csv' AND collector_input_output = 1;
