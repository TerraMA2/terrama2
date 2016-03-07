
CREATE SCHEMA terrama2;

CREATE TABLE terrama2.version (
  "id" SERIAL,
  "version" INTEGER,
  "tag" VARCHAR(50),
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE ("tag")
);

CREATE TABLE terrama2.project (
  "id" SERIAL,
  "version" INTEGER,
  "name" VARCHAR(50),
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE(name)
);

CREATE TABLE terrama2.service_type (
  "id" SERIAL,
  "name" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE (name)
);

CREATE TABLE terrama2.service_instance (
  "id" SERIAL,
  "service_type_id" INTEGER,
  "name" VARCHAR(50),
  "host" VARCHAR(50),
  "port" INTEGER,
  "ssh_user" VARCHAR(50),
  "ssh_port" INTEGER,
  "description" TEXT,
  "path_to_binary" TEXT,
  "number_of_threads" INTEGER,
  "run_environment" TEXT,
  PRIMARY KEY ("id"),
  FOREIGN KEY(service_type_id) REFERENCES terrama2.service_type(id)
);

CREATE TABLE terrama2.data_provider_type (
  "id" SERIAL,
  "name" VARCHAR(50),
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE(name)
);

CREATE TABLE terrama2.data_provider_intent (
  "id" SERIAL,
  "name" VARCHAR(50),
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE(name)
);

CREATE TABLE terrama2.data_provider (
  "project_id" INTEGER,
  "name" VARCHAR(50),
  "description" TEXT,
  "data_provider_type_id" INTEGER,
  "data_provider_intent_id" INTEGER,
  "uri" TEXT,
  "active" BOOLEAN,
  "id" SERIAL,
  PRIMARY KEY ("id"),
  FOREIGN KEY(project_id) REFERENCES terrama2.project(id),
  FOREIGN KEY(data_provider_type_id) REFERENCES terrama2.data_provider_type(id),
  FOREIGN KEY(data_provider_intent_id) REFERENCES terrama2.data_provider_intent(id),
  UNIQUE(project_id, name)
);

CREATE TABLE terrama2.data_series_type (
  "id" SERIAL,
  "name" VARCHAR(50),
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE ("name")
);

CREATE TABLE terrama2.data_formats (
  "id" SERIAL,
  "name" VARCHAR(50),
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE ("name")
);

CREATE TABLE terrama2.data_series_semantics (
  "id" SERIAL,
  "data_format_id" INTEGER,
  "data_series_type_id" INTEGER,
  "name" VARCHAR(50),
  PRIMARY KEY ("id"),
  UNIQUE ("name"),
  FOREIGN KEY(data_format_id) REFERENCES terrama2.data_formats(id),
  FOREIGN KEY(data_series_type_id) REFERENCES terrama2.data_series_type(id)
);

CREATE TABLE terrama2.data_series (
  "data_provider_id" INTEGER,
  "data_series_semantics_id" INTEGER,
  "id" SERIAL,
  "name" TEXT,
  "description" TEXT,
  PRIMARY KEY ("id"),
  FOREIGN KEY(data_provider_id) REFERENCES terrama2.data_provider(id),
  FOREIGN KEY(data_series_semantics_id) REFERENCES terrama2.data_series_semantics(id),
  UNIQUE (name)
);

CREATE TABLE terrama2.dataset (
  "id" SERIAL,
  "data_series_id" INTEGER,
  "active" BOOLEAN,
  PRIMARY KEY ("id"),
  FOREIGN KEY(data_series_id) REFERENCES terrama2.data_series(id)
);

CREATE TABLE terrama2.data_series_properties (
  "id" INTEGER,
  "data_series_id" INTEGER,
  "attribute" TEXT,
  "alias" TEXT,
  "position" INTEGER,
  PRIMARY KEY ("id"),
  FOREIGN KEY(data_series_id) REFERENCES terrama2.data_series(id),
  UNIQUE(data_series_id, attribute),
  UNIQUE(data_series_id, alias)
);
