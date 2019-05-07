"use strict";

const fs = require('fs');
const pg = require('pg');
const moment = require('moment');
const Regex = require('xregexp');
const AdmZip = require('adm-zip');

var StatusLog = require("./../core/Enums").StatusLog;
var storageUtils = require("./utils");

/**
 * performs the backup of the data control execution messages of the dynamic data/analysis
 * @param {*} params.storage : storage service is running
 * @param {*} params.schema : schema database
 * @param {*} params.client : database client
 * @param {*} params.logger : logger file
 */
async function backup_Messages(params, service_table, service_type){
  try{
    var sel_process = "SELECT " + service_type + ".id FROM \
    " + params.schema + "." + service_type +", " + params.schema + ".storages WHERE \
    " + service_type + ".data_series_output = \'" + params.storage.data_series_id + "\' AND \
    storages.id = \'" + params.storage.id + "\'";
    console.log(params.storage.name, sel_process);
    var res = await params.client.query(sel_process);
    if (res.rowCount){

      var process_id = res.rows[0].id;
      
    //Copy messages from process to storage_historics
      var sql_insert = "INSERT INTO " + params.schema + ".storage_historics (process_id, status, start_timestamp, data_timestamp, last_process_timestamp, data, \
      origin, type, description, storage_id) SELECT \
      a.process_id, a.status, a.start_timestamp, a.data_timestamp, a.last_process_timestamp, \
      a.data, \'" + service_type + "\', b.type, b.description, \'" + params.storage.id + "\' FROM \
      " + params.schema + "." + service_table + " a, " + params.schema + "." + service_table + "_messages b, \
      " + params.schema + "." + service_type + " c, " + params.schema + ".storages d\
      WHERE b.log_id = a.id AND  \
      a.process_id = \'" + process_id + "\'";
      console.log(params.storage.name, sql_insert);
      var res = await params.client.query(sql_insert);

      if (!res.rowCount){ //Don't have messages, so never have error, brings only data
        var sql_insert1 = "INSERT INTO " + params.schema + ".storage_historics (\
        process_id, status, start_timestamp, data_timestamp, last_process_timestamp, data, \
        origin, storage_id) SELECT \
        a.process_id, a.status, a.start_timestamp, a.data_timestamp, a.last_process_timestamp, \
        a.data, \'" + service_type + "\', \'" + params.storage.id + "\' FROM \
        " + params.schema + "." + service_table + " a WHERE a.process_id = \'" + process_id + "\'";
        console.log(params.storage.name, sql_insert1);
        params.client.query(sql_insert1);

      }
//Delete messages from collector_messages or analysis_messages
      var sql_remove_messages = "DELETE FROM " + params.schema + "." + service_table + "_messages USING \
      " + params.schema + "." + service_table + " WHERE \
      " + service_table + ".id = " + service_table + "_messages.log_id AND \
      " + service_table + ".process_id = \'" + process_id + "\'";
      var res1 = await params.client.query(sql_remove_messages);

//Delete process from collector or analysis
      var sql_remove_process = "DELETE FROM " + params.schema + "." + service_table + " WHERE \
      " + service_table + ".process_id = \'" + process_id + "\'";
      var res2 = params.client.query(sql_remove_process);

      if (res.rowCount){
        params.logger.info(params.storage.name + ": Added " + res.rowCount + " rows in storage_historics");
        params.logger.info(params.storage.name + ": Removed " + res1.rowCount + " rows from " + service_table + "_messages");
        params.logger.info(params.storage.name + ": Removed " + res2.rowCount + " rows from " + service_table);
      }
    }
  }
  catch(err)
  {
    throw err;
  }
};

/**
 * Moves a file from one directory to another
 * @param {*} oldPath : source path
 * @param {*} newPath : destiny path
 */
  function moveFile(oldPath, newPath) {

  fs.rename(oldPath, newPath, function (err) {
      if (err) {
          if (err.code === 'EXDEV') {
              copy();
          } else {
              throw err;
          }
          return;
      }
  });

  function copy() {
      var readStream = fs.createReadStream(oldPath);
      var writeStream = fs.createWriteStream(newPath);

      readStream.on('error', callback);
      writeStream.on('error', callback);

      readStream.on('close', function () {
          fs.unlink(oldPath, callback);
      });

      readStream.pipe(writeStream);
  }
};


