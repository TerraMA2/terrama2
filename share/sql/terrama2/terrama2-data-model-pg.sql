--
-- TerraMA2 Data Model for PostgreSQL DBMS
--

BEGIN TRANSACTION;

CREATE SCHEMA terrama2 AUTHORIZATION postgres;

COMMENT ON SCHEMA terrama2 IS 'Schema used to store all objects related to TerraMA';

CREATE TABLE terrama2.version
(
  version     VARCHAR(10),
  description TEXT
);

COMMENT ON TABLE terrama2.version IS 'Table used to store the database version';
COMMENT ON COLUMN terrama2.version.version IS 'Number of the version';
COMMENT ON COLUMN terrama2.version.description IS 'Description of this version';

CREATE TABLE terrama2.users
(
  id        SERIAL       PRIMARY KEY,
  login     VARCHAR(20)  NOT NULL UNIQUE,
  password  VARCHAR(20)  NOT NULL,
  name      VARCHAR(200) NOT NULL,
  email     VARCHAR(50)  NOT NULL,
  cellphone VARCHAR(20)  NOT NULL,
);

COMMENT ON TABLE terrama2.users IS 'Store information about TerraMA2 users';
COMMENT ON COLUMN terrama2.users.id IS 'Identifier of an user';
COMMENT ON COLUMN terrama2.users.login IS 'Login used to access the system';
COMMENT ON COLUMN terrama2.users.password IS 'Password used to access the system';
COMMENT ON COLUMN terrama2.users.name IS 'Full username';
COMMENT ON COLUMN terrama2.users.email IS 'User email';
COMMENT ON COLUMN terrama2.users.cellphone IS 'Cellphone number used to send notifications';

CREATE TABLE terrama2.data_provider_type
{
  id          SERIAL PRIMARY KEY,
  name        VARCHAR(50) UNIQUE,
  description TEXT
};

INSERT INTO terrama2.data_provider_type (name, description)
     VALUES ('FTP', 'File Transfer Protocol'),
            ('HTTP', 'Hyper Text Transfer Protocol'),
            ('FILE', 'Local System File'),
            ('WFS', 'OGC Web Feature Service'),
            ('WCS', 'OGC Web Coverage Service');

CREATE TABLE terrama2.data_provider
(
  id          SERIAL NOT NULL PRIMARY KEY,
  name        VARCHAR(60) NOT NULL UNIQUE,
  description TEXT,
  kind        INTEGER NOT NULL,
  uri         TEXT,
  active      BOOLEAN NOT NULL,
  CONSTRAINT fk_data_provider_kind
             FOREIGN KEY(kind)
             REFERENCES terrama2.data_provider_type(id)
             ON UPDATE CASCADE ON DELETE RESTRICT
);


--COMMENT ON TABLE terrama2.data_provider IS 'Store information about TerraMA2 data providers (remote servers, ftp servers, web services)';
--COMMENT ON COLUMN terrama2.server.id IS 'Data Provider identifier';
--COMMENT ON COLUMN terrama2.server.name IS 'Name of the server, must be unique';
--COMMENT ON COLUMN terrama2.server.protocol IS 'Defines which protocol of communication will be used, e.g. FTP, HTTP, File or OGC';
--COMMENT ON COLUMN terrama2.server.url IS 'Defines the server address, for File protocol should be the base file path';
--COMMENT ON COLUMN terrama2.server.username IS 'Username to be used in order to access a FTP server';
--COMMENT ON COLUMN terrama2.server.password IS 'Password to be used in order to access a FTP server';
--COMMENT ON COLUMN terrama2.server.port IS 'Port to be user in order to access a FTP server';
--COMMENT ON COLUMN terrama2.server.description IS 'Description of the server';
--COMMENT ON COLUMN terrama2.server.active IS 'Defines if the server is active and should be checked periodically';
--COMMENT ON COLUMN terrama2.server.base_path IS 'Optional parameter to indicate a base path to a FTP server';

CREATE TABLE terrama2.dataset_type
(
  id          SERIAL NOT NULL PRIMARY KEY,
  name        VARCHAR(50) NOT NULL UNIQUE,
  description TEXT
);

