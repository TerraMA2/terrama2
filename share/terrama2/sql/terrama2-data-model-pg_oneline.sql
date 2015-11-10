--
-- TerraMA2 Data Model for PostgreSQL DBMS
--
BEGIN TRANSACTION;

CREATE SCHEMA terrama2;

COMMENT ON SCHEMA terrama2 IS 'TerraMA2 Schema';

CREATE TABLE terrama2.version(version VARCHAR(10), description TEXT);

COMMENT ON TABLE terrama2.version IS 'Store TerraMA2 data model version';
COMMENT ON COLUMN terrama2.version.version IS 'Data model version';
COMMENT ON COLUMN terrama2.version.description IS 'Description of this version';

CREATE TABLE terrama2.users(id SERIAL PRIMARY KEY, login VARCHAR(20)  NOT NULL UNIQUE, password  VARCHAR(20)  NOT NULL, name VARCHAR(200) NOT NULL, email VARCHAR(50)  NOT NULL, cell_phone VARCHAR(20)  NOT NULL);

COMMENT ON TABLE terrama2.users IS 'Store information about TerraMA2 users';
COMMENT ON COLUMN terrama2.users.id IS 'User identifier';
COMMENT ON COLUMN terrama2.users.login IS 'User login';
COMMENT ON COLUMN terrama2.users.password IS 'User password';
COMMENT ON COLUMN terrama2.users.name IS 'User name';
COMMENT ON COLUMN terrama2.users.email IS 'User e-mail';
COMMENT ON COLUMN terrama2.users.cell_phone IS 'User cell phone';

CREATE TABLE terrama2.data_provider_type ( id SERIAL PRIMARY KEY, name VARCHAR(50) UNIQUE, description TEXT);

COMMENT ON TABLE terrama2.data_provider_type IS 'Stores the data server type';
COMMENT ON COLUMN terrama2.data_provider_type.id IS 'Type identifier';
COMMENT ON COLUMN terrama2.data_provider_type.name IS 'Name that identifies the data server type';
COMMENT ON COLUMN terrama2.data_provider_type.description IS 'Description of the data server type';

INSERT INTO terrama2.data_provider_type (name, description) VALUES ('FTP', 'File Transfer Protocol'), ('HTTP', 'Hyper Text Transfer Protocol'), ('FILE', 'Local File System'), ('WFS', 'OGC Web Feature Service'), ('WCS', 'OGC Web Coverage Service'), ('SOS', 'OGC Sensor Observation Service'), ('POSTGIS', 'PostgreSQL/POSTGIS database');

CREATE TABLE terrama2.data_provider ( id SERIAL NOT NULL PRIMARY KEY, name VARCHAR(60) NOT NULL UNIQUE, description TEXT, kind INTEGER NOT NULL, uri  TEXT, active BOOLEAN NOT NULL, CONSTRAINT fk_data_provider_kind FOREIGN KEY(kind) REFERENCES terrama2.data_provider_type(id) ON UPDATE CASCADE ON DELETE RESTRICT);

COMMENT ON TABLE terrama2.data_provider IS 'Store information about TerraMA2 data providers (remote servers, ftp servers, web services)';
COMMENT ON COLUMN terrama2.data_provider.id IS 'Data Provider identifier';
COMMENT ON COLUMN terrama2.data_provider.name IS 'A name used to refer to the data server';
COMMENT ON COLUMN terrama2.data_provider.description IS 'A brief description about the data server';
COMMENT ON COLUMN terrama2.data_provider.kind IS 'The identifier of data server type';
COMMENT ON COLUMN terrama2.data_provider.uri IS 'An URI describing how to access the data provider';
COMMENT ON COLUMN terrama2.data_provider.active IS 'A true value indicates that the server is active and must be checked periodically';

CREATE TABLE terrama2.dataset_type ( id SERIAL NOT NULL PRIMARY KEY, name VARCHAR(50) NOT NULL UNIQUE, description TEXT);

