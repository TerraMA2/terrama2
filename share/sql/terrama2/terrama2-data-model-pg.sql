--
-- TerraMA2 Data Model for PostgreSQL DBMS
--

CREATE SCHEMA terrama2 AUTHORIZATION postgres;

COMMENT ON SCHEMA terrama2 IS 'Schema used to store all objects related to TerraMA';


CREATE TABLE terrama2.users
(
  id SERIAL,
  login character varying(20) NOT NULL,
  password character varying(20) NOT NULL,
  name  character varying(200) NOT NULL,
  email character varying(50) NOT NULL,
  cellphone character varying(20) NOT NULL,
  CONSTRAINT pk_users_id PRIMARY KEY (id),
  CONSTRAINT uk_users_login UNIQUE (login)
);

COMMENT ON TABLE terrama2.users IS 'Table used to store the users of TerraMA';
COMMENT ON COLUMN terrama2.users.id IS 'Identifier of an user';
COMMENT ON COLUMN terrama2.users.login IS 'Login used to access the system';
COMMENT ON COLUMN terrama2.users.password IS 'Password used to access the system';
COMMENT ON COLUMN terrama2.users.name IS 'Full username';
COMMENT ON COLUMN terrama2.users.cellphone IS 'Cellphone number used to send notifications';


CREATE TABLE terrama2.collector_roles
(
  id serial NOT NULL,
  role character varying(20) NOT NULL,
  description TEXT NOT NULL,
  CONSTRAINT pk_collector_roles_id PRIMARY KEY (id)
);

COMMENT ON TABLE terrama2.collector_roles IS 'Table used to store the roles used when sharing a collector in a multi-user environment';
COMMENT ON COLUMN terrama2.collector_roles.id IS 'Identifier of a role';
COMMENT ON COLUMN terrama2.collector_roles.role IS 'Role name';
COMMENT ON COLUMN terrama2.collector_roles.description IS 'Role description';

CREATE TABLE terrama2.collector
(
  id serial NOT NULL,
  name character varying(20) NOT NULL,
  description text,
  type character varying(20) NOT NULL,
  update_frequency_minutes INTEGER NOT NULL,
  prefix character varying(10),
  projection_id integer NOT NULL,
  mask character varying(255) NOT NULL,
  unit character varying(20),
  timezone character varying(10) NOT NULL DEFAULT '+00:00',
  format text NOT NULL,

  CONSTRAINT pk_collector_id PRIMARY KEY (id),
  CONSTRAINT uk_name UNIQUE (name),
  CONSTRAINT fk_collector_projection FOREIGN KEY (projection_id)
    REFERENCES public.spatial_ref_sys (srid) MATCH SIMPLE
    ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT format_check CHECK ((format =
    ANY (ARRAY[ 'ASCII-Grid'::text, 'PCD'::text, 'TIFF'::text, 'GrADS'::text, 'Additional Map'::text,
          'PROARCO File'::text, 'Model'::text, 'OGC WCS'::text, 'PCD OGC WFS'::text, 'Surface'::text, 'Instrument'::text])))

--  CONSTRAINT type_check CHECK ((type = ANY (ARRAY[SERIES DE DADOS ACEITAS]))),
);

COMMENT ON TABLE terrama2.collector IS 'Table used to store the collector information';
COMMENT ON COLUMN terrama2.collector.id IS 'Identifier of a collector';
COMMENT ON COLUMN terrama2.collector.name IS 'Unique name of the collector';
COMMENT ON COLUMN terrama2.collector.description IS 'Description of the collector';
COMMENT ON COLUMN terrama2.collector.type IS 'Data serie type';
COMMENT ON COLUMN terrama2.collector.update_frequency_minutes IS 'Time in minutes between data aquisition';
COMMENT ON COLUMN terrama2.collector.prefix IS 'Prefix for the data tables or files created to store aquired data';
COMMENT ON COLUMN terrama2.collector.projection_id IS 'SRID of the projection';
COMMENT ON COLUMN terrama2.collector.mask IS 'Mask of filename format to aquire';
COMMENT ON COLUMN terrama2.collector.unit IS 'Unit of aquired data';
COMMENT ON COLUMN terrama2.collector.timezone IS 'Timezone of stored date';
COMMENT ON COLUMN terrama2.collector.format IS 'Data format type';


