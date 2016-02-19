CREATE SCHEMA terrama2;

CREATE TABLE terrama2.version (
  "id" serial,
  "version" integer NOT NULL,
  "tag" varchar(50) NOT NULL,
  "description" text,
  PRIMARY KEY ("id"),
  UNIQUE(tag)
);

CREATE TABLE terrama2.project_version (
  "id" serial,
  "version" integer NOT NULL,
  "tag" varchar(50) NOT NULL,
  PRIMARY KEY ("id"),
  UNIQUE(tag)
);

CREATE TABLE terrama2.project (
  "id" serial,
  "name" varchar(50) NOT NULL,
  "description" text,
  "project_version" integer,
  PRIMARY KEY ("id"),
  FOREIGN KEY(project_version) REFERENCES terrama2.project_version(id),
  UNIQUE(name)
);

CREATE TABLE terrama2.instance_type (
  "id" serial,
  "name" varchar(50) NOT NULL,
  PRIMARY KEY ("id"),
  UNIQUE (name)
);

CREATE TABLE terrama2.instance (
  "id" serial,
  "name"  varchar(50) NOT NULL,
  "description" text,
  "instance_type" integer,
  PRIMARY KEY ("id"),
  FOREIGN KEY(instance_type) REFERENCES terrama2.instance_type(id)
);
