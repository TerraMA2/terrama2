CREATE TABLE terrama2.data_provider_scheme (
  "id" SERIAL,
  "name" VARCHAR(50) NOT NULL,
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE(name)
);

CREATE TABLE terrama2.data_provider_type (
  "id" SERIAL,
  "name" VARCHAR(50) NOT NULL,
  "description" TEXT,
  PRIMARY KEY ("id"),
  UNIQUE(name)
);

CREATE TABLE terrama2.data_series_type (
  "id" serial,
  "name" varchar(50) NOT NULL,
  "description" text,
  PRIMARY KEY ("id")
);

CREATE TABLE terrama2.data_provider (
  "project" integer,
  "name" varchar(60) NOT NULL,
  "description" text,
  "data_provider_scheme" integer,
  "data_provider_type" integer,
  "uri" text NOT NULL,
  "active" boolean NOT NULL,
  "id" serial,
  PRIMARY KEY ("id"),
  FOREIGN KEY(project) REFERENCES terrama2.project(id),
  FOREIGN KEY(data_provider_scheme) REFERENCES terrama2.data_provider_scheme(id),
  FOREIGN KEY(data_provider_type) REFERENCES terrama2.data_provider_type(id),
  UNIQUE(project, name)
);

CREATE TABLE terrama2.data_series (
  "data_provider" integer,
  "id" serial,
  "name" text NOT NULL,
  "description" text,
  "data_series_type" integer,
  PRIMARY KEY ("id"),
  FOREIGN KEY(data_provider) REFERENCES terrama2.data_provider(id),
  FOREIGN KEY(data_series_type) REFERENCES terrama2.data_series_type(id),
  UNIQUE (name)
);

CREATE TABLE terrama2.dataset (
  "id" serial,
  "data_series" integer,
  "mask" varchar(255) NOT NULL,
  "path" varchar(255),
  "active" boolean NOT NULL,
  "timezone" text default '+00:00' NOT NULL,
  "srid" integer,
  PRIMARY KEY ("id"),
  FOREIGN KEY(data_series) REFERENCES terrama2.data_series(id),
  FOREIGN KEY(srid) REFERENCES spatial_ref_sys(srid)
);

CREATE TABLE terrama2.dataset_properties (
  "id" serial,
  "dataset" integer,
  "attribute" text NOT NULL,
  "alias" text,
  "position" integer NOT NULL,
  PRIMARY KEY ("id"),
  FOREIGN KEY(dataset) REFERENCES terrama2.dataset(id),
  UNIQUE(dataset, attribute),
  UNIQUE(dataset, alias)
);

CREATE TABLE terrama2.dataset_metadata (
  "id" serial,
  "dataset_id" integer,
  "key" varchar(50) NOT NULL,
  "value" text,
  PRIMARY KEY ("id"),
  FOREIGN KEY(dataset_id) REFERENCES terrama2.dataset(id)
);
