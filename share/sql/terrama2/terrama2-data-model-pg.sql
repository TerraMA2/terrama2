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
  cellphone VARCHAR(20)  NOT NULL
);

COMMENT ON TABLE terrama2.users IS 'Store information about TerraMA2 users';
COMMENT ON COLUMN terrama2.users.id IS 'Identifier of an user';
COMMENT ON COLUMN terrama2.users.login IS 'Login used to access the system';
COMMENT ON COLUMN terrama2.users.password IS 'Password used to access the system';
COMMENT ON COLUMN terrama2.users.name IS 'Full username';
COMMENT ON COLUMN terrama2.users.email IS 'User email';
COMMENT ON COLUMN terrama2.users.cellphone IS 'Cellphone number used to send notifications';


CREATE TABLE terrama2.unit_of_measure
(
  id     INTEGER PRIMARY KEY,
  unit   TEXT
);


CREATE TABLE terrama2.data_provider_type
(
  id          SERIAL PRIMARY KEY,
  name        VARCHAR(50) UNIQUE,
  description TEXT
);

INSERT INTO terrama2.data_provider_type (name, description)
     VALUES ('FTP', 'File Transfer Protocol'),
            ('HTTP', 'Hyper Text Transfer Protocol'),
            ('FILE', 'Local File System'),
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
     VALUES ('PCD', 'Identifies a PCD dataset'),
            ('Occurrence', 'Identifies a dataset for occurrences'),
            ('Grid', 'Identifies a grid dataset');

CREATE TABLE terrama2.dataset
(
  id                        SERIAL NOT NULL PRIMARY KEY,
  name                      VARCHAR(20) NOT NULL UNIQUE,
  description               TEXT,
  active                    BOOLEAN,
  data_provider_id          INTEGER NOT NULL,
  kind                      INTEGER NOT NULL,
  data_frequency            NUMERIC,
  data_frequency_id         INTEGER,
  schedule                  TIME,
  schedule_retry            NUMERIC,
  schedule_retry_unit_id    INTEGER,
  schedule_timeout          NUMERIC,
  schedule_timeout_unit_id  INTEGER,
  CONSTRAINT fk_dataset_data_provider_id
           FOREIGN KEY(data_provider_id)
           REFERENCES terrama2.data_provider(id)
           ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT fk_dataset_data_frequency_id
           FOREIGN KEY(data_frequency_id)
           REFERENCES terrama2.unit_of_measure(id)
           ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT fk_dataset_schedule_retry_unit_id
         FOREIGN KEY(schedule_retry_unit_id)
         REFERENCES terrama2.unit_of_measure(id)
         ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT fk_dataset_schedule_timeout_unit_id
         FOREIGN KEY(schedule_timeout_unit_id)
         REFERENCES terrama2.unit_of_measure(id)
         ON UPDATE CASCADE ON DELETE RESTRICT
);

CREATE TABLE terrama2.dataset_metadata
(
  id     INTEGER PRIMARY KEY,
  key    VARCHAR(50),
  value  VARCHAR(50),
  dataset_id INTEGER,
  CONSTRAINT fk_dataset_metadata_dataset_id
    FOREIGN KEY(dataset_id)
    REFERENCES terrama2.dataset(id)
    ON UPDATE CASCADE ON DELETE CASCADE
);