async function insertTable(uri, table_name, params){
  return new Promise(async function resolvePromise(resolve, reject){
    try{
      pg.connect(uri, async (err, client, done) =>{
        console.log(params.storage.name,"Conectou "+ uri);

        var date_attr;
        var create_table;

        if (params.seq){
          var create_seq = "CREATE SEQUENCE IF NOT EXISTS " + params.seq.sequence_name + " \
          INCREMENT " + params.seq.increment + " \
          MINVALUE " + params.seq.minimum_value + " \
          MAXVALUE " + params.seq.maximum_value + " \
          START " + params.seq.start_value + " \
          CACHE 1";

          console.log(params.storage.name,create_seq);

          var sequence = await client.query(create_seq);

          create_table = "CREATE TABLE IF NOT EXISTS " + table_name + " ("
                  
          for (let column of params.columns) {
            create_table += column.column_name;
            if (column.data_type == 'USER-DEFINED'){
                create_table += " " + column.udt_name + "(" + column.type + "," + column.srid + ")";
            }
            else{
              create_table +=  " " + column.data_type;
              if (column.data_type == 'character varying')
              create_table += "(" + column.character_maximum_length + ")"
              if (!column.is_nullable)
                create_table += " NOT NULL";
              if (column.column_default)
                create_table += " DEFAULT " + column.column_default;
            }
            create_table += ", ";
          }
          create_table += "CONSTRAINT " + params.key.constraint_name + " \
          PRIMARY KEY (" + params.key.column_name + "))";
        }
        else{
          create_table = "CREATE TABLE IF NOT EXISTS " + table_name + " ("
                  
          for (let column of params.columns) {
            create_table += column.column_name + " " + column.type;
            if (column.notNull)
              table_name += " NOT NULL";
            if (column.default)
              table_name += " DEFAULT " + column.default;
            create_table += ", ";
          }

        }

        console.log(params.storage.name,create_table);

        var rows_added = 0;
        var rows_read = 0;
        client.query(create_table, async (err1, res1) => {
          if (err1){
            throw err1;
          }
          var insert = "INSERT INTO " + table_name + "(" + Object.keys(params.values.rows[0]) + ") VALUES (";
          console.log(params.storage.name,insert);
          for await (let row of params.values.rows){
            var insert_values = insert;
            var row_values = Object.values(row);
            for await (let val of row_values){
              if (Object.prototype.toString.call(val)=== '[object Date]')
                insert_values += "\'" + moment(val).format('YYYY-MM-DD HH:mm:ss') + "\',";
              else if (val !== null){
                if (typeof(val) === 'string')
                  val = val.replace("\'", "\'\'")
                insert_values += "\'" + val + "\',";
              }
              else
                insert_values += "null,";
            }
            var n = insert_values.length;
            insert_values = insert_values.slice(0, n-1); //remove last ','
            insert_values += ") ON CONFLICT DO NOTHING";
            console.log(params.storage.name,insert_values);
            var res2 = await client.query(insert_values);
            rows_added += res2.rowCount;
          }
          params.logger.info(params.storage.name + ": " + rows_added + " rows added in table " + table_name);
          return resolve(true);
        });
      });
   //   return resolve(false);
    }
    catch(err){
      params.looger.error(params.storage.name + ": " + err);
      return reject(err);
    }
  });
};

var getPostgisUriInfo = function(uri) {
  var params = {};
  params.protocol = uri.split(':')[0];
  var hostData = uri.split('@')[1];

  if(hostData) {
    params.hostname = hostData.split(':')[0];
    params.port = hostData.split(':')[1].split('/')[0];
    params.database = hostData.split('/')[1];
  }

  var auth = uri.split('@')[0];

  if(auth) {
    var userData = auth.split('://')[1];

    if(userData) {
      params.user = userData.split(':')[0];
      params.password = userData.split(':')[1];
    }
  }

  return params;
};

async function selectServiceInput (params) 
{
  return new Promise(async function resolvPromise(resolve, reject){
    const is_dynamicData = "SELECT service_instance_id FROM \
    " + params.schema + ".collectors WHERE data_series_output = " + params.storage.data_series_id;
    const is_analysis = "SELECT instance_id FROM \
    " + params.schema + ".analysis, " + params.schema + ".data_sets, " + params.schema + ".data_series WHERE \
    analysis.dataset_output = data_sets.id AND data_series.id = " + params.storage.data_series_id;

    console.log(params.storage.name,is_dynamicData);
    console.log(params.storage.name,is_analysis);
    var service_table ;
    var service_type;

    try{
      //var res= await client.query(select_service);
      var res = await params.client.query(is_dynamicData);

      if (res.rowCount){
        service_type = "collectors"
        service_table = "collector_" + res.rows[0].service_instance_id;
      }
      else{
        var res1 = await params.client.query(is_analysis);
        service_type = "analysis"
        service_table = "analysis_" + res.rows[0].instance_id;
      }
      return resolve({service_table, service_type});
    }
    catch(e){
      params.logger.error(params.storage.name + ": "  + e.watch());
      return reject(e);
    }
  });

};