INSERT INTO terrama2.dataset_type (name, description)
     VALUES ('PCD', '?????'),
            ('Ocorrencias', '???'),
            ('Grades', '????');

CREATE TABLE terrama2.dataset
(
  id                SERIAL NOT NULL PRIMARY KEY,
  name              VARCHAR(20) NOT NULL UNIQUE,
  description       TEXT,
  kind              INTEGER NOT NULL,
  data_provider_id  INTEGER NOT NULL,
  
);

CREATE TABLE terrama2.dataset_metadata
(
  key    VARCHAR(50),
  value  VARCHAR(50),
  dataset_id INTEGER
);

CREATE TABLE terrama2.series_pcd
(
  id         SERIAL NOT NULL,
  dataset_id INTEGER
  data_type VARCHAR(40),
  script text,
  id_attr varchar(50),
  CONSTRAINT pk_series_pcd_id PRIMARY KEY(id),
  CONSTRAINT uk_series_pcd_name UNIQUE (name)
);

COMMENT ON TABLE terrama2.series_pcd IS 'Used to store the parameters of a PCD collector';
COMMENT ON COLUMN terrama2.series_pcd.id IS 'PCD identifier';
COMMENT ON COLUMN terrama2.series_pcd.name IS 'Name of the PCD';
COMMENT ON COLUMN terrama2.series_pcd.influence IS 'Defines the area of influence of a PCD';
COMMENT ON COLUMN terrama2.series_pcd.data_type IS 'Describes the type of content of this PCD';
COMMENT ON COLUMN terrama2.series_pcd.script IS 'Preprocessing rule to be applied before importing data';
COMMENT ON COLUMN terrama2.series_pcd.id_attr IS 'Attribute that identifies a PCD';



  update_frequency_minutes INTEGER NOT NULL,
  prefix VARCHAR(10),
  projection_id INTEGER NOT NULL,
  mask VARCHAR(255) NOT NULL,
  unit VARCHAR(20),
  timezone VARCHAR(10) NOT NULL DEFAULT '+00:00',
  format text NOT NULL,
  dynamic_metadata JSON,

  CONSTRAINT pk_collector_id PRIMARY KEY (id),
  CONSTRAINT uk_name UNIQUE (name),
  CONSTRAINT fk_ccollector_server_id FOREIGN KEY (server_id) REFERENCES terrama2.server (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_collector_projection FOREIGN KEY (projection_id)
    REFERENCES public.spatial_ref_sys (srid) MATCH SIMPLE
    ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT format_check CHECK ((format =
    ANY (ARRAY[ 'ASCII-Grid'::text, 'TIFF'::text, 'GrADS'::text, 'PROARCO File'::text, 'OGC WCS'::text, 'PCD OGC WFS'::text, 'Surface'::text])))

--  CONSTRAINT type_check CHECK ((type = ANY (ARRAY[SERIES DE DADOS ACEITAS]))),
);

COMMENT ON TABLE terrama2.collector IS 'Table used to store the collector information';
COMMENT ON COLUMN terrama2.collector.id IS 'Identifier of a collector';
COMMENT ON COLUMN terrama2.collector.name IS 'Unique name of the collector';
COMMENT ON COLUMN terrama2.collector.description IS 'Description of the collector';
COMMENT ON COLUMN terrama2.collector.server_id IS 'Server identifier';
COMMENT ON COLUMN terrama2.collector.type IS 'Data serie type';
COMMENT ON COLUMN terrama2.collector.update_frequency_minutes IS 'Time in minutes between data aquisition';
COMMENT ON COLUMN terrama2.collector.prefix IS 'Prefix for the data tables or files created to store aquired data';
COMMENT ON COLUMN terrama2.collector.projection_id IS 'SRID of the projection';
COMMENT ON COLUMN terrama2.collector.mask IS 'Mask of filename format to aquire';
COMMENT ON COLUMN terrama2.collector.unit IS 'Unit of aquired data';
COMMENT ON COLUMN terrama2.collector.timezone IS 'Timezone of stored date';
COMMENT ON COLUMN terrama2.collector.format IS 'Data format type';


