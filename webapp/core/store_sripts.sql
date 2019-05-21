// Criar tabela de Armazenamento - "storages" -> Parametro de entrada bamco onde tabela será criada (database_tma)
            
create_storage = """CREATE TABLE %s.storages (
                id serial NOT NULL,
                name character varying(255),
                description text,
                active boolean,
                schedule_type integer,
                filter boolean,
                backup boolean,
                project_id integer NOT NULL,
                data_provider_id integer NOT NULL,
                data_series_id integer NOT NULL,
                schedule_id integer,
                automatic_schedule_id integer,
                CONSTRAINT storages_pkey PRIMARY KEY (id),
                CONSTRAINT storages_automatic_schedule_id_fkey FOREIGN KEY (automatic_schedule_id)
                    REFERENCES %s.automatic_schedules (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE,
                CONSTRAINT storages_data_provider_id_fkey FOREIGN KEY (data_provider_id)
                    REFERENCES %s.data_providers (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE,
                CONSTRAINT storages_data_series_id_fkey FOREIGN KEY (data_series_id)
                    REFERENCES %s.data_series (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE,
                CONSTRAINT storages_project_id_fkey FOREIGN KEY (project_id)
                    REFERENCES %s.projects (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE,
                CONSTRAINT storages_schedule_id_fkey FOREIGN KEY (schedule_id)
                    REFERENCES %s.schedules (id) MATCH SIMPLE
                    ON UPDATE CASCADE ON DELETE CASCADE
                )""" % (database_tma,database_tma,database_tma,database_tma,database_tma,database_tma)

//Tabela com logger
create_storage1 = """CREATE TABLE %s.storage_%d
                    (
                    id serial NOT NULL,
                    process_id integer NOT NULL,
                    status integer NOT NULL,
                    start_timestamp timestamp(1) with time zone,
                    data_timestamp timestamp with time zone,
                    last_process_timestamp timestamp(1) with time zone,
                    data text,
                    CONSTRAINT storage_1_pk PRIMARY KEY (id)
                    )""" % (database_tma, instance_id)

//Verificar se tabela de armazenamento existe
test_storages = "select exists(select * from information_schema.tables where table_name='storages')"

//Criar nova regra de armazenamento
// Entradas:
// database_tma = Nome do banco (terrama2)
// name_storage = nome da regra (default será nome da dataserie de entrada)
// active_storage = regra ativa (sim/não)
// schedule_type_storage = tipo de armazenamento (manual 3, agendado 1)
// backup_storage = faz backup (sim/não)
// project_id_storage = id do projeto
// data_provider_id_storage = id  do servidor de dados para onde será feito o backup
// data_series_id_storage = dataserie onde será aplicada a regra
// schedule_id_storage = regras do aagendamento
new_rule_storage = """insert into %s.storages(
    name,
    active, 
    schedule_type,
    filter, 
    backup,
    project_id,
    data_provider_id,
    data_series_id,
    schedule_id) values 
    ('%s', %s, %d, %s, %s, %d, %d, %d, %d)""" % (
    database_tma,
    name_storage,
    active_storage, 
    schedule_type_storage,
    filter_storage, 
    backup_storage,
    project_id_storage,
    data_provider_id_storage,
    data_series_id_storage,
    schedule_id_storage)

//Criar schedule_id_storage para new_rule_storage
schedule_sql = """insert into %s.schedules
    (frequency,
    frequency_unit,
    frequency_start_time) values
    (%d, '%s', '%s') returning id""" % (
    database_tma,
    requency_storage,
    frequency_unit_storage,
    frequency_start_time_storage)

//Recuperar path para onde os dados serão copiados
//Entrada:
// database_tma = Nome do banco
// id_storage = regra de armazenamento
select_uri_out = """SELECT 
  data_providers.uri
FROM 
  %s.data_providers, 
  %s.storages
WHERE 
  data_providers.id = storages.data_provider_id AND 
  storages.id = %d""" % (database_tma, database_tma, id_storage)

//Verificar qual tipo de regra será aplicado par armazenar tiff ou tabelas simples ou n tabelas
//Entrada:
// database_tma = Nome do banco
// id_storage = regra de armazenamento
select_data_type= """SELECT 
  data_series_semantics.code
FROM 
  %s.storages, 
  %s.data_series, 
  %s.data_series_semantics
WHERE 
  data_series_semantics.id = data_series.data_series_semantics_id AND 
  data_series.id = storages.data_series_id AND 
  storages.id = %d""" % (database_tma,database_tma,database_tma, id_storage)