async function StoreNTable_1(params, res){
  return new Promise(async function resolvePromise(resolve, reject){
    try{
      for (var row of res.rows){
        var table_name_back = params.storage.uri.slice(params.storage.uri.lastIndexOf("/")+1, params.storage.uri.length);
        if (res.rowCount > 1) 
          table_name_back += "_" + row.value;

        var timestamp = "";
        var geometry = "";

        if (params.storage.data_serie_code != "ANALYSIS_MONITORED_OBJECT-postgis"){
          var sel_timestamp = "SELECT data_set_formats.value FROM \
          " + params.schema + ".data_set_formats WHERE \
          data_set_formats.key = \'timestamp_property\' AND \
          data_set_formats.data_set_id = \'" + row.id + "\'";
          console.log(params.storage.name,sel_timestamp);

          var sel_geom = "SELECT data_set_formats.value FROM \
          " + params.schema + ".data_set_formats WHERE \
          data_set_formats.key = \'geometry_property\' AND \
          data_set_formats.data_set_id = \'" + row.id + "\'";
          console.log(params.storage.name,sel_geom);

          //Gets a attribute name of DataTime
          var res1 = await params.client.query(sel_timestamp);
          //Gets a attribute name of GeometryData
          var res2 = await params.client.query(sel_geom);

          if (res1.rowCount)
            timestamp = res1.rows[0].value;
          if (res2.rowCount)
            geometry = res2.rows[0].value;
        }
        else
          timestamp = analysisTimestampPropertyName; //"execution_date"
        
        var paramsout = {
          storage : params.storage,
          schema : params.schema,
          client : params.client,
          logger : params.logger,
          table_name : row.value,
          database_project : row.uri,
          table_name_back : table_name_back,
          timestamp_prop : timestamp,
          geometry_prop: geometry
        };

        var paramsreturn = await StoreTable(paramsout);
        if (paramsreturn){
          params.storage.process.last_process_timestamp = moment().format();
          params.storage.process.data_timestamp = paramsreturn.data_timestamp;
          params.storage.process.description = paramsreturn.description;
          params.storage.process.status = paramsreturn.status;

          updateMessages(params);
        }

        return resolve();
      }
    }
    catch(e){
      return reject(e);
    }
  });

};

