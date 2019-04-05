#!/usr/bin/env node

'use strict';

/**
 * It defines which context nodejs should initialize. The context names are available in {@link config/instances}
 * @typeof {string}
 */
var nodeContext = process.argv[2];
console.log('nodeContext ' +nodeContext );

/**
 * It defines which port nodejs should use. When this variables is used, it overrides port number from {@link config/instances}
 */
var nodeContextPort = process.argv[3];
console.log('nodeContextPort ' + nodeContextPort);

/**
 * It defines TerraMA² global configuration (Singleton). Make sure it initializes before everything
 *
 * @type {Application}
 */
var Application = require("./../core/Application");

// setting currentContext
Application.setCurrentContext(nodeContext);

/**
 * Module dependencies.
//  */
const net = require('net');
const Sequelize = require('sequelize');
const pg = require('pg');
const fs = require('fs');
const Regex = require('xregexp');
const moment = require('moment');
 
var PortScanner = require("./../core/PortScanner");
var io = require('../io');
var debug = require('debug')('webapp:server');
var load = require('express-load');
var TcpService = require("./../core/facade/tcp-manager/TcpService");
var TcpManager = require("./../core/TcpManager");
var Utils = require('./../core/Utils');
var Signals = require('./../core/Signals');
var ServiceType = require("./../core/Enums").ServiceType;
var StatusLog = require("./../core/Enums").StatusLog;

var app = require('../app');

var portNumber = '3600'; //default port

// storing active connections
var connections = {};
let settings = Application.getContextConfig();

let contextConfig = settings.db;

var debugMessage = "";
let schema = contextConfig.define.schema;
let databaseName = contextConfig.database;

const analysisTimestampPropertyName = "execution_date";

if (PortScanner.isValidPort(Number(nodeContextPort))) {
  portNumber = nodeContextPort;
} else {
  debugMessage = "No valid port found. The default port " + portNumber + " will be used";
}

/**
 * Get port from environment and store in Express.
 */
let port = normalizePort(process.env.PORT || portNumber);

var format = new Date().toLocaleTimeString();

/**
 * It defines TerraMA² logger library
 * @type {winston.Logger}
 */
//var logger = require("./../core/Logger");
var logger = require("./Logger")(port);

/**
 * Create server.
 */
const server = net.createServer();

if (debugMessage) {
  console.log(debugMessage);
}

/**
 * Creates a new Buffer based on any number of Buffer
 *
 * @private
 * @param {Buffer} arguments Any number of Buffer as arguments, may have undefined itens.
 * @return {Buffer} The new Buffer created out of the list.
 */
function _createBufferFrom() {
  let size = 0;
  for (var i = 0; i < arguments.length; i++) {
    if(arguments[i]) {
      size += arguments[i].length;
    }
  }

  let tmp = new Buffer(size);
  let offset = 0;
  for (var i = 0; i < arguments.length; i++) {
    if(arguments[i]) {
      tmp.set(new Buffer.from(arguments[i]), offset);
      offset+=arguments[i].length;
    }
  }

  return tmp;
}


/**
 This method parses the bytearray received.
 @param {Buffer} byteArray - a nodejs buffer with bytearray received
 @return {Object} object - a javascript object with signal, message and size

 */
function parseByteArray(byteArray) {
  var messageSizeReceived = byteArray.readUInt32BE(0);
  var signalReceived = byteArray.readUInt32BE(4);
  var len = byteArray.length;
  var rawData = byteArray.slice(8, byteArray.length);
 // console.log(rawData.toString());

  // validate signal
  var signal = Utils.getTcpSignal(signalReceived);

  var jsonMessage;

  try{
    if (rawData.length === 0) {
      jsonMessage = {};
    } else {
      jsonMessage = JSON.parse(rawData);
    }
  }
  catch(e){
    console.log(e);
    console.log("rawData:", rawData.toString());
    jsonMessage = {};
  }

  return {
    size: messageSizeReceived,
    signal: signal,
    message: jsonMessage
  };
}

let client_Terrama2_db;
let Storages = [];
let config_db = {};

function storageExists(id) {
  return Storages.some(function(storage) {
    return storage.id === id;
  });
}

function updateStorage(newstorage)
{
  return Storages.some(function(storage) {
    if (storage.id === newstorage.id){
      storage = newstorage;
      return storage;
    } 
  });
}

function addStorage(storage, projects){
  try{
    if (storage.active){
      for (var project of projects){
        if (project.id === storage.project_id){
          if (project.active){
            if (Storages.includes(storage) > 0){
              updateStorage(storage);
            }
            else{
              Storages.push(storage);
            }    
          }
        }
      }
    }
  }
  catch(e){
    logger.error(e);
    throw e;
  }
}

