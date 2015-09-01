-- CADASTRANDO UM USUARIO
INSERT INTO terrama2.users(login, password, name, email, cellphone)
    VALUES ('user','password','Nome','email@terrama.com.br','55(11)11111-1111');

-- CADASTRANDO UM DATA_PROVIDER
INSERT INTO terrama2.data_provider(name, description, kind, uri, active)
    VALUES ('CPTEC - INPE', 'Os dados de previsões numéricas ....', 1, 'URI', TRUE);

-- CADASTRANDO UNIT_OF_MEASURE
INSERT INTO terrama2.unit_fo_measure(unit)
     VALUES ('MINUTES');

-- CADASTRANDO DATASET
INSERT INTO terrama2.dataset(name, description, active, data_provider_id, kind, data_frequency, data_frequency_unit_id, schedule, schedule_retry, schedule_retry_unit_id, schedule_timeout, schedule_timeout_unit_id)
     VALUES ('Focos_novo', 'Focos de queimadas', TRUE, 1, 2, 5, 1, NULL, NULL, NULL, NULL, NULL);

INSERT INTO terramas2.dataset_metadata(key, value, dataset_id)
     VALUES ('metadado1', 'valor1', 1),('metadado2', 'valor2', 1);

--INSERT INTO terrma2.data_type(name, description)
--     VALUES( ?????)

INSERT INTO terrama2.data(active, dataset_id, mask, timezone)
     VALUES(????);