async function StoreTable(params)
{
  return new Promise(async function resolvePromise(resolve, reject){
    try{  
      var database_proj_name = params.database_project.slice(params.database_project.lastIndexOf("/")+1, params.database_project.length);
      console.log (database_proj_name);

      var res_data = await getDataUntilStore(params);

      var data_timestamp;
      //if filter, erase all messages to collect/analysis all again, from filter origin date
      if (params.storage.filter)
        backup_Messages(params, res_data.service_table, res_data.service_type);

      params.logger.info(params.storage.name + ": Removing data before " + res_data.data_until_store.format('YYYY-MM-DD HH:mm:ss'));

      pg.connect(params.database_project, async (err6, client_proj, done) =>{
        if(err6) {
          params.logger.error(params.storage.name + ": "  + err6);
          throw err6;
        }

        //Attributes to be included in newtable, key is serial, so it will not be included
        var sel_columns = "SELECT * FROM information_schema.columns WHERE table_name = \'" + params.table_name + "\'";
        var res_columns = await client_proj.query(sel_columns, function (err7){
          if (err7)
            return reject(err7);
        });
        var columns_to_be_back = "";
        for (var column of res_columns.rows){
          if (!column.column_default)
            columns_to_be_back += column.column_name + ", ";
        }
        
        columns_to_be_back = columns_to_be_back.slice(0,columns_to_be_back.lastIndexOf(","));

        var sql_delete = "DELETE FROM " + params.table_name + " WHERE \
        " + params.table_name + "." + params.timestamp_prop + " < \'" + res_data.data_until_store.format('YYYY-MM-DD HH:mm:ss') + "\' \
        RETURNING " + columns_to_be_back;
        console.log(params.storage.name,sql_delete);

        var res_del = await client_proj.query(sql_delete, function (err){
          if (err)
            return reject(err);
        });
 
        if (!res_del.rowCount){
          var obj = {
            flag: false,
            data_timestamp: "",
            description : "No data to process",
            status: StatusLog.WARNING
          }; 

          return resolve(obj);
        }

        var uri;

        var row_values = Object.values(res_del.rows[res_del.rowCount-1]);
        for await (let val of row_values){
          if (Object.prototype.toString.call(val)=== '[object Date]'){
            data_timestamp = val;
            break;
          }
        }

        if (params.storage.backup){
          var sql_data_provider_out = "SELECT data_providers.uri FROM \
          " + params.schema+".data_providers," + params.schema + ".storages WHERE \
          data_providers.id = \'" + params.storage.data_provider_id+"\' AND \
          storages.id = \'" + params.storage.id + "\'";
        
          var data_provider_out = await params.client.query(sql_data_provider_out);
          //console.log(params.storage.name,JSON.stringify(res3.rows[0], null, 4));
          uri = data_provider_out.rows[0].uri;
          console.log(params.storage.name,uri);

          //To verify if table contains geometric information
          var selecttypes = "SELECT \
            g.column_name, \
            g.column_default, \
            g.is_nullable, \
            g.data_type, \
            g.character_maximum_length, \
            g.udt_name, \
            f.type, \
            f.srid FROM \
            information_schema.columns as g JOIN \
            geometry_columns AS f \
            ON (g.table_schema = f.f_table_schema and g.table_name = f.f_table_name ) \
            WHERE table_name = \'" + params.table_name + "\'";
        
          var res3 = await client_proj.query(selecttypes);

          var table;
          var params_newtable = {};
          params_newtable.storage = params.storage;
          params_newtable.values = res_del;
          params_newtable.logger = params.logger;
          var seq_name;

          if (res3.rowCount){
            params_newtable.columns = res3.rows;
          }
          else{
            selecttypes = "SELECT \
            g.column_name, \
            g.column_default, \
            g.is_nullable, \
            g.data_type, \
            g.character_maximum_length, \
            g.udt_name FROM \
            information_schema.columns as g \
            WHERE table_name = \'" + params.table_name + "\'";

            var res_col = await client_proj.query(selecttypes);
            if (res_col.rowCount)
              params_newtable.columns = res_col.rows;
            else
              throw ("Table " + params.table_name + " with problems!"); 
          }

          for (var row in params_newtable.columns){
            if (params_newtable.columns[row].column_default){
              seq_name = params_newtable.columns[row].column_default.slice(params_newtable.columns[row].column_default.indexOf("(")+1, params_newtable.columns[row].column_default.indexOf(":"))
              params_newtable.columns[row].column_name = params_newtable.columns[row].column_name.replace(params.table_name, params.table_name_back);
              var re = new RegExp(params.table_name, 'g'); //to replace all references
              params_newtable.columns[row].column_default = params_newtable.columns[row].column_default.replace(re, params.table_name_back);
            }
          }

          var select_key = "SELECT column_name, constraint_name FROM information_schema.constraint_column_usage WHERE table_name = \'" + params.table_name + "\'";
          var select_seq = "SELECT sequence_name, increment, minimum_value, maximum_value, start_value FROM information_schema.sequences WHERE sequence_name = " + seq_name + "";

          console.log(select_key);
          console.log(select_seq);

          var res4 = await client_proj.query(select_key);

          var res5 = await client_proj.query(select_seq);

          if (res4.rowCount){
            params_newtable.key = res4.rows[0];
            params_newtable.key.column_name = params_newtable.key.column_name.replace(params.table_name, params.table_name_back);
            params_newtable.key.constraint_name = params_newtable.key.constraint_name.replace(params.table_name, params.table_name_back);
          }
      
          if (res5.rowCount){
            params_newtable.seq = res5.rows[0];
            params_newtable.seq.sequence_name = params_newtable.seq.sequence_name.replace(re, params.table_name_back);
          }

          var res_insert_table = await insertTable(uri, params.table_name_back, params_newtable);
        }

        if (data_timestamp)
          params.storage.process.data_timestamp = data_timestamp ;

        params.logger.log('info', params.storage.name + ": " + res_del.rowCount + " rows removed from " + params.table_name);       

        var obj = {
          flag: true,
          deleted_register: res_del.rowCount,
          table: params.table_name,
          data_timestamp: params.storage.process.data_timestamp,
          status: StatusLog.DONE
        }; 

        return resolve(obj);

      });
    }
    catch(err){
      params.logger.error(params.storage.name + ": "  + err);
      return reject(err);
    }
  });
};