async function databaseConnect(database){
  return new Promise((resolve, reject) => {

    const config = {
      user: database.PG_USER,
      password: database.PG_PASSWORD,
      host: database.PG_HOST,
      port: database.PG_PORT,
      database: database.PG_DB_NAME,
      ssl: true
    };

    var pool = new pg.Pool(config);
    pool.connect((err, client, done) => {
      this.client = client;
      this.done = done;

      if (err)
        reject(err);
      else
        resolve(client);
    });
  });
}

  // try{
  //   const config = {
  //     user: database.PG_USER,
  //     password: database.PG_PASSWORD,
  //     host: database.PG_HOST,
  //     port: database.PG_PORT,
  //     database: database.PG_DB_NAME,
  //     ssl: true
  //   };

  //   logger.info("Initializing database " + database.PG_DB_NAME);
  //   var pool = new pg.Pool(config);
  //   pool.connect().then(client =>{
  //     //client.query("Select * from terrama2.projects where projects.id = '1'").then (res =>{
  //     //  console.log(res.toString());
  //       return Promise.resolve(client);
  //     //});
  //   });
  // }
  // catch (e){
  //   console.log(e);
  //   logger.error(e);
  //   return Promise.reject(e); 
  // }
//}



let beginOfMessage = "(BOM)\0";
let endOfMessage = "(EOM)\0";

/**
 * Listen on provided port, on all network interfaces.
 */
server.listen(port, 'localhost', () =>{
  console.log('Storage is running on port ' + port +'.');
});

server.on('error', onError);
server.on('listening', onListening);


// var teste = {
//   "PG_HOST": "127.0.0.1",
//   "PG_PORT": 5432,
//   "PG_USER": "postgres",
//   "PG_PASSWORD": "postgres",
//   "PG_DB_NAME": "terrama2"
// }

// var client_test = databaseConnect(teste);

server.on('connection', async function(clientSocket) {
  console.log('CONNECTED: ' + clientSocket.remoteAddress + ':' + clientSocket.remotePort);

  let extraData;
  let tempBuffer;
  let parsed;
  let service_instance_id;
  let service_instance_name;
  let serviceLoaded_ = false;
  let sShuttingDown_ = false;

  clientSocket.on('data', async function(byteArray) {

    console.log("RECEIVED: ", byteArray.toString());

     clientSocket.answered = true;
  
    // append and check if the complete message has arrived
    tempBuffer = _createBufferFrom(tempBuffer, byteArray);

    let completeMessage = true;
    // process all messages in the buffer
    // or until we get a incomplete message
    while(tempBuffer && completeMessage) {
      try  {
        let bom = tempBuffer.toString('utf-8', 0, beginOfMessage.length);
        while(tempBuffer.length > beginOfMessage.length && bom !== beginOfMessage) {
          // remove any garbage left in the buffer until a valid message
          // obs: should never happen
          tempBuffer = new Buffer.from(tempBuffer.slice(1));
          bom = tempBuffer.toString('utf-8', 0, beginOfMessage.length);
        }

        if(bom !== beginOfMessage) {
          // no begin of message header:
          //  - clear the buffer
          //  - wait for a new message
          parsed = parseByteArray(byteArray);
          if (parsed.message){
            tempBuffer = undefined;
            completeMessage = false;
          }
          else{
            tempBuffer = undefined;
            throw new Error("Invalid message (BOM)");
          }
        }
        else{
          const messageSizeReceived = tempBuffer.readUInt32BE(beginOfMessage.length);
          const headerSize = beginOfMessage.length + endOfMessage.length;
          const expectedLength = messageSizeReceived + 4;
          if(tempBuffer.length < expectedLength+headerSize) {
            // if we don't have the complete message
            // wait for the rest
            completeMessage = false;
            return;
          }

          const eom = tempBuffer.toString('ascii', expectedLength + beginOfMessage.length, expectedLength+headerSize);
          if(eom !== endOfMessage) {
            // we should have a complete message and and end of message mark
            // if we arrived here we got an ill-formed message
            // clear the buffer and raise an error
            tempBuffer = undefined;
            throw new Error("Invalid message (EOM)");
          }

          // if we got many messages at once
          // hold the buffer we the extra messages for processing
          if(tempBuffer.length > expectedLength+headerSize) {
            extraData = new Buffer.from(tempBuffer.slice(expectedLength + headerSize));
          } else {
            extraData = undefined;
          }

          // get only the first message for processing
          tempBuffer = new Buffer.from(tempBuffer.slice(beginOfMessage.length, expectedLength+beginOfMessage.length));
          parsed = parseByteArray(tempBuffer);
          // get next message in the buffer for processing
          tempBuffer = extraData;
        }
        
        console.log("Size: " + parsed.size + " Signal: " + parsed.signal + " Message: " + JSON.stringify(parsed.message, null, 4));
    
        switch(parsed.signal){
          case Signals.TERMINATE_SERVICE_SIGNAL:
          isShuttingDown_ = true;
          self.emit("stop", parsed);
          break;

        case Signals.STATUS_SIGNAL:
          if (!serviceLoaded_){
            var buffer = beginOfMessage + TcpManager.makebuffer(Signals.STATUS_SIGNAL, {service_loaded:false}) + endOfMessage;
          }
          else{
            var obj={
              service_loaded : true,
              instance_id : service_instance_id,
              instance_name : service_instance_name,
              start_time : moment().format("lll"),
              terrama2_version : "4.1.0", // parsed.message.version, preciso descobtir isto tbm
              shutting_down : false,
              logger_online : false
            }
            var buffer = beginOfMessage + TcpManager.makebuffer(Signals.STATUS_SIGNAL, obj) + endOfMessage;
          }
          console.log("STATUS_SIGNAL", buffer);
          clientSocket.write(buffer);
          break;

        case Signals.ADD_DATA_SIGNAL:
          try{
            var storages = parsed.message.Storages;
            for (let storage of storages){
              addStorage(storage, parsed.message.Projects);
            }
          }
          catch(e){
            console.log(e);
            var buffer = beginOfMessage + TcpManager.makebuffer(Signals.UPDATE_SERVICE_SIGNAL, parsed.message) + endOfMessage;
            console.log("ADD_ERROR", buffer);
            clientSocket.write(buffer);
          }
          var buffer = beginOfMessage + TcpManager.makebuffer(Signals.ADD_DATA_SIGNAL, parsed.message) + endOfMessage;
          clientSocket.write(buffer);
          break;
        case Signals.START_PROCESS_SIGNAL:
        case Signals.LOG_SIGNAL:
        case Signals.REMOVE_DATA_SIGNAL:
        case Signals.PROCESS_FINISHED_SIGNAL:
          break;
    
        case Signals.UPDATE_SERVICE_SIGNAL:
        //Need to discover how many theads are supported
          if (parsed.message.instance_id){
            serviceLoaded_ = true;
            service_instance_id = parsed.message.instance_id;
            service_instance_name = parsed.message.instance_name;

            // if (!client_Terrama2_db){
            //   const config = {
            //     user: parsed.message.log_database.PG_USER,
            //     password: parsed.message.log_database.PG_PASSWORD,
            //     host: parsed.message.log_database.PG_HOST,
            //     port: parsed.message.log_database.PG_PORT,
            //     database: parsed.message.log_database.PG_DB_NAME,
            //     ssl: true
            //   };
            


            //   logger.info("Initializing database " + config.database);
            //   var pool = new pg.Pool(config);
            //   pool.connect().then(client =>{
              client_Terrama2_db = await databaseConnect(parsed.message.log_database)
              console.log("Conectou ", client_Terrama2_db.toString());
              
              var buffer = beginOfMessage + TcpManager.makebuffer(Signals.UPDATE_SERVICE_SIGNAL, parsed.message) + endOfMessage;
              console.log("UPDATE_SERVICE_SIGNAL", buffer);
              clientSocket.write(buffer);
              //});
           // }
          }
          break;
  
        case Signals.VALIDATE_PROCESS_SIGNAL:
        
          var buffer = beginOfMessage + TcpManager.makebuffer(Signals.ADD_DATA_SIGNAL, obj) + endOfMessage;
          console.log("VALIDATE_PROCESS_SIGNAL", buffer);
          clientSocket.write(buffer);
          break;
        }
      }
      catch(e)
      {
        console.log(e);
        throw (e);
      }
    }
  });
});