COMMENT ON TABLE terrama2.dataset_type IS 'Stores the dataset type';
COMMENT ON COLUMN terrama2.dataset_type.id IS 'Type identifier';
COMMENT ON COLUMN terrama2.dataset_type.name IS 'Name that identifies the dataset type';
COMMENT ON COLUMN terrama2.dataset_type.description IS 'Brief description about the dataset type';

INSERT INTO terrama2.dataset_type (name, description) VALUES ('PCD', 'Identifies a PCD dataset'), ('Occurrence', 'Identifies a dataset for occurrences'), ('Grid', 'Identifies a grid dataset');

CREATE TABLE terrama2.dataset ( id SERIAL NOT NULL PRIMARY KEY, name VARCHAR(20) NOT NULL UNIQUE, description  TEXT, active BOOLEAN, data_provider_id INTEGER NOT NULL, kind INTEGER NOT NULL, data_frequency INTEGER, schedule TIME, schedule_retry INTEGER, schedule_timeout INTEGER, CONSTRAINT fk_dataset_data_provider_id FOREIGN KEY(data_provider_id) REFERENCES terrama2.data_provider(id) ON UPDATE CASCADE ON DELETE CASCADE);

COMMENT ON TABLE terrama2.dataset IS 'Stores information about the dataset (PCD, Occurrence or Grid)';
COMMENT ON COLUMN terrama2.dataset.id IS 'Dataset identifier';
COMMENT ON COLUMN terrama2.dataset.name IS 'A name used to refer to the dataset';
COMMENT ON COLUMN terrama2.dataset.description IS 'A brief description about the dataset';
COMMENT ON COLUMN terrama2.dataset.active IS 'A true value indicates that the dataset is active and must be checked periodically';
COMMENT ON COLUMN terrama2.dataset.data_provider_id IS 'Data provider identifier';
COMMENT ON COLUMN terrama2.dataset.kind IS 'The identifier of dataset type';
COMMENT ON COLUMN terrama2.dataset.data_frequency IS 'Frequency to check for a new data in seconds';
COMMENT ON COLUMN terrama2.dataset.schedule IS 'Time scheduled to check for a new data';
COMMENT ON COLUMN terrama2.dataset.schedule_retry IS 'Frequency in seconds to retry a collection when the data was not available in the scheduled time';
COMMENT ON COLUMN terrama2.dataset.schedule_timeout IS 'Timeout in seconds for retrying a scheduled collection';


CREATE TABLE terrama2.dataset_metadata ( id SERIAL PRIMARY KEY, key  VARCHAR(50), value  VARCHAR(50), dataset_id INTEGER, CONSTRAINT fk_dataset_metadata_dataset_id FOREIGN KEY(dataset_id) REFERENCES terrama2.dataset(id) ON UPDATE CASCADE ON DELETE CASCADE);

COMMENT ON TABLE terrama2.dataset_metadata IS 'Stores metadata from a dataset';
COMMENT ON COLUMN terrama2.dataset_metadata.id IS 'Metadata identifier';
COMMENT ON COLUMN terrama2.dataset_metadata.key IS 'Metadata key';
COMMENT ON COLUMN terrama2.dataset_metadata.value IS 'Metadata value';
COMMENT ON COLUMN terrama2.dataset_metadata.dataset_id IS 'Dataset identifier';


CREATE TABLE terrama2.dataset_collect_rule ( id SERIAL NOT NULL PRIMARY KEY, script TEXT, dataset_id INTEGER, CONSTRAINT fk_dataset_collect_rule_dataset_id FOREIGN KEY(dataset_id) REFERENCES terrama2.dataset(id) ON UPDATE CASCADE ON DELETE CASCADE);