async function updateMessages(params){
  console.log (params.storage.name, "updateMessages");
  return new Promise(async function resolvePromise(resolve, reject){
    try{

      var service_table = "storage_" + params.storage.service_instance_id;
      var service_table_message = service_table + "_messages";

      var insertvalues;
      var start = moment(params.storage.process.start_timestamp).format('YYYY-MM-DD HH:mm:ss');
      var last = moment(params.storage.process.last_process_timestamp).format('YYYY-MM-DD HH:mm:ss');
      if (params.storage.process.data_timestamp){
        var data_time = moment(params.storage.process.data_timestamp).format('YYYY-MM-DD HH:mm:ss');
        insertvalues = "INSERT INTO " + params.schema + "." + service_table + 
        " (process_id, status, start_timestamp, data_timestamp, last_process_timestamp, data) VALUES(" + 
        params.storage.id + ", " + params.storage.process.status + ", " + "\'" + start + "\', \'" + data_time +
         "\', \'" + last + "\', \'{\"processing_end_time\":[\"" + moment(params.storage.process.last_process_timestampl).format('YYYY-MM-DDTHH:mm:ssZ') + 
         "\"],\"processing_start_time\":[\"" + moment(params.storage.process.start_process_timestampl).format('YYYY-MM-DDTHH:mm:ssZ') + "\"]}\')";
      }
      else{
        insertvalues = "INSERT INTO " + params.schema + "."  + service_table + 
        " (process_id, status, start_timestamp, last_process_timestamp) VALUES(" + 
        params.storage.id + "," + params.storage.process.status + ", \'" + start + "\', \'" + last + "\') RETURNING *";
      }

      console.log(params.storage.name,insertvalues);
      
      params.client.query(insertvalues, async (err, res) =>{
        if (err){
          console.log(params.storage.name,insertvalues + " ==> " + err);
          throw err;
        }
        if (!params.storage.process.data_timestamp){
          params.storage.process.description = params.storage.process.description ? params.storage.process.description : " ";
          var insert_msg = "INSERT INTO " + params.schema + "." + service_table_message + 
          " (log_id, type, description, timestamp) VALUES(" + res.rows[0].id + ", " + 
          params.storage.process.status + ", \'" + params.storage.process.description + "\', \'" + last + "\')";
            console.log(params.storage.name,insert_msg);
            params.client.query(insert_msg);
        }
        return resolve(true);
      });
    }
    catch(e){
      params.logger.error(params.storage.name + ": "  + e);
      return reject(e);
    }
  });
};

async function getMessages(client, service_table_message, id){
  return new Promise(async function resolvePromise(resolve, reject){
    try{
      var selec_messages = "SELECT * FROM " + service_table_message + " WHERE id = \'"  + id + "\'";
      console.log(selec_messages);

      var res = await client.query(selec_messages);
      var messages = [];
      if (res.rowCount){
        row = res.rows[0];
        var msg = {
          id : row.id,
          log_id : row.log_id,
          type : row.type,
          description : row.description,
          timestamp : row.timestamp
        }
        messages.push(msg);
      }

      return resolve(messages);
    }
    catch(err){
      console.log(err);
      return reject(err);
    }
  });
};


async function getProcessLog(client, selec_log, service_table_message){
  return new Promise(async function resolvePromise(resolve, reject){

    try{
      var logs = [];
      var res = await client.query(selec_log);
      for await (const row of res.rows){
        var log ={
          id : row.id,
          process_id : row.process_id,
          status : row.status,
          start_timestamp : row.start_timestamp,
          data_timestamp : row.data_timestamp,
          last_process_timestamp : row.last_process_timestamp,
          data : row.data
        }
  
        var messages = await getMessages(client, service_table_message, log.id);
        log.messages = messages;
        logs.push(log);
      }
      return resolve(logs);
    }
    catch(err){
      console.log(err);
      return reject(err);
    }
  });
};

async function getDataUntilStore(params){
  return new Promise(async function resolvePromise(resolve, reject){
    try{

      var res_1 =  await selectServiceInput(params);

      var service_table = res_1.service_table;
      var service_type = res_1.service_type; 

      var sel_date = "SELECT MAX(data_timestamp) FROM \
      " + params.schema + "." + service_table + ", \
      " + params.schema + "." + service_type + " WHERE \
      " + service_table + ".process_id = " + service_type + ".id  AND \
      " + service_type + ".data_series_output = " + params.storage.data_series_id;
      var res_data = await params.client.query(sel_date);

      var data_until_store = res_data.rowCount ? new moment(res_data.rows[0].max) : new moment();
      data_until_store = data_until_store._isValid ? data_until_store : new moment();

      // if keep_data equal zero, erases everything
      if (params.storage.keep_data > 0){
        data_until_store.subtract(params.storage.keep_data, params.storage.keep_data_unit);
      }
      else //erases all
        data_until_store = new moment();

      var res = {
        service_table : service_table,
        service_type : service_type,
        data_until_store : data_until_store
      }
      return resolve(res);

    }
    catch(err){
      return reject(err);
    }
  });
}


module.exports = {
/**
 * Createa a message control table of storages in terrama2 database
 * @param {*} client : database client
 * @param {*} schema : schema database
 * @param {*} storage : storage service to run
 */
  createMessageTable: async function (params)
  {
    try{
      var service_table = "storage_" + params.storage.service_instance_id;
      var service_table_message = service_table + "_messages";

      var createtable = "CREATE TABLE IF NOT EXISTS " + params.schema + "." + service_table + " (\
        id serial NOT NULL,\
        process_id integer NOT NULL,\
        status integer NOT NULL,\
        start_timestamp timestamp(1) with time zone,\
        data_timestamp timestamp with time zone,\
        last_process_timestamp timestamp(1) with time zone,\
        data text,\
        CONSTRAINT " + service_table + "_pk PRIMARY KEY (id))";

      var createtable_messages = "CREATE TABLE IF NOT EXISTS "+ params.schema + "." + service_table_message + " ( \
        id serial NOT NULL,\
        log_id integer NOT NULL,\
        type integer NOT NULL,\
        description text,\
        \"timestamp\" timestamp(1) with time zone,\
        CONSTRAINT " + service_table_message +"_pk PRIMARY KEY (id),\
        CONSTRAINT " + service_table_message +"_fk FOREIGN KEY (log_id)\
            REFERENCES " + params.schema + "." + service_table +"(id) MATCH SIMPLE\
            ON UPDATE NO ACTION ON DELETE NO ACTION)";
      
      console.log(params.storage.name,createtable);
      console.log(params.storage.name,createtable_messages);
      var res1 = await params.client.query(createtable);
      var res2 = await params.client.query(createtable_messages);
    }
    catch(e){
      throw e;
    }
  },

  //To backup/erase GRID-geotiff
  StoreTIFF: async function (params)
  {
    return new Promise(async function resolvePromise(resolve, reject){
      const select_sql = "SELECT service_types.name, \
      service_types.id, \
      data_providers.uri, \
      data_set_formats.value FROM \
      " + params.schema + ".service_types, \
      " + params.schema + ".service_instances, \
      " + params.schema + ".data_providers, \
      " + params.schema + ".data_series, \
      " + params.schema +".data_sets, \
      " + params.schema +".data_set_formats, \
      " + params.schema + ".storages \
      WHERE data_sets.data_series_id = data_series.id AND \
      data_set_formats.data_set_id = data_sets.id AND \
      data_set_formats.key = 'mask' AND \
      data_series.data_provider_id = data_providers.id AND \
      service_instances.id =  \'" + params.storage.service_instance_id + "\' AND \
      service_instances.service_type_id = service_types.id AND \
      data_series.id = \'" + params.storage.data_series_id +"\'";

      params.storage.process = {};
      params.storage.process.start_timestamp  = moment();
    // server.emit('startService');
      console.log('Executing ' + params.storage.name);
      params.logger.log('info', "Initializing execution of " + params.storage.name);

      //last valid date of processed data
      var data_timestamp;

      try{
        var res1 = await params.client.query(select_sql);

        //Get date of data of last success tiff generation
        var res_data_tiff = await getDataUntilStore(params);

        //Get date of data of last success storage
        var service_table_storage = "storage_" + params.storage.service_instance_id;
        var sel_date_storage = "SELECT MAX(data_timestamp) FROM " + params.schema + "." + service_table_storage + " WHERE process_id = " + params.storage.id;
        var res_data_storage = await params.client.query(sel_date_storage);

        //if filter, erase all messages to collect/analysis all again, from filter origin date
        if (params.storage.filter)
          backup_Messages(params, res_data_tiff.service_table, res_data_tiff.service_type);

        var dateObj = new Date(res_data_storage.rows[0].max);
        var momentObj = moment(dateObj);

        //test if has data to process, that is, if the data was processed after the last storage 
        if (momentObj.isAfter(res_data_tiff.data_until_store)){
          var obj = {
            flag: false,
            data_out: data_out,
            data_timestamp: "",
            description : "No data to process"
          }; 

          return resolve(obj);
        }

        params.logger.info(params.storage.name + ": Removing data before " + res_data_tiff.data_until_store.format('YYYY-MM-DD HH:mm:ss'));

        console.log("storage " + params.storage.name + " data " + JSON.stringify(res_data_tiff.data_until_store, null, 4));

        var uri = res1.rows[0].uri;
        var mask = res1.rows[0].value;
        var path = uri.slice(uri.indexOf("//") +2, uri.lenght) + "/" + mask.slice(0,mask.indexOf("/"));
        console.log(params.storage.name,"path "+path);
        var regexString = await storageUtils.terramaMask2Regex( mask.slice(mask.indexOf("/")+1, mask.lenght));
        const regex = new Regex(regexString);
        console.log ("no store " + regexString);

        var data_out = params.storage.uri;

        if (params.storage.zip){
          data_out = data_out.slice(params.storage.uri.indexOf("//")+2, params.storage.uri.lenght) + "/" + mask.slice(0,mask.indexOf("%"));
          console.log("ZIPFILE", data_out);
          var zip = new AdmZip();
        }
        else{
          data_out = data_out.slice(params.storage.uri.indexOf("//")+2, params.storage.uri.lenght) + "/" + params.storage.uri + "/" + mask.slice(0,mask.indexOf("/"));
          console.log(params.storage.name,data_out); 
        }

        var moved_files = 0;
        var deleted_files = 0;
        await fs.readdir(path, async function(err4, files){
          if (err4){
            params.logger.error(params.storage.name + ": "  + err4);
            throw err4;
          }
          for (var file of files){
            var match = regex.exec(file);
            //test if filename match with mask
            if (match){
              var year_file = new Number(match[1]);
              var year_proc = res_data_tiff.data_until_store.year();
              var century = year_proc - year_file;
              // If the year is represented by 2 digits it is necessary to know which century. 
              // I considered only the 20th (1900) and 21st (2000) centuries 
              if (century > 1999)
                year_file = year_file +2000;
              else if (century > 1899)
                year_file = year_file + 1900;
              if (match[4]=== null)
                match[4] = 0;
              if (match[5]=== null)
                match[5] = 0;
              
              var filedate = new moment([year_file, Number(match[2])-1, Number(match[3]), Number(match[4]), Number(match[5])]);

              if (!data_timestamp)
                data_timestamp = filedate;
              else
                data_timestamp = filedate.isBefore(data_timestamp) ? filedate : data_timestamp;

              if (filedate.isBefore(res_data_tiff.data_until_store)){
                if (params.storage.backup === true){
                  if (params.storage.zip){
                    zip.addLocalFile(path + "/" + file);
                    fs.unlink(path + "/" + file, function(err6){
                      if (err6){
                        params.logger.error(params.storage.name + ": "  + err6);
                        throw err6;
                      }
                      deleted_files++;
                    });
                  }
                  else{
                    if (!fs.existsSync(data_out)){
                      fs.mkdirSync(data_out, {recursive:true});
                    }

                    moveFile(path + "/" + file, data_out + "/" + file);
                  }
                  moved_files++;
                }
                else{
                  fs.unlink(path + "/" + file, function(err6){
                    if (err6){
                      params.logger.error(params.storage.name + ": "  + err6);
                      throw err6;
                    }
                    deleted_files++;
                  });
                }
              }
            }
          }

          if (params.storage.zip){
            var maskzip = mask.slice(mask.indexOf("%"), mask.indexOf("."));
            maskzip = maskzip.replace(/%/g,'');
            var zipfile = data_out + moment().format(maskzip) + ".zip";
            console.log(params.storage.name,zipfile);
            zip.writeZip(zipfile);
          }
  
          params.storage.process.status = StatusLog.DONE;
          params.storage.process.data_timestamp = data_timestamp;
          params.storage.process.last_process_timestamp = moment();
    
          updateMessages(params);
  
          var obj = {
            flag: true,
            moved_files: moved_files,
            deleted_files: deleted_files,
            path: path,
            data_out: data_out,
            data_timestamp: data_timestamp,
            zipfile : zipfile
          }; 
  
          return resolve(obj);
        });
      }
      catch(err){
        params.logger.error(params.storage.name + ": "  + err);
        params.storage.process.last_process_timestamp = moment();
        params.storage.process.status = StatusLog.ERROR;
        params.storage.process.description = err;
        updateMessages(params);
        return reject(err);
      }
    });
  },

  StoreSingleTable: async function (params)
  {
    return new Promise(async function resolvePromise(resolve, reject){
      //server.emit('startService');

      params.storage.process = {};
      params.storage.process.start_timestamp  = moment();

      var table_name = "dcp_data_" + params.storage.data_series_id;

      const select_sql = "SELECT service_types.name, service_types.id, data_providers.uri FROM \
      " + params.schema + ".service_types, \
      " + params.schema + ".service_instances, \
      " + params.schema + ".storages, \
      " + params.schema + ".data_series, \
      " + params.schema + ".data_providers WHERE \
      data_providers.id = data_series.data_provider_id AND \
      data_series.id = \'" + params.storage.data_series_id + "\' AND \
      service_types.id = service_instances.service_type_id AND \
      service_instances.id =  \'" + params.storage.service_instance_id + "\' AND \
      storages.id = \'" + params.storage.id + "\'";
      console.log(params.storage.name,select_sql);
      console.log(params.storage.name,'Executing ' + params.storage.name);
      params.logger.log('info', "Initializing execution of " + params.storage.name);

      try{
        var res1 = await params.client.query(select_sql);

        //console.log(params.storage.name,JSON.stringify(res1.rows[0], null, 4));

        var params1 = {
          storage : params.storage,
          schema : params.schema,
          client : params.client,
          logger: params.logger,
          table_name : table_name,
          database_project : res1.rows[0].uri,
          table_name_back : params.storage.uri.slice(params.storage.uri.lastIndexOf("/")+1, params.storage.uri.length),
          timestamp_prop : 'datetime'
        };

        var res_storage = await StoreTable(params1);

        params.logger.log('info', "Finalized execution of " + params.storage.name);
        params.storage.process.data_timestamp = res_storage.data_timestamp;
        params.storage.process.last_process_timestamp = moment();
        params.storage.process.status = StatusLog.DONE;
        params.storage.process.description = res_storage.description;
        updateMessages(params);

        return resolve();
     }
      catch(err){
        params.logger.error(params.storage.name + ": "  + err);
        params.storage.process.last_process_timestamp = moment();
        params.storage.process.status = StatusLog.ERROR;
        params.storage.process.description = err;
        updateMessages(params);
        return reject(err);
      }
    });
  },


  StoreNTable: async function (params)
  {
    return new Promise(async function resolvePromise(resolve, reject){
      // server.emit('startService');
      params.storage.process = {};
      params.storage.process.start_timestamp  = moment();

      try{

        params.logger.log('info', "Initializing execution of " + params.storage.name);

        const select_sql = "SELECT data_providers.uri, data_set_formats.value, data_sets.id FROM \
        " + params.schema + ".storages, \
        " + params.schema + ".data_series, \
        " + params.schema+".data_sets, \
        " + params.schema+".data_set_formats, \
        " + params.schema + ".data_providers WHERE \
        data_series.id = data_sets.data_series_id AND \
        data_set_formats.data_set_id = data_sets.id AND \
        data_set_formats.key = 'table_name' AND \
        data_providers.id = data_series.data_provider_id AND \
        data_series.id = \'" + params.storage.data_series_id + "\' AND \
        storages.id = \'" + params.storage.id + "\'";
      
        params.client.query(select_sql, async (err, res) => {
          if (err){
            throw err;
          }

          await StoreNTable_1(params, res);

          return resolve();
    
        });
      }
      catch(err){
        params.logger.error(params.storage.name + ": "  + err);
        params.storage.process.last_process_timestamp = moment();
        params.storage.process.status = StatusLog.ERROR;
        params.storage.process.description = err;
        updateMessages(params);
        return reject(err);
      }
    });
  },

  getLogs: async function(client, schema, service_instance_id, process_id, begin, end){
    return new Promise(async function resolvePromise(resolve, reject){
      if(begin > end)
        swap(begin, end);
  
      var rowNumbers = (end - begin) + 1;

      var service_table = "storage_" + service_instance_id;
      var service_table_message = schema + "." + service_table + "_messages";

      var selec_log = "SELECT * FROM " + schema + "." + service_table + " WHERE process_id = \'" + process_id + "\' \
      ORDER BY id DESC LIMIT \'" + rowNumbers + "\' OFFSET \'" + begin + "\'";
      console.log(selec_log);

      await getProcessLog(client, selec_log, service_table_message)
      .catch(err => {
        console.log(err);
      })
      .then (logs => {
        var obj ={
          instance_id : service_instance_id,
          process_id : process_id,
          log : logs
        }
        var objs=[];
        objs.push(obj);
        return resolve(objs);
      });
    });
  }
}