// detecting sigint error and then close server
process.on('SIGINT', async () => {
  TcpService.finalize();
  TcpService.disconnect();

   server.close(() => {
     console.log('Storage finalized');

     process.exit(0);
   });

  for (var key in connections)
    connections[key].destroy();
});

process.on('message', async (msg) =>{
  console.log(msg);
})

/**
 * Normalize a port into a number, string, or false.
 */
function normalizePort(val) {
  var port = parseInt(val, 10);

  if (isNaN(port)) {
    // named pipe
    return val;
  }

  if (port >= 0) {
    // port number
    return port;
  }

  return false;
}

/**
 * Event listener for HTTP server "error" event.
 */
function onError(error) {
  if (error.syscall !== 'listen') {
      console.log(error);
  }

  var bind = typeof port === 'string' ? 'Pipe ' + port : 'Port ' + port;

  // handle specific listen errors with friendly messages
  switch (error.code) {
    case 'EACCES':
      console.log(bind + ' requires elevated privileges');
      process.exit(1);
      break;
    case 'EADDRINUSE':
      console.log(bind + ' is already in use');
      process.exit(1);
      break;
    default:
      console.log(error);
  }
}

function handle(signal){
  console.log('Received ${signal}');
}

/**
 * Event listener for HTTP server "listening" event.
 */
function onListening() {
  var addr = server.address();
  var bind = typeof addr === 'string' ? 'pipe ' + addr : 'port ' + addr.port;

  console.log('Listening on ' + bind);
}

io.attach(server);
load('sockets').into(io);


// /**
//  * This method sends a ADD_DATA_SIGNAL with bytearray to tcp socket. It is async
//  *
//  * @param {ServiceInstance} serviceInstance - a terrama2 service instance
//  * @param {Object} data - a javascript object message to send
//  */
// TcpManager.prototype.sendData = function(serviceInstance, data) {
//   console.log("sendData", data.toString());
//   this.$send(serviceInstance, data, Signals.ADD_DATA_SIGNAL);
// };


// var schema = contextConfig.define.schema;
// var databaseName = contextConfig.database;

// var results = [];

// //Connect to Terrama2 database
// try{

//   const config = {
//     user: contextConfig.username,
//     password: contextConfig.password,
//     host: contextConfig.host,
//     port: contextConfig.port,
//     database: contextConfig.database
//   };

//   logger.info("Initializing database " + databaseName);
  