CREATE TABLE terrama2.dataset_collect_rule
(
  id     INTEGER PRIMARY KEY,
  script TEXT,
  dataset_id INTEGER,
  CONSTRAINT fk_dataset_collect_rule_dataset_id
    FOREIGN KEY(dataset_id)
    REFERENCES terrama2.dataset(id)
    ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE terrama2.data_type
(
  id          SERIAL NOT NULL PRIMARY KEY,
  name        VARCHAR(50) NOT NULL UNIQUE,
  description TEXT
);

CREATE TABLE terrama2.data
(
  id           SERIAL NOT NULL PRIMARY KEY,
  kind         INTEGER NOT NULL,
  active       BOOLEAN,
  dataset_id   INTEGER,
  mask         VARCHAR(255),
  timezone     text DEFAULT '+00:00',
  CONSTRAINT fk_dataset_data_type_id
    FOREIGN KEY(kind)
    REFERENCES terrama2.data_type(id)
    ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT fk_data_dataset_id
    FOREIGN KEY(dataset_id)
    REFERENCES terrama2.dataset(id)
    ON UPDATE CASCADE ON DELETE CASCADE
);



CREATE TABLE terrama2.data_collection_log
(
  id SERIAL NOT NULL,
  data_id INTEGER NOT NULL,
  uri VARCHAR(255) NOT NULL,
  data_timestamp timestamp without time zone NOT NULL,
  collect_timestamp timestamp without time zone NOT NULL DEFAULT NOW(),
  CONSTRAINT fk_data_collection_log_data_id
    FOREIGN KEY(data_id)
    REFERENCES terrama2.data (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

COMMENT ON TABLE terrama2.data_collection_log IS 'Table used to register all collected data';
COMMENT ON COLUMN terrama2.data_collection_log.id IS 'Log identifier';
COMMENT ON COLUMN terrama2.data_collection_log.data_id IS 'Data identifier';
COMMENT ON COLUMN terrama2.data_collection_log.uri IS 'URI to the collected data';
COMMENT ON COLUMN terrama2.data_collection_log.data_timestamp IS 'Date of the generated data';
COMMENT ON COLUMN terrama2.data_collection_log.collect_timestamp IS 'Date of the collection by TerraMA';


CREATE TABLE terrama2.pcd
(
  data_id SERIAL NOT NULL PRIMARY KEY,
  location GEOMETRY(Point,4326),
  table_name VARCHAR(50) NOT NULL,
  CONSTRAINT fk_pcd_data_id
    FOREIGN KEY(data_id)
    REFERENCES terrama2.data (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);


CREATE TABLE terrama2.pcd_attributes
(
  id      SERIAL  NOT NULL PRIMARY KEY,
  data_id INTEGER NOT NULL,
  attr_name VARCHAR(50) NOT NULL,
  attr_type_id INTEGER NOT NULL,
  CONSTRAINT fk_pcd_attributes_data_id
    FOREIGN KEY(data_id)
    REFERENCES terrama2.pcd (data_id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE terrama2.archiving_rules
(
  id SERIAL NOT NULL,
  dataset_id INTEGER,
  analysis_id INTEGER,
  type VARCHAR(50) NOT NULL,
  action VARCHAR(50) NOT NULL,
  condition INTEGER NOT NULL,
  create_filter BOOLEAN NOT NULL,
  CONSTRAINT pk_archiving_rules_id PRIMARY KEY (id),
  CONSTRAINT fk_archiving_rules_dataset_id FOREIGN KEY(dataset_id) REFERENCES terrama2.dataset(id) ON UPDATE CASCADE ON DELETE CASCADE
--  CONSTRAINT fk_archiving_rules_analysis_id FOREIGN KEY(analysis_id) REFERENCES terrama2.analysis(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);


COMMENT ON TABLE terrama2.archiving_rules IS 'Table used to store the archiving rules, it is possible to configure a rule for an analysis or a collector';
COMMENT ON COLUMN terrama2.archiving_rules.id IS 'Identifier of the archiving rule';
COMMENT ON COLUMN terrama2.archiving_rules.dataset_id IS 'Dataset identifier, used when the rule is pointed to a dataset';
COMMENT ON COLUMN terrama2.archiving_rules.analysis_id IS 'Analysis identifier, used when the rule is pointed to a analysis';
COMMENT ON COLUMN terrama2.archiving_rules.type IS 'Rule type, possible values: DATASOURCE or ANALYSIS';
COMMENT ON COLUMN terrama2.archiving_rules.action IS 'Defines strategy to archive the data, possible values: DELETE_DATA, DELETE_LOG, DELETE_WARNING and DELETE_SURFACE';
COMMENT ON COLUMN terrama2.archiving_rules.condition IS 'Defines which data from the collector or analysis will be considered by the rule';
COMMENT ON COLUMN terrama2.archiving_rules.create_filter IS 'Filter valid only for the type datasource, it will impact the filter of the collector, it will change for the date of the execution of the archiving';


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



CREATE TABLE terrama2.storage_strategy
(
  dataset_id   INTEGER NOT NULL PRIMARY KEY,
  uri_mask     TEXT,
  CONSTRAINT fk_storage_strategy_dataset_id FOREIGN KEY(dataset_id) REFERENCES terrama2.dataset (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);


COMMENT ON TABLE terrama2.storage_strategy IS 'Used to store the storage strategy of a dataset';
COMMENT ON COLUMN terrama2.storage_strategy.dataset_id IS 'Dataset identifier';
COMMENT ON COLUMN terrama2.storage_strategy.uri_mask IS 'URI to the mask of the collected data';

COMMIT;