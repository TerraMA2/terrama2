CREATE SCHEMA terrama2;

CREATE TABLE terrama2.version (
  "id"          SERIAL,
  "version"     INTEGER NOT NULL,
  "tag"         VARCHAR(50) NOT NULL,
  "description" TEXT NOT NULL,
  PRIMARY KEY ("id"),
  UNIQUE(tag)
);

CREATE TABLE terrama2.project (
  "id"          SERIAL,
  "version"     INTEGER NOT NULL,
  "name"        VARCHAR(50) NOT NULL,
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE(name)
);

CREATE TABLE terrama2.service_type (
  "id"   SERIAL,
  "name" VARCHAR(50) NOT NULL,
  PRIMARY KEY ("id"),
  UNIQUE (name)
);

CREATE TABLE terrama2.service_instance (
  "id"              SERIAL,
  "name"            VARCHAR(50) NOT NULL,
  "host"            VARCHAR(50) NOT NULL
  "port"            INTEGER     NOT NULL,
  "ssh_user"        VARCHAR(50),
  "ssh_port"        INTEGER, 
  "description"     TEXT,
  "service_type_id" INTEGER,
  PRIMARY KEY ("id"),
  FOREIGN KEY(service_type_id) REFERENCES terrama2.service_type(id) MATCHES RESTRICT
);