CREATE TABLE terrama2.interpolator
(
  id SERIAL NOT NULL,
  attribute_name text,
  grid_output_name text,
  method text,
  number_neighbors INTEGER,
  pow_value INTEGER,
  roi_x1 double precision,
  roi_y1 double precision,
  roi_x2 double precision,
  roi_y2 double precision,
  res_unit text,
  res_x double precision,
  res_y double precision,
  CONSTRAINT interpolator_method_check   CHECK ((method = ANY (ARRAY['NN'::text, 'AVERAGE'::text, 'WEIGHT_AVERAGE'::text, 'LINEAR_LEAST_SQUARE'::text]))),
  CONSTRAINT interpolator_res_unit_check CHECK ((res_unit = ANY (ARRAY['METERS'::text, 'DD'::text]))),
  CONSTRAINT pk_interpolator PRIMARY KEY(id)
);

COMMENT ON TABLE terrama2.interpolator IS 'Table used to store the interpolator parameters used to create a surface from a PCD';
COMMENT ON COLUMN terrama2.interpolator.id IS 'Interpolator identifier';
COMMENT ON COLUMN terrama2.interpolator.attribute_name IS 'Attribute to be used to create the surface';
COMMENT ON COLUMN terrama2.interpolator.grid_output_name IS 'Name of the output grid';
COMMENT ON COLUMN terrama2.interpolator.method IS 'Intepolation method';
COMMENT ON COLUMN terrama2.interpolator.number_neighbors IS 'Number of number_neighbors';
COMMENT ON COLUMN terrama2.interpolator.pow_value IS 'Power value, used only for the method weight average';
COMMENT ON COLUMN terrama2.interpolator.roi_x1 IS 'Lower left X coordinate';
COMMENT ON COLUMN terrama2.interpolator.roi_y1 IS 'Lower left Y coordinate';
COMMENT ON COLUMN terrama2.interpolator.roi_x2 IS 'Upper right X coordinate';
COMMENT ON COLUMN terrama2.interpolator.roi_y2 IS 'Upper right Y coordinate';
COMMENT ON COLUMN terrama2.interpolator.res_unit IS 'Resolution unit of measure';
COMMENT ON COLUMN terrama2.interpolator.res_x IS 'Size of X resolution';
COMMENT ON COLUMN terrama2.interpolator.res_y IS 'Size of Y resolution';


CREATE TABLE terrama2.collector_interpolator
(
  collector_id INTEGER NOT NULL,
  interpolator_id INTEGER NOT NULL,
  CONSTRAINT fk_collector_interpolator_collector_id FOREIGN KEY(collector_id) REFERENCES terrama2.collector(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_collector_interpolator_interpolator_id FOREIGN KEY(interpolator_id ) REFERENCES terrama2.interpolator(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT pk_collector_interpolator PRIMARY KEY (collector_id, interpolator_id)
);

COMMENT ON TABLE terrama2.collector_interpolator IS 'Table used to link multiple interpolators with a collector';
COMMENT ON COLUMN terrama2.collector_interpolator.collector_id IS 'Collector identifier';
COMMENT ON COLUMN terrama2.collector_interpolator.interpolator_id IS 'Interpolator identifier';



CREATE TABLE terrama2.collector_log
(
  id SERIAL NOT NULL,
  collector_id INTEGER NOT NULL,
  data_path VARCHAR(255) NOT NULL,
  date_time_file timestamp without time zone NOT NULL,
  date_time_collection timestamp without time zone NOT NULL DEFAULT NOW(),
  status VARCHAR(20),
  CONSTRAINT pk_collector_log PRIMARY KEY(id),
  CONSTRAINT fk_collector_log_collector_id FOREIGN KEY(collector_id) REFERENCES terrama2.collector (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

COMMENT ON TABLE terrama2.collector_log IS 'Table used to register all collected data';
COMMENT ON COLUMN terrama2.collector_log.id IS 'Log identifier';
COMMENT ON COLUMN terrama2.collector_log.collector_id IS 'Collector identifier';
COMMENT ON COLUMN terrama2.collector_log.data_path IS 'Path to the collected data';
COMMENT ON COLUMN terrama2.collector_log.date_time_file IS 'Date of the generated data';
COMMENT ON COLUMN terrama2.collector_log.date_time_collection IS 'Date of the collection by TerraMA';
COMMENT ON COLUMN terrama2.collector_log.status IS 'Status of the collection';


CREATE TABLE terrama2.archiving_rules
(
  id SERIAL NOT NULL,
  collector_id INTEGER,
  analysis_id INTEGER,
  type VARCHAR(50) NOT NULL,
  action VARCHAR(50) NOT NULL,
  condition INTEGER NOT NULL,
  create_filter BOOLEAN NOT NULL,
  CONSTRAINT pk_archiving_rules_id PRIMARY KEY (id),
  CONSTRAINT fk_archiving_rules_collector_id FOREIGN KEY(collector_id) REFERENCES terrama2.collector(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
--  CONSTRAINT fk_archiving_rules_analysis_id FOREIGN KEY(analysis_id) REFERENCES terrama2.analysis(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);


COMMENT ON TABLE terrama2.archiving_rules IS 'Table used to store the archiving rules, it is possible to configure a rule for an analysis or a collector';
COMMENT ON COLUMN terrama2.archiving_rules.id IS 'Identifier of the archiving rule';
COMMENT ON COLUMN terrama2.archiving_rules.collector_id IS 'Collector identifier, used when the rule is pointed to a collector';
COMMENT ON COLUMN terrama2.archiving_rules.analysis_id IS 'Analysis identifier, used when the rule is pointed to a analysis';
COMMENT ON COLUMN terrama2.archiving_rules.type IS 'Rule type, possible values: DATASOURCE or ANALYSIS';
COMMENT ON COLUMN terrama2.archiving_rules.action IS 'Defines strategy to archive the data, possible values: DELETE_DATA, DELETE_LOG, DELETE_WARNING and DELETE_SURFACE';
COMMENT ON COLUMN terrama2.archiving_rules.condition IS 'Defines which data from the collector or analysis will be considered by the rule';
COMMENT ON COLUMN terrama2.archiving_rules.create_filter IS 'Filter valid only for the type datasource, it will impact the filter of the collector, it will change for the date of the execution of the archiving';





CREATE TABLE terrama2.series_pcd_collector
(
  series_pcd_id INTEGER NOT NULL,
  collector_id INTEGER NOT NULL,
  CONSTRAINT pk_series_pcd_collector_id PRIMARY KEY(series_pcd_id, collector_id),
  CONSTRAINT fk_series_pcd_collector_collector_id FOREIGN KEY(collector_id) REFERENCES terrama2.collector (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_series_pcd_collector_series_pcd_id FOREIGN KEY(series_pcd_id) REFERENCES terrama2.series_pcd (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

COMMENT ON TABLE terrama2.series_pcd_collector IS 'Used to link multiple collectors to a series PCD';
COMMENT ON COLUMN terrama2.series_pcd_collector.series_pcd_id IS 'PCD identifier';
COMMENT ON COLUMN terrama2.series_pcd_collector.collector_id IS 'Collector identifier';


CREATE TABLE terrama2.storage_strategy
(
  collector_id INTEGER NOT NULL,
  table_name VARCHAR(40),
  unique_storage BOOLEAN NOT NULL,
  CONSTRAINT pk_storage_strategy_id PRIMARY KEY(collector_id),
  CONSTRAINT fk_storage_strategy_collector_id FOREIGN KEY(collector_id) REFERENCES terrama2.collector (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);


COMMENT ON TABLE terrama2.storage_strategy IS 'Used to store the parameters of a PCD collector';
COMMENT ON COLUMN terrama2.storage_strategy.collector_id IS 'Collector identifier';
COMMENT ON COLUMN terrama2.storage_strategy.table_name IS 'Name of the table that will store the data collected';
COMMENT ON COLUMN terrama2.storage_strategy.unique_storage IS 'Defines if all collected data should be stored in the same table';

COMMIT;