//   var pool = new pg.Pool(config);
//   pool.connect().then (client =>{
//     server.emit('message', "Conectou ao TerraMA2");

//     var storage = client.query("Select id, name from " + schema + ".service_types ")
//     var sql_logtable = "CREATE "
//     //Load rules of storage
//     client.query('SELECT * FROM ' + schema +'.storages where storages.active = true', async (err, res) => {
//     // Stream results back one row at a time
//       const nrows = res.rows.length;
//       for (var i = 0; i < nrows; i++){
//         var storage = res.rows[i];
//         console.log(storage);
//         logger.debug(storage);

//         var store_id = storage.id;
//         var data_serie_code;
//        // console.log(store_id);

//         //Verifies which data_serie type will be stored
//         var select_data_type= "SELECT data_series_semantics.code FROM \
//         "+schema+".storages, \
//         "+schema+".data_series, \
//         "+schema+".data_series_semantics WHERE \
//         data_series_semantics.id = data_series.data_series_semantics_id AND \
//         data_series.id = \'" + storage.data_series_id + "\'";
//         console.log(select_data_type);
//         var res1 = await client.query(select_data_type);

//         data_serie_code = res1.rows[0].code;
//         storage.data_serie_code = data_serie_code;
//         switch (data_serie_code){
//           case "GRID-geotiff":
//            // StoreTIFF(storage, schema, client);
//           break;
//           case "DCP-single_table":
//            // StoreSingleTable(storage, schema, client);
//           break;
//           case "DCP-postgis":
//           case "ANALYSIS_MONITORED_OBJECT-postgis":
//           case "OCCURRENCE-postgis":
//            // StoreNTable(storage, schema, client);
//           break;
//           default:
//           console.log("erro -");
//         }
//           //return res.rows[0].code;
//       };
//     });
//   });
//  // return process.exit(1);
// }
// catch(err)
// {
// // Handle connection errors
//  // done();
//   console.log(err);
//   logger.error(err);
//  // return res.status(500).json({success: false, data: err});
// }


async function selectService (storage, schema, client) 
{
  return new Promise(async function resolvPromise(resolve, reject){
    const select_service = "SELECT service_types.name, service_instances.id FROM \
    " + schema + ".service_types, \
    " + schema + ".service_instances \
    WHERE service_instances.id =  \'" + storage.service_instance_id + "\' AND \
    service_types.id = service_instances.service_type_id";

    console.log(select_service);
    var service_table ;
    var service_type;

    try{
      var res= await client.query(select_service);

      if (res.rows[0].name.toString() === "COLLECT"){
        service_type = "collectors"
        service_table = "collector_" + res.rows[0].id;
      }
      else{
        service_type = "analysis"
        service_table = "analysis_" + res.rows[0].id;
      }
      return resolve({service_table, service_type});
    }
    catch(e){
      logger.error(storage.name + ": "  + e.watch());
      return reject(e);
    }
  });

}

//To backup/erase GRID-geotiff
async function StoreTIFF(storage, schema, client)
{
  const select_sql = "SELECT service_types.name, \
  service_types.id, \
  data_providers.uri, \
  data_set_formats.value FROM \
  " + schema + ".service_types, \
  " + schema + ".service_instance, \
  " + schema + ".data_providers, \
  " + schema + ".data_series, \
  " + schema +".data_sets, \
  " + schema +".data_set_formats, \
  " + schema + ".storages \
  WHERE data_sets.data_series_id = data_series.id AND \
  data_set_formats.data_set_id = data_sets.id AND \
  data_set_formats.key = 'mask' AND \
  data_series.data_provider_id = data_providers.id AND \
  service_instance.id =  \'" + storage.service_instance_id + "\' AND \
  service_instance.service_type_id = service_type.id AND \
  data_series.id = \'" + storage.data_series_id +"\'";

  storage.process = {};
  storage.process.start_timestamp  = moment();
  server.emit('startService');
  console.log('Executing ' + storage.name);
  logger.log('info', "Initializing execution of " + storage.name);

  var data_timestamp;

  try{
    var res1 = await client.query(select_sql);

   // console.log(JSON.stringify(res1.rows[0], null, 4));

    var service_table ;
    var service_type;
    var data_until_store = new moment();

    // if keep_data equal zero, erases everything
    if (storage.keep_data > 0){
      data_until_store.subtract(storage.keep_data, storage.keep_data_unit);
    }
    else
      backup_Messages(storage, schema, client);

    logger.info(storage.name + ": Removing data before " + data_until_store.format('YYYY-MM-DD HH:mm:ss'));

    console.log("storage " + storage.name + " data " + JSON.stringify(data_until_store, null, 4));

    var uri = res1.rows[0].uri;
    var mask = res1.rows[0].value;
    var path = uri.slice(uri.indexOf("//") +2, uri.lenght) + "/" + mask.slice(0,mask.indexOf("/"));
   // console.log("path "+path);
    var regexString = terramaMask2Regex( mask.slice(mask.indexOf("/")+1, mask.lenght));
    const regex = new Regex(regexString);
    //console.log ("no store " + regexString);

    var data_provider_out = "Select data_providers.uri from "+schema+".data_providers,"+schema+".storages WHERE \
    data_providers.id = \'"+storage.data_provider_id+"\'";

    var res3 = await client.query(data_provider_out);
    var data_out = res3.rows[0].uri;

    data_out = data_out.slice(data_out.indexOf("//")+2, data_out.lenght) + "/" + storage.uri + "/" + mask.slice(0,mask.indexOf("/"));
    //console.log(data_out);
    await fs.readdir(path, async function(err4, files){
      if (err4){
        logger.error(storage.name + ": "  + err4);
        throw err4;
      }
      var moved_files = 0;
      var deleted_files = 0;
      //files.forEach(async function(file, index){
      for(var i=0; i<files.length; i++){
        var file = files[i];
        var match = regex.exec(file);
        if (match){
         // console.log(JSON.stringify(match, null, 4));
          var year_file = new Number(match[1]);
          var year_proc = data_until_store.year();
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

         // console.log(JSON.stringify(filedate, null, 4));
         // console.log(JSON.stringify(data_until_store, null, 4));
          //Falta testar a data
          if (filedate.isBefore(data_until_store)){
            console.log(file);
            if (storage.backup === true){
              if (!fs.existsSync(data_out)){
                fs.mkdirSync(data_out, {recursive:true});
              }
              //moveFile(path + "/" + file, data_out + "/" + file);
              moved_files++;
            }
            else{
              fs.unlink(path + "/" + file, function(err6){
                if (err6){
                  logger.error(storage.name + ": "  + err6);
                  throw err6;
                }
                deleted_files++;
              });
            }
          }
        }
      }

      await logger.log('info', storage.name + ": moved files: " + moved_files + " from " + path + " to " + data_out);
      await logger.log('info', storage.name + ": removed files: " + deleted_files + " from " + path);
      logger.log('info', "Finalized execution of " + storage.name);
      storage.process.data_timestamp = data_timestamp;
      storage.process.last_process_timestamp = moment();
      storage.process.status = StatusLog.DONE;
      updateMessages(storage, schema, client);

    });
  }
  catch(err){
    logger.error(storage.name + ": "  + err);
    storage.process.last_process_timestamp = moment();
    storage.process.status = StatusLog.ERROR;
    storage.procees.description = err;
    updateMessages(storage, schema, client);
    throw err;
  }

  return;
}