CREATE TABLE terrama2.user_collector
(
  user_id integer NOT NULL,
  collector_id integer NOT NULL,
  role_id integer NOT NULL,
  CONSTRAINT pk_user_collector PRIMARY KEY (user_id, collector_id),
  CONSTRAINT fk_collector_id FOREIGN KEY (collector_id)
      REFERENCES terrama2.collector (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_collector_roles_id FOREIGN KEY (role_id)
      REFERENCES terrama2.collector_roles (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE RESTRICT,
  CONSTRAINT fk_user_id FOREIGN KEY (user_id)
      REFERENCES terrama2.users (id) MATCH SIMPLE
      ON UPDATE CASCADE ON DELETE CASCADE
);

COMMENT ON TABLE terrama2.user_collector IS 'Table used to store what role a user has in a collector';
COMMENT ON COLUMN terrama2.user_collector.user_id IS 'Identifier of a user';
COMMENT ON COLUMN terrama2.user_collector.collector_id IS 'Identifier of a collector';
COMMENT ON COLUMN terrama2.user_collector.role_id IS 'Role of the user in the collector';


CREATE TABLE terrama2.connection
(
  id SERIAL NOT NULL,
  name character varying(60) NOT NULL,
  protocol character varying(4),
  url character varying(255),
  username character varying(50),
  password character varying(50),
  port integer,
  description text,
  active boolean NOT NULL,
  base_path character varying(255),
  CONSTRAINT connection_protocol_check CHECK (((protocol)::text = ANY ((ARRAY['FTP'::character varying, 'HTTP'::character varying, 'FILE'::character varying, 'OGC'::character varying])::text[]))),
  CONSTRAINT pk_connection PRIMARY KEY(id),
  CONSTRAINT uk_connection_name UNIQUE (name)
);


COMMENT ON TABLE terrama2.connection IS 'Table used to store the connection parameters with a server';
COMMENT ON COLUMN terrama2.connection.id IS 'Identifier of a connection';
COMMENT ON COLUMN terrama2.connection.name IS 'Name of the connection, must be unique';
COMMENT ON COLUMN terrama2.connection.protocol IS 'Defines which protocol of communication will be used, e.g. FTP, HTTP, File or OGC';
COMMENT ON COLUMN terrama2.connection.url IS 'Defines the server address, for File protocol should be the base file path';
COMMENT ON COLUMN terrama2.connection.username IS 'Username to be used in order to access a FTP server';
COMMENT ON COLUMN terrama2.connection.password IS 'Password to be used in order to access a FTP server';
COMMENT ON COLUMN terrama2.connection.port IS 'Port to be user in order to access a FTP server';
COMMENT ON COLUMN terrama2.connection.description IS 'Description of the server';
COMMENT ON COLUMN terrama2.connection.active IS 'Defines if the server is active and should be checked periodically';
COMMENT ON COLUMN terrama2.connection.base_path IS 'Optional parameter to indicate a base path to a FTP server';




CREATE TABLE terrama2.collector_connection
(
  collector_id INTEGER NOT NULL,
  connection_id INTEGER NOT NULL,
  CONSTRAINT fk_collector_connection_id FOREIGN KEY (collector_id) REFERENCES terrama2.collector (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_ccollector_connection_id FOREIGN KEY (connection_id) REFERENCES terrama2.connection (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT pk_collector_connection PRIMARY KEY (collector_id, connection_id)
);

COMMENT ON TABLE terrama2.collector_connection IS 'Table used to link a connection with mutiple collectors';
COMMENT ON COLUMN terrama2.collector_connection.collector_id IS 'Collector identifier';
COMMENT ON COLUMN terrama2.collector_connection.connection_id IS 'Connection identifier';


CREATE TABLE terrama2.interpolator
(
  id SERIAL NOT NULL,
  attribute_name text,
  grid_output_name text,
  method text,
  number_neighbors integer,
  pow_value integer,
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


CREATE TABLE terrama2.collector_interpolator
(
  collector_id integer NOT NULL,
  interpolator_id integer NOT NULL,
  CONSTRAINT fk_collector_interpolator_collector_id FOREIGN KEY(collector_id) REFERENCES terrama2.collector(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_collector_interpolator_interpolator_id FOREIGN KEY(interpolator_id ) REFERENCES terrama2.interpolator(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT pk_collector_interpolator PRIMARY KEY (collector_id, interpolator_id)
);

COMMENT ON TABLE terrama2.collector_interpolator IS 'Table used to link multiple interpolators with a collector';
COMMENT ON COLUMN terrama2.collector_interpolator.collector_id IS 'Collector identifier';
COMMENT ON COLUMN terrama2.collector_interpolator.interpolator_id IS 'Interpolator identifier';


CREATE TABLE terrama2.version
(
  version character varying(10),
  description text,
  study boolean
);

COMMENT ON TABLE terrama2.version IS 'Table used to store the database version';
COMMENT ON COLUMN terrama2.version.version IS 'Number of the version';
COMMENT ON COLUMN terrama2.version.description IS 'Description of this version';
COMMENT ON COLUMN terrama2.version.study IS 'Defines if this is a study database, it is used to verify the behavior of an analysis';

CREATE TABLE terrama2.collector_log
(
  id SERIAL NOT NULL,
  collector_id INTEGER NOT NULL,
  data_path character varying(255) NOT NULL,
  date_time_file timestamp without time zone NOT NULL,
  date_time_collection timestamp without time zone NOT NULL DEFAULT NOW(),
  status character varying(20),
  CONSTRAINT pk_collector_log PRIMARY KEY(id),
  CONSTRAINT fk_collector_log_collector_id FOREIGN KEY(collector_id) REFERENCES terrama2.collector (id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

COMMENT ON TABLE terrama2.collector_log IS 'Table used to register all collected data';
COMMENT ON COLUMN terrama2.collector_log.id IS 'Log identifier';
COMMENT ON COLUMN terrama2.collector_log.data_path IS 'Path to the collected data';
COMMENT ON COLUMN terrama2.collector_log.date_time_file IS 'Date of the generated data';
COMMENT ON COLUMN terrama2.collector_log.date_time_collection IS 'Date of the collection by TerraMA';
COMMENT ON COLUMN terrama2.collector_log.status IS 'Status of the collection';


CREATE TABLE terrama2.archiving_rules
(
  id serial NOT NULL,
  collector_id integer,
  analysis_id integer,
  type character varying(50) NOT NULL,
  action character varying(50) NOT NULL,
  condition integer NOT NULL,
  create_filter boolean NOT NULL,
  CONSTRAINT pk_archiving_rules_id PRIMARY KEY (id),
  CONSTRAINT fk_archiving_rules_collector_id FOREIGN KEY(collector_id) REFERENCES terrama2.collector(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
--  CONSTRAINT fk_archiving_rules_analysis_id FOREIGN KEY(analysis_id) REFERENCES terrama2.analysis(id) MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);


COMMENT ON TABLE terrama2.archiving_rules IS 'Table used to store the database version';
COMMENT ON COLUMN terrama2.archiving_rules.id IS 'Identifier of the archiving rule';
COMMENT ON COLUMN terrama2.archiving_rules.collector_id IS 'Collector identifier, used when the rule is pointed to a collector';
COMMENT ON COLUMN terrama2.archiving_rules.analysis_id IS 'Analysis identifier, used when the rule is pointed to a analysis';
COMMENT ON COLUMN terrama2.archiving_rules.type IS 'Rule type, possible values: DATASOURCE or ANALYSIS';
COMMENT ON COLUMN terrama2.archiving_rules.action IS 'Defines strategy to archive the data, possible values: DELETE_DATA, DELETE_LOG, DELETE_WARNING and DELETE_SURFACE';
COMMENT ON COLUMN terrama2.archiving_rules.condition IS 'Defines which data from the collector or analysis will be considered by the rule';
COMMENT ON COLUMN terrama2.archiving_rules.create_filter IS 'Filter valid only for the type datasource, it will impact the filter of the collector, it will change for the date of the execution of the archiving';


