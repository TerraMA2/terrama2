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
    insert_time timestamp with time zone NOT NULL,
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

INSERT INTO terrama2.version (major, minor, patch, tag, database, insert_time) VALUES (4, 0, 1, "RELEASE", 1, NOW());