Tipos válidos para o armazenamento : 
- regra para banco 
DCP-single_table
DCP-postgis
ANALYSIS_MONITORED_OBJECT-postgis
OCCURRENCE-postgis

- regra para arquivos
GRID-geotiff

//Regra para banco
//Recuperar nome da(s) tabela(s)
//Entrada:
// database_tma = Nome do banco
// id_storage = regra de armazenamento
select_table = """SELECT 
    data_set_formats.value
  FROM 
    %s.storages,
    %s.data_sets, 
    %s.data_set_formats, 
    %s.data_series
    WHERE 
    data_series.id = data_sets.data_series_id AND 
    data_set_formats.data_set_id = data_sets.id AND 
    data_set_formats.key = 'table_name' AND
    data_series.id = storages.data_series_id AND
    storages.id = %d""" % (database_tma, database_tma,database_tma,database_tma,id_storage)

//Recuperar nome da tabela se DCP-single_table
id_data_serie = "Select storages.data_series_id from %s.storages where storages.id = %d "% (database_tma,id_storage)
table_name = "dcp_data_%d"%data_serie.id

//Recuperar onde estão as tabelas
select_database = """SELECT 
  data_providers.uri
FROM 
  %s.storages, 
  %s.data_series, 
  %s.data_providers
WHERE 
  data_providers.id = data_series.data_provider_id AND 
  storages.data_series_id = data_series.id AND 
  storages.id = %d;""" %(database_tma, database_tma,database_tma,id_storage)
//Ex: retorna 'postgis://postgres:postgres@127.0.0.1:5432/angra'
//Precisa destrinchar para host, database_server, user, password
// user = postgres
// password = postgres
// host = 127.0.0.1
// database_server = angra
//Vai precisar  conectar ao data_provider (outro banco)

//se backup
copy_data_csv = 



//Regras para tiff

//Recuperar o path de onde os dados serão apagados/copiados (TIFFs) 
//Entrada:
// database_tma = Nome do banco
// id_storage = regra de armazenamento
select_uri_in = """SELECT 
  data_providers.uri
FROM 
  %s.data_providers, 
  %s.data_series, 
  %s.storages
WHERE 
  data_series.data_provider_id = data_providers.id AND
  storages.data_series_id = data_series.id AND
  storages.id = %d""" % (database_tma,database_tma,database_tma, id_storage)

//Recuperar mascara dos dados que serão apagados/copiados
//Entrada:
// database_tma = Nome do banco
// id_storage = regra de armazenamento
select_mask = """SELECT 
  data_set_formats.value
FROM 
  %s.data_series, 
  %s.data_sets, 
  %s.data_set_formats, 
  %.storages
WHERE 
  data_sets.data_series_id = data_series.id AND
  data_set_formats.data_set_id = data_sets.id AND
  data_set_formats.key = 'mask' AND 
  data_series.id = storages.data_series_id AND
  storages.id = %d;""" % (database_tma, database_tma,database_tma,database_tma,id_storage)

//Recuperar data até onde os dados serão apagados
//No caso de Coletor
// id_colet = "SELECT service_types.id FROM %s.service_types WHERE service_types.name = 'COLLECT'" % (database_tma)
//get_last_process= """
SELECT 
  collector_%d.start_timestamp
FROM 
  %s.collector_%d, 
  %s.collectors, 
  %s.storages
WHERE 
  collector_%d.process_id = collectors.id AND 
  collectors.data_series_output = storages.data_series_id AND 
  storages.id = %d;""" % (id_colet, database_tma, id_colet, database_tma, database_tma,id_colet,id_storage)

//No caso de analise
// id_ana = "SELECT service_types.id FROM %s.service_types WHERE service_types.name = 'ANALYSIS'" % (database_tma)
//get_last_process= """
SELECT 
  analysis_%d.start_timestamp
FROM 
  %s.analysis_%d, 
  %s.analysis, 
  %s.storages
WHERE 
  analysis_%d.process_id = analysis.id AND 
  analysis.data_series_output = storages.data_series_id AND 
  storages.id = %d;""" % (id_ana, database_tma, id_ana, database_tma, database_tma, id_ana, id_storage)