-----------------------------------------------------------------------------------------------------------------------
-- 1. Creating the version table
-----------------------------------------------------------------------------------------------------------------------

-- Sequence: terrama2.version_id_seq

CREATE SEQUENCE terrama2.version_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

-- Table: terrama2.version

CREATE TABLE terrama2.version
(
    id integer NOT NULL DEFAULT nextval('terrama2.version_id_seq'::regclass),
    major integer NOT NULL,
    minor integer NOT NULL,
    patch integer NOT NULL,
    tag text NOT NULL,
    database integer NOT NULL,
    insert_time timestamp with time zone NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT version_pkey PRIMARY KEY (id)
);

-- Index: full_database_version

CREATE UNIQUE INDEX full_database_version
    ON terrama2.version USING btree
    (major, minor, database);

-- Index: full_version

CREATE UNIQUE INDEX full_version
    ON terrama2.version USING btree
    (major, minor, patch);

-- Inserting the current version in the database

INSERT INTO terrama2.version (major, minor, patch, tag, database) VALUES (4, 0, 1, "RELEASE", 1);

-----------------------------------------------------------------------------------------------------------------------
-- 2. Replacing the column analysis_id with the column schedule_id in the table terrama2.reprocessing_historical_data
-----------------------------------------------------------------------------------------------------------------------

ALTER TABLE terrama2.reprocessing_historical_data ADD COLUMN schedule_id integer;

UPDATE terrama2.reprocessing_historical_data SET schedule_id=analysis.schedule_id FROM (select id, schedule_id from terrama2.analysis) AS analysis WHERE analysis_id=analysis.id;

ALTER TABLE terrama2.reprocessing_historical_data DROP COLUMN analysis_id;

ALTER TABLE terrama2.reprocessing_historical_data ALTER COLUMN schedule_id SET NOT NULL;

ALTER TABLE terrama2.reprocessing_historical_data ADD CONSTRAINT reprocessing_historical_data_schedule_id_fkey FOREIGN KEY (schedule_id) REFERENCES terrama2.schedules (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE;