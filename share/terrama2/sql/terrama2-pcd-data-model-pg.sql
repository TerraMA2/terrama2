--
-- TerraMA2 Data Collection Plataform Data Model for PostgreSQL DBMS
--

BEGIN TRANSACTION;

CREATE TABLE terrama2.dcp
(
  dataset_item_id       SERIAL NOT NULL PRIMARY KEY,
  location      GEOMETRY(Point,4326),
  table_name    VARCHAR(50) NOT NULL,
  CONSTRAINT fk_dcp_dataset_item_id
    FOREIGN KEY(dataset_item_id)
    REFERENCES terrama2.dataset_item (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE terrama2.dcp_attribute_type
(
  id          SERIAL  NOT NULL PRIMARY KEY,
  name        VARCHAR(50) UNIQUE,
  description TEXT
);

CREATE TABLE terrama2.dcp_attributes
(
  id      SERIAL  NOT NULL PRIMARY KEY,
  dataset_item_id INTEGER NOT NULL,
  attr_name VARCHAR(50) NOT NULL,
  attr_type_id INTEGER NOT NULL,
  CONSTRAINT fk_dcp_attributes_dataset_item_id
    FOREIGN KEY(dataset_item_id)
    REFERENCES terrama2.dcp (dataset_item_id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_dcp_attributes_attr_type_id
    FOREIGN KEY(attr_type_id)
    REFERENCES terrama2.dcp_attribute_type(id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);

COMMIT;