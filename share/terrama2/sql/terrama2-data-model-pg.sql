--
-- TerraMA2 Data Model for PostgreSQL DBMS
--

BEGIN TRANSACTION;

CREATE SCHEMA terrama2;

COMMENT ON SCHEMA terrama2 IS 'TerraMA2 Schema';

CREATE TABLE terrama2.version
(
  version     VARCHAR(10),
  description TEXT
);

COMMENT ON TABLE terrama2.version IS 'Store TerraMA2 data model version';
COMMENT ON COLUMN terrama2.version.version IS 'Data model version';
COMMENT ON COLUMN terrama2.version.description IS 'Description of this version';

CREATE TABLE terrama2.users
(
  id        SERIAL       PRIMARY KEY,
  login     VARCHAR(20)  NOT NULL UNIQUE,
  password  VARCHAR(20)  NOT NULL,
  name      VARCHAR(200) NOT NULL,
  email     VARCHAR(50)  NOT NULL,
  cell_phone VARCHAR(20)  NOT NULL
);

COMMENT ON TABLE terrama2.users IS 'Store information about TerraMA2 users';
COMMENT ON COLUMN terrama2.users.id IS 'User identifier';
COMMENT ON COLUMN terrama2.users.login IS 'User login';
COMMENT ON COLUMN terrama2.users.password IS 'User password';
COMMENT ON COLUMN terrama2.users.name IS 'User name';
COMMENT ON COLUMN terrama2.users.email IS 'User e-mail';
COMMENT ON COLUMN terrama2.users.cell_phone IS 'User cell phone';

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

COMMENT ON TABLE terrama2.data_provider IS 'Store information about TerraMA2 data providers (remote servers, ftp servers, web services)';
COMMENT ON COLUMN terrama2.data_provider.id IS 'Data Provider identifier';
COMMENT ON COLUMN terrama2.data_provider.name IS 'A name used to refer to the data server';
COMMENT ON COLUMN terrama2.data_provider.description IS 'A brief description about the dara server';
COMMENT ON COLUMN terrama2.data_provider.kind IS 'The identifier of data server type';
COMMENT ON COLUMN terrama2.data_provider.uri IS 'An URI describing how to access the data provider';
COMMENT ON COLUMN terrama2.data_provider.active IS 'A true value indicates that the server is active and must be checked periodically';

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
  data_frequency            INTEGER,
  schedule                  TIME,
  schedule_retry            INTEGER,
  schedule_timeout          INTEGER,
  CONSTRAINT fk_dataset_data_provider_id
           FOREIGN KEY(data_provider_id)
           REFERENCES terrama2.data_provider(id)
           ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE terrama2.dataset_metadata
(
  id     SERIAL PRIMARY KEY,
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
  id     SERIAL NOT NULL PRIMARY KEY,
  script TEXT,
  dataset_id INTEGER,
  CONSTRAINT fk_dataset_collect_rule_dataset_id
    FOREIGN KEY(dataset_id)
    REFERENCES terrama2.dataset(id)
    ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE terrama2.dataset_item_type
(
  id          SERIAL NOT NULL PRIMARY KEY,
  name        VARCHAR(50) NOT NULL UNIQUE,
  description TEXT
);

INSERT INTO terrama2.dataset_item_type(name, description)
     VALUES('PCD-INPE', 'INPE Format'),
           ('PCD-TOA5', 'TOA5'),
           ('FIRE POINTS', 'Occurrence of fire'),
           ('DISEASE OCCURRENCE', 'Occurrence of diseases');


CREATE TABLE terrama2.dataset_item
(
  id           SERIAL NOT NULL PRIMARY KEY,
  kind         INTEGER NOT NULL,
  active       BOOLEAN,
  dataset_id   INTEGER,
  mask         VARCHAR(255),
  timezone     text DEFAULT '+00:00',
  CONSTRAINT fk_dataset_data_type_id
    FOREIGN KEY(kind)
    REFERENCES terrama2.dataset_item_type(id)
    ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT fk_data_dataset_id
    FOREIGN KEY(dataset_id)
    REFERENCES terrama2.dataset(id)
    ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE terrama2.filter
(
  dataset_item_id                   INTEGER NOT NULL PRIMARY KEY,
  discard_before                    TIMESTAMP,
  discard_after                     TIMESTAMP,
  geom                              GEOMETRY(POLYGON, 4326),
  external_dataset_item_id          INTEGER,
  by_value                          NUMERIC,
  by_value_type                     INTEGER,
  within_external_dataset_item_id   INTEGER,
  band_filter                       TEXT,
  CONSTRAINT fk_filter_dataset_item_id FOREIGN KEY(dataset_item_id) REFERENCES terrama2.dataset_item (id) ON UPDATE CASCADE ON DELETE CASCADE
  --CONSTRAINT fk_filter_external_dataset_item_id FOREIGN KEY(external_dataset_item_id) REFERENCES terrama2.??? (id) ON UPDATE CASCADE ON DELETE CASCADE
  --CONSTRAINT fk_filter_within_by_value_type FOREIGN KEY(by_value_type) REFERENCES terrama2.???? (id) ON UPDATE CASCADE ON DELETE CASCADE
  --CONSTRAINT fk_filter_within_external_dataset_item_id FOREIGN KEY(within_external_dataset_item_id) REFERENCES terrama2.??? (id) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE terrama2.data_collection_log
(
  id SERIAL NOT NULL,
  dataset_item_id INTEGER NOT NULL,
  uri VARCHAR(255) NOT NULL,
  data_timestamp timestamp without time zone NOT NULL,
  collect_timestamp timestamp without time zone NOT NULL DEFAULT NOW(),
  CONSTRAINT fk_data_collection_log_dataset_item_id
    FOREIGN KEY(dataset_item_id)
    REFERENCES terrama2.dataset_item (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

COMMENT ON TABLE terrama2.data_collection_log IS 'Table used to register all collected data';
COMMENT ON COLUMN terrama2.data_collection_log.id IS 'Log identifier';
COMMENT ON COLUMN terrama2.data_collection_log.dataset_item_id IS 'Data identifier';
COMMENT ON COLUMN terrama2.data_collection_log.uri IS 'URI to the collected data';
COMMENT ON COLUMN terrama2.data_collection_log.data_timestamp IS 'Date of the generated data';
COMMENT ON COLUMN terrama2.data_collection_log.collect_timestamp IS 'Date of the collection by TerraMA';


CREATE TABLE terrama2.pcd
(
  dataset_item_id       SERIAL NOT NULL PRIMARY KEY,
  location      GEOMETRY(Point,4326),
  table_name    VARCHAR(50) NOT NULL,
  CONSTRAINT fk_pcd_dataset_item_id
    FOREIGN KEY(dataset_item_id)
    REFERENCES terrama2.dataset_item (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE terrama2.pcd_attribute_type
(
  id          SERIAL  NOT NULL PRIMARY KEY,
  name        VARCHAR(50) UNIQUE,
  description TEXT
);

CREATE TABLE terrama2.pcd_attributes
(
  id      SERIAL  NOT NULL PRIMARY KEY,
  dataset_item_id INTEGER NOT NULL,
  attr_name VARCHAR(50) NOT NULL,
  attr_type_id INTEGER NOT NULL,
  CONSTRAINT fk_pcd_attributes_dataset_item_id
    FOREIGN KEY(dataset_item_id)
    REFERENCES terrama2.pcd (dataset_item_id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_pcd_attributes_attr_type_id
    FOREIGN KEY(attr_type_id)
    REFERENCES terrama2.pcd_attribute_type(id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

COMMIT;