async function StoreTable(params)
{
  try{  
    var database_proj_name = params.database_project.slice(params.database_project.lastIndexOf("/")+1, params.database_project.length);
    console.log (database_proj_name);

    var data_until_store = new moment();
    var data_timestamp;

    // if keep_data equal zero, erases everything
    if (params.storage.keep_data > 0){
      data_until_store.subtract(params.storage.keep_data, params.storage.keep_data_unit);
    }
    else
      backup_Messages(params.storage, params.schema, params.client);

    logger.info(params.storage.name + ": Removing data before " + data_until_store.format('YYYY-MM-DD HH:mm:ss'));

    pg.connect(params.database_project, async (err6, client_proj, done) =>{
      if(err6) {
        logger.error(params.storage.name + ": "  + err6);
        throw err6;
      }

      //  var sql_delete = "DELETE FROM " + params.table_name + " WHERE \
      //  " + params.table_name + "." + params.timestamp_prop + " < \'" + data_until_store.format('YYYY-MM-DD HH:mm:ss') + "\' \
      // ORDER BY " + params.timestamp_prop + " ASC RETURNING *";
      //  console.log(sql_delete);
      //teste ->retornar o código anterior depoius de testar
      var sql_delete = "SELECT * FROM " + params.table_name + " WHERE \
      " + params.table_name + "." + params.timestamp_prop + " < \'" + data_until_store.format('YYYY-MM-DD HH:mm:ss') + "\' \
      ORDER BY " + params.timestamp_prop + " ASC";
      console.log(sql_delete);

      var res_del = await client_proj.query(sql_delete);

      if (res_del.rowCount){
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
          //console.log(JSON.stringify(res3.rows[0], null, 4));
          uri = data_provider_out.rows[0].uri;
          console.log(uri);

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
          //console.log(JSON.stringify(res3.rows, null, 4));
          var table;
          var params_newtable = {};
          params_newtable.storage = params.storage;
          params_newtable.values = res_del;
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
           // console.log(JSON.stringify(res_col.rows, null, 4));
            if (res_col.rowCount)
              params_newtable.columns = res_col.rows;
            else
              throw ("Table " + params.table_name + " with problems!"); 
          }

          for (var row in params_newtable.columns){
            if (params_newtable.columns[row].column_default)
              seq_name = params_newtable.columns[row].column_default.slice(params_newtable.columns[row].column_default.indexOf("(")+1, params_newtable.columns[row].column_default.indexOf(":"))
          }

          var select_key = "SELECT column_name, \
            constraint_name FROM information_schema.constraint_column_usage WHERE \
            table_name = \'" + params.table_name + "\'";

          var select_seq = "SELECT sequence_name, increment, minimum_value, maximum_value, start_value \
            FROM information_schema.sequences WHERE sequence_name = " + seq_name + "";

          var res4 = await client_proj.query(select_key);

          var res5 = await client_proj.query(select_seq);

          if (res4.rowCount)
            params_newtable.key = res4.rows[0];
      
          if (res5.rowCount)
            params_newtable.seq = res5.rows[0];

          await insertTable(uri, params.table_name_back, params_newtable);

        }

        if (data_timestamp){
          params.storage.process.data_timestamp = params.storage.process.data_timestamp ? 
          data_timestamp.isAfter(params.storage.process.data_timestamp) ?
          data_timestamp : params.storage.process.data_timestamp : params.storage.process.data_timestamp;
        }
        logger.log('info', params.storage.name + ": " + res_del.rowCount + " rows removed from " + params.table_name);       
      }  
    });
  }
  catch(err){
    logger.error(params.storage.name + ": "  + err);
    throw err;
  }
}

async function StoreSingleTable(storage, schema, client)
{
  server.emit('startService');

  storage.process = {};
  storage.process.start_timestamp  = moment();

  var table_name = "dcp_data_" + storage.data_series_id;

  const select_sql = "SELECT service_types.name, service_types.id, data_providers.uri FROM \
  " + schema + ".service_types, \
  " + schema + ".service_instances, \
  " + schema + ".storages, \
  " + schema + ".data_series, \
  " + schema + ".data_providers WHERE \
  data_providers.id = data_series.data_provider_id AND \
  data_series.id = \'" + storage.data_series_id + "\' AND \
  service_types.id = service_instances.service_type_id AND \
  service_instances.id =  \'" + storage.service_instance_id + "\' AND \
  storages.id = \'" + storage.id + "\'";
  console.log(select_sql);
  console.log('Executing ' + storage.name);
  logger.log('info', "Initializing execution of " + storage.name);

   try{
    var res1 = await client.query(select_sql);

    //console.log(JSON.stringify(res1.rows[0], null, 4));

    var params = {
      storage : storage,
      schema : schema,
      client : client,
      table_name : table_name,
      database_project : res1.rows[0].uri,
      table_name_back : storage.uri,
      timestamp_prop : 'datetime'
    };

    StoreTable(params, async(err)=>{
      if (err){
        throw err;
      }
 
       logger.log('info', "Finalized execution of " + storage.name);
       storage.process.data_timestamp = data_timestamp;
       storage.process.last_process_timestamp = moment();
       storage.process.status = StatusLog.DONE;
       updateMessages(storage, schema, client);
    });
   }
   catch(err){
     logger.error(storage.name + ": "  + err);
     storage.process.last_process_timestamp = moment();
     storage.process.status = StatusLog.ERROR;
     storage.procees.description = err;
     updateMessages(storage, schema, client);
     throw (err);
   }
}

async function StoreNTable_1(storage, schema, client, res){
  for (var row of res.rows){

  var table_name_back = storage.uri;
  if (res.rowCount > 1) 
    table_name_back += "_" + row.value;

  var timestamp = "";
  var geometry = "";

  if (storage.data_serie_code != "ANALYSIS_MONITORED_OBJECT-postgis"){
    var sel_timestamp = "SELECT data_set_formats.value FROM \
    " + schema + ".data_set_formats WHERE \
    data_set_formats.key = \'timestamp_property\' AND \
    data_set_formats.data_set_id = \'" + row.id + "\'";
    console.log(sel_timestamp);

    var sel_geom = "SELECT data_set_formats.value FROM \
    " + schema + ".data_set_formats WHERE \
    data_set_formats.key = \'geometry_property\' AND \
    data_set_formats.data_set_id = \'" + row.id + "\'";
    console.log(sel_geom);

    var res1 = await client.query(sel_timestamp);
    var res2 = await client.query(sel_geom);

    if (res1.rowCount)
      timestamp = res1.rows[0].value;
    if (res2.rowCount)
      geometry = res2.rows[0].value;
  }
  else
    timestamp = analysisTimestampPropertyName; //"execution_date"
  
  var params = {
    storage : storage,
    schema : schema,
    client : client,
    table_name : row.value,
    database_project : row.uri,
    table_name_back : table_name_back,
    timestamp_prop : timestamp,
    geometry_prop: geometry
  };

  await StoreTable(params);
}

}


async function StoreNTable(storage, schema, client)
{
  server.emit('startService');
  storage.process = {};
  storage.process.start_timestamp  = moment();

  try{

    logger.log('info', "Initializing execution of " + storage.name);

    const select_sql = "SELECT data_providers.uri, data_set_formats.value, data_sets.id FROM \
    " + schema + ".storages, \
    " + schema + ".data_series, \
    " +schema+".data_sets, \
    " +schema+".data_set_formats, \
    " + schema + ".data_providers WHERE \
    data_series.id = data_sets.data_series_id AND \
    data_set_formats.data_set_id = data_sets.id AND \
    data_set_formats.key = 'table_name' AND \
    data_providers.id = data_series.data_provider_id AND \
    data_series.id = \'" + storage.data_series_id + "\' AND \
    storages.id = \'" + storage.id + "\'";
  
    client.query(select_sql, async (err, res) => {
      if (err){
        throw err;
      }

      //res.rows.forEach(async function(row, index){
     // for (var row of res.rows){

        await StoreNTable_1(storage, schema, client, res);
 
     // };

      logger.log('info', "Finalized execution of " + storage.name);
      storage.process.last_process_timestamp = moment();
      storage.process.status = StatusLog.DONE;
      updateMessages(storage, schema, client);

    
    });
 
 }
  catch(err){
    logger.error(storage.name + ": "  + err);
    storage.process.last_process_timestamp = moment();
    storage.process.status = StatusLog.ERROR;
    storage.procees.description = err;
    updateMessages(storage, schema, client);
    throw (err);
  }


}

function terramaMask2Regex(mask)
{
  var regex = new Regex('\\%\\(\\)\\%');
  var list = mask.split(regex);
  var listout = '';
 
  var m;
  for(var i=0; i< list.length;i++){
    m = list[i];
    if(mask.startsWith("%(")){
      if (i%2==0){
        m = "("+m+")";
        continue;
      }    
    }
    else
    if (i%2!=0){
      m = "("+m+")";
      continue;
    }    

    // escape regex metacharacters
    m = m.replace("+", "\\+");
    m = m.replace("(", "\\(");
    m = m.replace(")", "\\)");
    m = m.replace("[", "\\[");
    m = m.replace("]", "\\]");
    m = m.replace("{", "\\{");
    m = m.replace("}", "\\}");
    m = m.replace("^", "\\^");
    m = m.replace("$", "\\$");
    m = m.replace("&", "\\&");
    m = m.replace("|", "\\|");
    m = m.replace("?", "\\?");
    m = m.replace(".", "\\.");

    /*
     *
      YYYY  year with 4 digits        [0-9]{4}
      YY    year with 2 digits        [0-9]{2}
      MM    month with 2 digits       0[1-9]|1[012]
      DD    day with 2 digits         0[1-9]|[12][0-9]|3[01]
      hh    hout with 2 digits        [0-1][0-9]|2[0-4]
      mm    minutes with 2 digits     [0-5][0-9]
      ss    seconds with 2 digits     [0-5][0-9]
      *    any character, any times  .*
      */

    m = m.replace(/%YYYY/gi, "(?<YEAR>[0-9]{4})");
    m = m.replace(/%YY/gi, "(?<YEAR2DIGITS>[0-9]{2})");
    m = m.replace("%MM", "(?<MONTH>0[1-9]|1[012])");
    m = m.replace("%DD", "(?<DAY>0[1-9]|[12][0-9]|3[01])");
    m = m.replace("%JJJ", "(?<JULIAN_DAY>\\d{3})");
    m = m.replace("%hh", "(?<HOUR>[0-1][0-9]|2[0-4])");
    m = m.replace("%mm", "(?<MINUTES>[0-5][0-9])");
    m = m.replace("%ss", "(?<SECONDS>[0-5][0-9])");
    m = m.replace("*", ".*");
    // add a extension validation in case of the name has it
    m = m + "(?<EXTENSIONS>(\\.(gz|zip|rar|7z|tar))+)?$";
    listout = listout + m;
  }

  //console.log("no terrama2 " + listout);
  return listout;
}

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
}

async function backup_Messages(storage, schema, client){
  try{
    var res_1 =  await selectService(storage, schema, client);
    //logger.debug(JSON.stringify(res_1, null, 4));

    var service_table = res_1.service_table;
    var service_type = res_1.service_type;  

    var sel_process = "SELECT " + service_type + ".id FROM \
    " + schema + "." + service_type +", " + schema + ".storages WHERE \
    " + service_type + ".data_series_output = \'" + storage.data_series_id + "\' AND \
    storages.id = \'" + storage.id + "\'";
    console.log(sel_process);
    var res = await client.query(sel_process);
    if (res.rowCount){

      var process_id = res.rows[0].id;
//Copy messages tfrom process to storage_historics
      var sql_insert = "INSERT INTO " + schema + ".storage_historics (\
        process_id, status, start_timestamp, data_timestamp, last_process_timestamp, data, \
        origin, type, description, storage_id) SELECT \
        a.process_id, a.status, a.start_timestamp, a.data_timestamp, a.last_process_timestamp, \
        a.data, \'" + service_type + "\', b.type, b.description, \'" + storage.id + "\' FROM \
        " + schema + "." + service_table + " a, " + schema + "." + service_table + "_messages b, \
        " + schema + "." + service_type + " c, " + schema + ".storages d\
        WHERE b.log_id = a.id AND  \
        a.process_id = \'" + process_id + "\'";
        console.log(sql_insert);
//Delete messages from collector_messages or analysis_messages
        var sql_remove_messages = "DELETE FROM " + schema + "." + service_table + "_messages USING \
        " + schema + "." + service_table + " WHERE \
        " + service_table + ".id = " + service_table + "_messages.log_id AND \
        " + service_table + ".process_id = \'" + process_id + "\'";

//Delete process frrom collector or analysis
        var sql_remove_process = "DELETE FROM " + schema + "." + service_table + " WHERE \
        " + service_table + ".process_id = \'" + process_id + "\'";

        var res = await client.query(sql_insert);
        var res1 = await client.query(sql_remove_messages);
        var res2 = client.query(sql_remove_process);

        if (res.rowCount){
          logger.info(storage.name + ": Added " + res.rowCount + " rows in storage_historics");
          logger.info(storage.name + ": Removed " + res1.rowCount + " rows from " + service_table + "_messages");
          logger.info(storage.name + ": Removed " + res2.rowCount + " rows from " + service_table);
        }
      }
  }
  catch(err)
  {
    throw err;
  }
}