COMMENT ON TABLE terrama2.dataset_collect_rule IS 'Stores metadata from a dataset';
COMMENT ON COLUMN terrama2.dataset_collect_rule.id IS 'Collect rule identifier';
COMMENT ON COLUMN terrama2.dataset_collect_rule.script IS 'Script to be used in the collection';
COMMENT ON COLUMN terrama2.dataset_collect_rule.dataset_id IS 'Dataset identifier';

CREATE TABLE terrama2.dataset_item_type ( id SERIAL NOT NULL PRIMARY KEY, name VARCHAR(50) NOT NULL UNIQUE, description TEXT);

COMMENT ON TABLE terrama2.dataset_item_type IS 'Stores the dataset item type';
COMMENT ON COLUMN terrama2.dataset_item_type.id IS 'Dataset item type identifier';
COMMENT ON COLUMN terrama2.dataset_item_type.name IS 'Name that identifies the dataset item type';
COMMENT ON COLUMN terrama2.dataset_item_type.description IS 'Brief description about the dataset item type';

INSERT INTO terrama2.dataset_item_type(name, description) VALUES('PCD-INPE', 'INPE Format'), ('PCD-TOA5', 'TOA5'), ('FIRE POINTS', 'Occurrence of fire'), ('DISEASE OCCURRENCE', 'Occurrence of diseases');


CREATE TABLE terrama2.dataset_item ( id  SERIAL NOT NULL PRIMARY KEY, kind  INTEGER NOT NULL, active  BOOLEAN, dataset_id INTEGER, mask  VARCHAR(255), timezone text DEFAULT '+00:00', CONSTRAINT fk_dataset_data_type_id FOREIGN KEY(kind) REFERENCES terrama2.dataset_item_type(id) ON UPDATE CASCADE ON DELETE RESTRICT, CONSTRAINT fk_data_dataset_id FOREIGN KEY(dataset_id) REFERENCES terrama2.dataset(id) ON UPDATE CASCADE ON DELETE CASCADE);

COMMENT ON TABLE terrama2.dataset_item IS 'Stores information about the dataset item';
COMMENT ON COLUMN terrama2.dataset_item.id IS 'Dataset item identifier';
COMMENT ON COLUMN terrama2.dataset_item.active IS 'A true value indicates that the dataset item is active and must be checked periodically';
COMMENT ON COLUMN terrama2.dataset_item.dataset_id IS 'Dataset identifier';
COMMENT ON COLUMN terrama2.dataset_item.kind IS 'The identifier of dataset type';
COMMENT ON COLUMN terrama2.dataset_item.mask IS 'Mask to be used in the collection';
COMMENT ON COLUMN terrama2.dataset_item.timezone IS 'Which timezone the data is produced';

CREATE TABLE terrama2.storage_metadata( id SERIAL PRIMARY KEY, key VARCHAR(50), value VARCHAR(50), dataset_item_id INTEGER, CONSTRAINT fk_dataset_metadata_dataset_id FOREIGN KEY(dataset_item_id) REFERENCES terrama2.dataset_item(id) ON UPDATE CASCADE ON DELETE CASCADE);

COMMENT ON TABLE terrama2.storage_metadata IS 'Stores metadata of the storage strategy';
COMMENT ON COLUMN terrama2.storage_metadata.id IS 'Metadata identifier';
COMMENT ON COLUMN terrama2.storage_metadata.key IS 'Metadata key';
COMMENT ON COLUMN terrama2.storage_metadata.value IS 'Metadata value';
COMMENT ON COLUMN terrama2.storage_metadata.dataset_item_id IS 'Dataset item identifier';

CREATE TABLE terrama2.filter_expression_type(id SERIAL NOT NULL PRIMARY KEY, name VARCHAR(50) NOT NULL UNIQUE, description TEXT);


COMMENT ON TABLE terrama2.filter_expression_type IS 'Stores the filter by value type';
COMMENT ON COLUMN terrama2.filter_expression_type.id IS 'Filter by value identifier';
COMMENT ON COLUMN terrama2.filter_expression_type.name IS 'Name that identifies the type of filter';
COMMENT ON COLUMN terrama2.filter_expression_type.description IS 'Brief description about the filter type';

INSERT INTO terrama2.filter_expression_type(name, description) VALUES('NONE_TYPE', 'None'), ('LESS_THAN_TYPE', 'Eliminate data when all values are less than a given value'), ('GREATER_THAN_TYPE', 'Eliminate data when all values are greater than a given value'), ('MEAN_LESS_THAN_TYPE', 'Eliminate data when the mean is less than a given value'), ('MEAN_GREATER_THAN_TYPE', 'Eliminate data when mean is greater than a given value');



CREATE TABLE terrama2.filter (dataset_item_id INTEGER NOT NULL PRIMARY KEY, discard_before  TIMESTAMP, discard_after   TIMESTAMP, geom GEOMETRY(POLYGON, 4326), external_data_id INTEGER, value NUMERIC, expression_type INTEGER, within_external_data_id INTEGER, band_filter TEXT, CONSTRAINT fk_filter_dataset_item_id FOREIGN KEY(dataset_item_id) REFERENCES terrama2.dataset_item (id) ON UPDATE CASCADE ON DELETE CASCADE, CONSTRAINT fk_filter_with_expression_type FOREIGN KEY(expression_type) REFERENCES terrama2.filter_expression_type (id) ON UPDATE CASCADE ON DELETE CASCADE);
  --CONSTRAINT fk_filter_external_data_id FOREIGN KEY(external_dataset_item_id) REFERENCES terrama2.??? (id) ON UPDATE CASCADE ON DELETE CASCADE
  --CONSTRAINT fk_filter_within_external_data_id FOREIGN KEY(within_external_dataset_item_id) REFERENCES terrama2.??? (id) ON UPDATE CASCADE ON DELETE CASCADE

COMMENT ON TABLE terrama2.filter IS 'Stores information about the filter to be used for a dataset item';
COMMENT ON COLUMN terrama2.filter.dataset_item_id IS 'Dataset item identifier';
COMMENT ON COLUMN terrama2.filter.discard_before IS 'Initial date of interest';
COMMENT ON COLUMN terrama2.filter.discard_after IS 'Final date of interest';
COMMENT ON COLUMN terrama2.filter.geom IS 'Geometry to filter the area of interest';
COMMENT ON COLUMN terrama2.filter.external_data_id IS 'Identifier of the dataset to be used as area of interest';
COMMENT ON COLUMN terrama2.filter.value IS 'Filter by value';
COMMENT ON COLUMN terrama2.filter.expression_type IS 'Type of filter by value';
COMMENT ON COLUMN terrama2.filter.within_external_data_id IS 'Identifier of the dataset to be used as area of interest';
COMMENT ON COLUMN terrama2.filter.band_filter IS 'Bands to exclude from collection';


CREATE TABLE terrama2.data_collection_log ( id SERIAL NOT NULL, dataset_item_id INTEGER NOT NULL, uri VARCHAR(255) NOT NULL, data_timestamp timestamp without time zone NOT NULL, collect_timestamp timestamp without time zone NOT NULL DEFAULT NOW(), CONSTRAINT fk_data_collection_log_dataset_item_id FOREIGN KEY(dataset_item_id) REFERENCES terrama2.dataset_item (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE);

COMMENT ON TABLE terrama2.data_collection_log IS 'Store the log of all collected data';
COMMENT ON COLUMN terrama2.data_collection_log.id IS 'Log identifier';
COMMENT ON COLUMN terrama2.data_collection_log.dataset_item_id IS 'Dataset item identifier';
COMMENT ON COLUMN terrama2.data_collection_log.uri IS 'URI to the collected data';
COMMENT ON COLUMN terrama2.data_collection_log.data_timestamp IS 'Date of the generated data';
COMMENT ON COLUMN terrama2.data_collection_log.collect_timestamp IS 'Date of the collection by TerraMA';



COMMIT;