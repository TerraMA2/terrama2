
CREATE TABLE terrama2.schedule (
  "id" serial,
  "frequency" integer,
  "frequency_unit" varchar(50),
  "schedule" timestamp with time zone,
  "schedule_retry" integer,
  "schedule_retry_unit" varchar(50),
  "schedule_timeout" integer,
  "schedule_timeout_unit" varchar(50),
  PRIMARY KEY ("id")
);



CREATE TABLE terrama2.collector (
  "id" serial,
  "data_series_input" integer,
  "data_series_output" integer,
  "project" integer,
  "instance" integer,
  "schedule" integer,
  "active" boolean,
  PRIMARY KEY ("id"),
  UNIQUE(data_series_input, project),
  FOREIGN KEY (data_series_input) REFERENCES terrama2.data_series(id),
  FOREIGN KEY (data_series_output) REFERENCES terrama2.data_series(id),
  FOREIGN KEY (project) REFERENCES terrama2.project(id),
  FOREIGN KEY (instance) REFERENCES terrama2.instance(id),
  FOREIGN KEY (schedule) REFERENCES terrama2.schedule(id)
);

CREATE TABLE terrama2.collector_input_output (
  "id" serial,
  "collector" integer,
  "input_dataset" integer,
  "output_dataset" integer,
  PRIMARY KEY ("id"),
  FOREIGN KEY (collector) REFERENCES terrama2.collector(id),
  FOREIGN KEY (input_dataset) REFERENCES terrama2.dataset(id),
  FOREIGN KEY (output_dataset) REFERENCES terrama2.dataset(id)
);

CREATE TABLE terrama2.log_status (
  "id" serial,
  "status" text NOT NULL,
  PRIMARY KEY ("id")
);

CREATE TABLE terrama2.data_collection_log (
  "id" serial,
  "status" integer,
  "origin_uri" text NOT NULL,
  "storage_uri" text,
  "data_timestamp" timestamp  with time zone,
  "collect_timestamp" timestamp  with time zone,
  "collector_input_output" integer,
  PRIMARY KEY ("id"),
  FOREIGN KEY (status) REFERENCES terrama2.log_status(id),
  FOREIGN KEY (collector_input_output) REFERENCES terrama2.collector_input_output(id)
);

CREATE TABLE terrama2.value_comparison_operation (
  "id" serial,
  "operation" varchar(50),
  PRIMARY KEY ("id")
);

CREATE TABLE terrama2.filter (
  "id" serial,
  "collector" integer,
  "discard_before" timestamp,
  "discard_after" timestamp,
  "region" polygon, --CONSTRAINT  (srid = 4326)
  "by_value" numeric,
  "value_comparison_operation" integer,
  PRIMARY KEY (id),
  FOREIGN KEY (collector) REFERENCES terrama2.collector(id),
  FOREIGN KEY (value_comparison_operation) REFERENCES terrama2.value_comparison_operation(id)
);