async function insertTable(uri, table_name, params){
  try{
    pg.connect(uri, async (err, client, done) =>{
      console.log ("Conectou "+ uri);

      var date_attr;
      var create_table;

      if (params.seq){
        var create_seq = "CREATE SEQUENCE IF NOT EXISTS " + params.seq.sequence_name + " \
        INCREMENT " + params.seq.increment + " \
        MINVALUE " + params.seq.minimum_value + " \
        MAXVALUE " + params.seq.maximum_value + " \
        START " + params.seq.start_value + " \
        CACHE 1";

        console.log(create_seq);

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

      console.log(create_table);

      var rows_added = 0;
      var rows_read = 0;
      client.query(create_table, async (err1, res1) => {
        if (err1){
          throw err1;
        }
        var insert = "INSERT INTO " + table_name + "(" + Object.keys(params.values.rows[0]) + ") VALUES (";
        console.log(insert);
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
          console.log(insert_values);
          var res2 = await client.query(insert_values);
          rows_added += res2.rowCount;
        }
        logger.info(params.storage.name + ": " + rows_added + " rows added in table " + table_name);
      });
    });
  }
  catch(err){
    looger.error(params.storage.name + ": " + err);
    throw err;
  }
}

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

async function updateMessages(storage, schema, client, start, data, last, msg){
  const select_service = "SELECT service_types.name, service_instances.id FROM \
  " + schema + ".service_types, \
  " + schema + ".service_instances \
  WHERE service_types.id =  \'" + ServiceType.STORAGE + "\' AND\
  service_types.id = service_instances.service_type_id";

  console.log(select_service);
  try{
    var res = await client.query(select_service);
    var service_table = "storage_" + res.rows[0].id;;
    var service_table_message = service_table + "_messages";

    var createtable = "CREATE TABLE IF NOT EXISTS " + schema + "." + service_table + " (\
      id serial NOT NULL,\
      process_id integer NOT NULL,\
      status integer NOT NULL,\
      start_timestamp timestamp(1) with time zone,\
      data_timestamp timestamp with time zone,\
      last_process_timestamp timestamp(1) with time zone,\
      data text,\
      CONSTRAINT " + service_table + "_pk PRIMARY KEY (id))";

    var createtable_messages = "CREATE TABLE IF NOT EXISTS "+ schema + "." + service_table_message + " ( \
      id serial NOT NULL,\
      log_id integer NOT NULL,\
      type integer NOT NULL,\
      description text,\
      \"timestamp\" timestamp(1) with time zone,\
      CONSTRAINT " + service_table_message +"_pk PRIMARY KEY (id),\
      CONSTRAINT " + service_table_message +"_fk FOREIGN KEY (log_id)\
          REFERENCES " + schema + "." + service_table +"(id) MATCH SIMPLE\
          ON UPDATE NO ACTION ON DELETE NO ACTION)";
        
    var res1 = await client.query(createtable);
    var res1 = await client.query(createtable_messages);

    var insertvalues;
    var start = moment(storage.process.start_timestamp).format('YYYY-MM-DD HH:mm:ss');
    var last = moment(storage.process.last_process_timestampl).format('YYYY-MM-DD HH:mm:ss');
    if (storage.process.data_timestamp){
      var data_time = moment(storage.process.data_timestamp).format('YYYY-MM-DD HH:mm:ss');
      insertvalues = "INSERT INTO " + schema + "." + service_table + " (\
      process_id, status, start_timestamp, data_timestamp, last_process_timestamp, data) \
      VALUES(" + storage.id + ", " + storage.process.status + ", \
      " + "\'" + start + "\', \'" + data_time + "\', \'" + last + "\', \
      \'{\"processing_end_time\":[\"" + moment(storage.process.last_process_timestampl).format('YYYY-MM-DDTHH:mm:ssZ') + "\"],\
\"processing_start_time\":[\"" + moment(storage.process.start_process_timestampl).format('YYYY-MM-DDTHH:mm:ssZ') + "\"]}\')";
    }
    else{
      insertvalues = "INSERT INTO " + schema + "."  + service_table + " (\
      process_id, status, start_timestamp, last_process_timestamp) \
      VALUES(" + storage.id + "," + storage.process.status + ", \'\
      " + start + "\', \'" + last + "\')";
    }

    console.log(insertvalues);
    
    client.query(insertvalues, async (err, res) =>{
      if (err){
        console.log(insertvalues + " ==> " + err);
        throw err;
      }
      if (!storage.process.data_timestamp){
        storage.process.description = storage.process.description ? storage.process.description : " ";
        var insert_msg = "INSERT INTO " + schema + "." + service_table_message + " \
          (log_id, type, description, timestamp) VALUES(\
          " + storage.id + ", " + storage.process.status + ", \'\
          " + storage.process.description + "\', \'" + last + "\')";
          console.log(insert_msg);
          client.query(insert_msg);
        }
    });
  }
  catch(e){
    logger.error(storage.name + ": "  + e.watch());
    return reject(e);
  }
}
