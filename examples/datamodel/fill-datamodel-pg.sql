-- CADASTRANDO UM USUARIO
INSERT INTO terrama2.users(login, password, name, email, cellphone)
    VALUES ('user','password','Nome','email@terrama.com.br','55(11)11111-1111');
    
-- INSERINDO UM SERVIDOR DE DADOS

-- CADASTRANDO UM DATA_PROVIDER
INSERT INTO terrama2.data_provider(id, name, description, kind, uri, active)
    VALUES (1,'CPTEC - INPE', 'Os dados de previsões numéricas ....', 1, 'URI', TRUE);

-- CADASTRANDO UNIT_OF_MEASURE (cadastrar no script de criação)
INSERT INTO terrama2.unit_of_measure(id, unit)		     
    VALUES (1, 'MINUTE'), (2, 'HOUR'), (3, 'DAY'), (4, 'WEEK');

-- INSERINDO UMA SÉRIE DE DADOS PCD

-- CADASTRANDO DATASET PCD
INSERT INTO terrama2.dataset(id, name, description, active, data_provider_id, kind, data_frequency, data_frequency_unit_id, schedule, schedule_retry, schedule_retry_unit_id, schedule_timeout, schedule_timeout_unit_id)
    VALUES (1, 'PCD', 'PCD INPE', TRUE, 1, 1, 5, 1, NULL, NULL, NULL, NULL, NULL);

INSERT INTO terrama2.dataset_metadata(key, value, dataset_id)
    VALUES ('metadado1', 'valor1', 1),('metadado2', 'valor2', 1);

-- CADASTRANDO UM SCRIPT PARA RODAR DURANTE A COLETA (ligar com o data ou dataset???)
INSERT INTO terrama2.dataset_collect_rule(script, dataset_id)
    VALUES ('script......script', 1);

-- (cadastrar os tipos suportados no script de criação)
INSERT INTO terrama2.data_type(id, name, description)
    VALUES(1, 'PCD-INPE', 'PCDs no formato INPE'), (2, 'PCD-TOA5', 'PCDs no formato TOA5');

-- CADASTRANDO OS ARQUIVOS PCD A SEREM BUSCADOS
INSERT INTO terrama2.data(id, kind, active, dataset_id, mask, timezone)
    VALUES(1, 1, TRUE, 1, 'angra.txt', '+00:00'), (2, 2, TRUE, 1, 'exporta_%A%M%d_%h%m.csv', '+00:00');
    
INSERT INTO terrama2.pcd(data_id, location, table_name)
    VALUES (1, ST_GeomFromText('POINT(-1.5000 29.2200)',4326), 'tabela_pcd_angra'), (2, ST_GeomFromText('POINT(-1.5000 29.2200)',4326), 'tabela_pcd_toa5');
    
--INSERT INTO terrama2.pcd_attributes(data_id, attr_name, attr_type_id)
INSERT INTO terrama2.pcd_attributes(data_id, attr_name, attr_type_id)
    VALUES (1, 'pluvio', 1), (1, 'ar', 1), (1, 'data', 2), (2, 'raios', 1), (2, 'data', 2);
  
-- INSERINDO UM FILTRO PARA O DATA
--INSERT INTO terrama2.filter(data_id, discard_before, discard_after, geom, external_data_id, by_value, by_value_type, within_external_data_id, band_filter)
INSERT INTO terrama2.filter(data_id, discard_before, discard_after, geom, by_value, band_filter)
    VALUES (1, '2015-08-30 00:00:00', '2015-12-30 00:00:00', NULL, NULL, NULL);


-- (colocar também o id do dataset???)
CREATE TABLE terrama2.tabela_pcd_angra_1
( 
    id      SERIAL NOT NULL PRIMARY KEY, 
    pluvio  INTEGER, 
    ar      INTEGER,
    data    TIMESTAMP
);

CREATE TABLE terrama2.tabela_pcd_toa5_2
( 
    id      SERIAL NOT NULL PRIMARY KEY, 
    raios  INTEGER,    
    data    TIMESTAMP
);

-- INSERINDO DADOS NA PCD INPE
INSERT INTO terrama2.tabela_pcd_angra_1(pluvio, ar, data)
    VALUES ( 40, 50, '2015-08-30 01:15:20');
    
-- INSERINDO DADOS NA PCD TOA5
INSERT INTO terrama2.tabela_pcd_toa5_2(raios, data)
    VALUES ( 400, '2009-03-03 01:15:20');
    
    
