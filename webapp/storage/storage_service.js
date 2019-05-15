#!/usr/bin/env node

'use strict';

/**
 * It defines which context nodejs should initialize. The context names are available in {@link config/instances}
 * @typeof {string}
 */

//console.log("No storage_Service.js", process.argv);

var nodeContext = "";//process.argv[1];
//console.log('nodeContext ' +nodeContext );

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
const Regex = require('xregexp');
const moment = require('moment');
const CronJob = require('cron').CronJob;

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
var ScheduleType = require("./../core/Enums").ScheduleType;
var storageUtils = require('./utils');
var sto_core = require('./storage_core');

var portNumber = '3600'; //TerraMA2 default port
var nodeContextPort = '5000'; //storage default port

let version = Application.get().metadata.version;

for (var v of process.argv){
  if (v === '--version'){
    console.log(version);
    process.exit(0);
  }
  if (!isNaN(v))
    nodeContextPort = v;
}

/**
 * It defines which port nodejs should use. When this variables is used, it overrides port number from {@link config/instances}
 */
//var nodeContextPort = process.argv[3];
console.log('nodeContextPort ' + nodeContextPort);

var app = require('../app');

// storing active connections
var connections = {};
let settings = Application.getContextConfig();

let contextConfig = settings.db;

console.log("contextConfig",contextConfig);

var debugMessage = "";
let schema = contextConfig.define.schema; // getting terrama2 schema
let databaseName = contextConfig.database; // getting terrama2 database name

if (PortScanner.isValidPort(Number(nodeContextPort))) {
  portNumber = nodeContextPort;
} else {
  debugMessage = "No valid port found. The default port " + portNumber + " will be used";
}

/**
 * Get port from environment and store in Express.
 */
let port = storageUtils.normalizePort(process.env.PORT || portNumber);

/**
 * It defines Storage logger library
 * @type {winston.Logger}
 */
var logger = require("./Logger")(port);

/**
 * Create server.
 */
const server = net.createServer();

if (debugMessage) {
  console.log(debugMessage);
}

/*
* Storages instances array
*/
let Storages = [];

/*
* List with cropjobs with schedule to run storage
*/
let joblist = [];

/**
 * Executes a storage
 * @param {*} clientSocket : terraMA2 application client
 * @param {*} client : terraMA2 database client
 * @param {*} storage : storage to execute
 */
async function runStorage(clientSocket, client, storage){
  return new Promise(async function resolvePromise(resolve, reject){
    try{
      console.log("Starting ", storage.name, " ", moment().format());
      logger.log("info","Starting: ", storage.name );
    
      if (storage.erase_all) //flag has priority 
        storage.keep_data = 0;

      var params = {
        clientSocket: clientSocket, 
        schema: schema, 
        storage: storage,
        client: client,
        logger: logger
      }

      await sto_core.createMessageTable(params);

      //Verifies which data_serie type will be stored
      logger.debug(storage.name + ": Checking which date will be deleted");
      var select_data_type= "SELECT data_series_semantics.code FROM \
      "+schema+".storages, \
      "+schema+".data_series, \
      "+schema+".data_series_semantics WHERE \
      data_series_semantics.id = data_series.data_series_semantics_id AND \
      data_series.id = \'" + storage.data_series_id + "\'";
     // console.log(select_data_type);
      var res1 = await client.query(select_data_type);

      var data_serie_code = res1.rows[0].code;
      storage.data_serie_code = res1.rows[0].code;
      switch (data_serie_code){
        case "GRID-geotiff":
          var res_storage = await sto_core.StoreTIFF(params)
          storage.process.last_process_timestamp = moment();
          storage.process.status = StatusLog.DONE;
          var msg ;
          if (res_storage.flag){
            if (storage.zip){
              msg = storage.name + ": added files: " + res_storage.deleted_files + " from " + res_storage.path + " to " + res_storage.zipfile;
            }
            else if (storage.backup){
              msg = storage.name + ": moved files: " + res_storage.moved_files + " from " + res_storage.path + " to " + res_storage.data_out;
            }
            else{
              msg =storage.name + ": removed files: " + res_storage.deleted_files + " from " + res_storage.path;
            }

            logger.log('info', msg);
            logger.log('info', "Finalized execution of " + storage.name);
            if (res_storage.moved_files === 0 && res_storage.deleted_files === 0){
              storage.process.description = "No data to process"
            }
            else{
              storage.process.data_timestamp = res_storage.data_timestamp;
              storage.process.description = msg;
            }
          }
          else{
            storage.process.description = res_storage.description;
            logger.log('info', "Finalized execution of " + storage.name + " -> " + res_storage.description);
          }

          break;

        case "DCP-single_table":
          await sto_core.StoreSingleTable(params);
 
        break;
        case "DCP-postgis":
        case "ANALYSIS_MONITORED_OBJECT-postgis":
        case "OCCURRENCE-postgis":
          await sto_core.StoreNTable(params);

          logger.log('info', "Finalized execution of " + storage.name);
          storage.process.last_process_timestamp = moment();
          storage.process.status = StatusLog.DONE;
  
          break;

        default:
        console.log("erro -");
        logger.error(storage.name + ": Invalid Data Serie Type!");
        throw("Invalid Data Serie Type!");
      }

      var obj = {
        automatic : true,//storage.automatic_schedule_id ? true : false,
        execution_date : moment().format(),
        instance_id : service_instance_id,
        result : true,
        process_id : storage.id,
        storage : storage
      }

      return resolve(obj);
    }
    catch(e){
      console.log(e);
      return reject(e);
    }
  });
};

function updateStorage(newstorage)
{
  for(let i = 0; i < Storages.length; i++) {
    if (Storages[i].id === newstorage.id){
      return Storages.splice(i,1, newstorage);
    } 
  };
}

/**
 * adds the storage in the array and schedules the execution in the list of jobs
 * @param {*} clientSocket : terraMA2 application client
 * @param {*} client : terraMA2 database client
 * @param {*} storages_new : new storage to add or update
 * @param {*} projects : projects list, only to verifiy if project is active
 */
async function addStorage(clientSocket, client, storages_new, projects){
  try{
    for (var storage of storages_new){
 
      var storagefound = Storages.find(s => s.id === storage.id);

      if (storagefound){
        updateStorage(storage);
        logger.log("info", "Storage Updated: ", storage.name);
       }
      else {
        Storages.push(storage);
        if (storage.active){
          logger.log("info", "Storage Added: ", storage.name);
        }
      }
      if (storage.active){
        var res = await client.query("Select * from " + schema + ".projects where id = \'" + storage.project_id + "\'");
        if (res.rowCount){
          if (res.rows[0].active){
            if (storage.schedule_type.toString() === ScheduleType.MANUAL || !storage.schedule_id){//without schedule, manual, so not cron
              joblist.push({
                id: storage.id,
                job: undefined
              });
            }
            else{
              var sql = "Select * from " + schema + ".schedules where id = \'" + storage.schedule_id + "\'";
              console.log(sql);
              var res1 = await client.query(sql);
              if (res1.rowCount){
                //when it is weekly, it should subtract one from the value, the bank stores from 1 to 7, 
                //but cron uses from 0 to 6 (Sunday to Saturday) 
                var freq = res1.rows[0].frequency ? res1.rows[0].frequency : res1.rows[0].schedule - 1;
                var unit = res1.rows[0].frequency_unit ? res1.rows[0].frequency_unit : res1.rows[0].schedule_unit;
                if (freq === undefined || unit === undefined){//without schedule, manual, so not cron
                  joblist.push({
                    id: storage.id,
                    job: undefined
                  });
                }
                else{  
                  var freq_seconds = moment.duration(freq, unit).asSeconds();
                  var start_time = res1.rows[0].frequency_start_time ? 
                  new moment(res1.rows[0].frequency_start_time, "HH:mm:ssZ") : res1.rows[0].schedule_time ?
                  new moment(res1.rows[0].schedule_time, "HH:mm:ssZ") : "0:0:0";

                  var rule;
                  if (res1.rows[0].schedule){
                    switch (res1.rows[0].schedule_unit.toLowerCase()){
                      case 'w':
                      case 'wk':
                      case 'week':
                      case 'weeks':
                        rule = start_time.second() + " " + start_time.minute() + " " + start_time.hour() + " * * " + freq;
                        break;
                    }
                  }
                  else{
                    switch (unit.toLowerCase()){
                      case 's':
                      case 'sec':
                      case 'second':
                      case 'seconds':
                        rule = "*/"+freq + " * * * * *";
                        break;
                      case 'min':
                      case 'minute':
                      case 'minutes':
                          rule = start_time.second() + " */" + freq + " * * * *";
                        break;
                      case 'h':
                      case 'hour':
                      case 'hours':
                          rule = start_time.second() + " " + start_time.minute() + " */" + freq + " * * *";
                        break;
                      case 'd':
                      case 'day':
                      case 'days':
                          rule = start_time.second() + " " + start_time.minute() + " " + start_time.hour() + " */" + freq + " * *";
                       break;
                    }
                  }
    
                  var newjob = new CronJob(rule, async function(){
                    for (var job of joblist) {
                      if (job.job === this){
                        storage = Storages.find(s => s.id === job.id) ;
                        logger.debug(storage.name + ": Setting schedule " + this.cronTime.source);
                        console.log(storage.name, job.job.cronTime.source);
                        this.stop();
                        runStorage(clientSocket, client, storage)
                        .catch(err =>{
                          console.log(storage.name, err);
                          this.start();
                        })
                        .then(obj => {
                          var buffer = TcpManager.makebuffer_be(Signals.PROCESS_FINISHED_SIGNAL, obj) ;
                          clientSocket.write(buffer);
                          logger.debug(storage.name + ": Finished");
                          this.start();
                          var lastdate = this.lastDate();
                          var nextdates = this.nextDates();
                          logger.log("info", storage.name + ": last date " + lastdate + " next date execution " + nextdates.format());
                         });
                        break;
                      }
                    }
                  });

                  logger.log("info", storage.name + " scheduled to run: " + newjob.nextDates().format());

                  var update = false;
                  for (var job of joblist){
                    if (job.id === storage.id){
                      job.job = newjob;
                      update = true;
                      break;
                    }
                  }
    
                  if (!update)
                    joblist.push({
                      id: storage.id,
                      job: newjob
                    });
                  
                  newjob.start();
                }
              }
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

let beginOfMessage = "(BOM)\0";
let endOfMessage = "(EOM)\0";

let extraData;
let tempBuffer;
let parsed;
let service_instance_id;
let service_instance_name;
let serviceLoaded_ = false;
let isShuttingDown_ = false;
let start_time;

/**
 * Listen on provided port, on all network interfaces.
 */
server.listen(port, 'localhost', () =>{
  console.log('Storage is running on port ' + port +'.');
});

server.on('error', onError);
server.on('listening', onListening);

let config_db = {
  user: contextConfig.username,
  password: contextConfig.password,
  host: contextConfig.host,
  port: contextConfig.port,
  database: contextConfig.database
};

console.log('Config_db', config_db);

/**
 * Handling received messages from TerraMA2
 * @param {*} clientSocket : TerraMA2 application client
 * @param {*} parsed : message received
 * @param {*} client_Terrama2_db : database client
 */
async function messageTreatment(clientSocket, parsed, client_Terrama2_db){

  switch(parsed.signal){
    case Signals.TERMINATE_SERVICE_SIGNAL:
    console.log("TERMINATE_SERVICE_SIGNAL");
    isShuttingDown_ = true;
    var buffer = await TcpManager.makebuffer_be(Signals.TERMINATE_SERVICE_SIGNAL);
    clientSocket.write(buffer);
    
    process.exit(0);

    break;

    case Signals.STATUS_SIGNAL:
      if (!serviceLoaded_){
        var buffer = await TcpManager.makebuffer_be(Signals.STATUS_SIGNAL, {service_loaded:false}) ;
      }
      else{
        var obj={
          instance_id : service_instance_id,
          instance_name : service_instance_name,
          logger_online : true,
          service_loaded : true,
          shutting_down : false,
          start_time : start_time,
          terrama2_version : version
        }
        var buffer =  await TcpManager.makebuffer_be(Signals.STATUS_SIGNAL, obj) ;
      }
      await clientSocket.write(buffer);
      break;

    case Signals.ADD_DATA_SIGNAL:
      console.log("ADD_DATA_SIGNAL");
      if (parsed.message.Storages){
        try{
          await addStorage(clientSocket, client_Terrama2_db, parsed.message.Storages, parsed.message.Projects);

          var buffer =  await TcpManager.makebuffer_be(Signals.VALIDATE_PROCESS_SIGNAL, {}) ;
          clientSocket.write(buffer);
        }
        catch(e){
          logger.error(e);
        }
      }
    
      break;
    case Signals.START_PROCESS_SIGNAL:
      console.log("START_PROCESS_SIGNAL");
      var storage_id = parsed.message.ids[0];

      for (var job of joblist) {
        if (job.id === storage_id){
          var storage = Storages.find(s => s.id === job.id) ;
          if (job.job){
          //   console.log(storage.name, moment().format(), storage.name, " job ", job.job.cronTime.source, " this ", this.cronTime.source);
             job.job.stop();
          }
          var obj ={
            instance_id: storage.service_instance_id,
            log:{
              data: "",
              data_timestamp: "",
              last_process_timestamp: moment().format(),
              messages: [],
              process_id: storage_id,
              start_timestamp: moment().format(),
              status: 2
            },
            process_id: storage_id
          }
          var objs =[];
          objs.push(obj);

          var buffer = TcpManager.makebuffer_be(Signals.LOG_SIGNAL, objs) ;
          console.log(buffer.toString());
          clientSocket.write(buffer);

          runStorage(clientSocket, client_Terrama2_db, storage)
          .catch(err =>{
            logger.error(err);
            if (job.job) job.job.start();
          })
          .then(obj1 => {
            var buffer = TcpManager.makebuffer_be(Signals.PROCESS_FINISHED_SIGNAL, obj1) ;
            clientSocket.write(buffer);
            console.log("Finishing", obj1.storage.name, " ", moment().format())
            if (job.job) job.job.start();
          });
          break;
        }
      }
      break;

    case Signals.LOG_SIGNAL:
      //console.log("LOG - process_ids",  parsed.message.process_ids);
      var begin = parsed.message.begin;
      var end = parsed.message.end;
      var process_id = parsed.message.process_ids; //lista com ids dos processos
      sto_core.getLogs(client_Terrama2_db, schema, service_instance_id, process_id, begin, end)
      .catch(err =>{
        logger.error(err);
      })
      .then(logs=>{
        var buffer = TcpManager.makebuffer_be(Signals.LOG_SIGNAL, logs) ;
        clientSocket.write(buffer);
      });
      break;

    case Signals.REMOVE_DATA_SIGNAL:
      console.log("REMOVE_DATA_SIGNAL");
      break;

    case Signals.PROCESS_FINISHED_SIGNAL:
      console.log("PROCESS_FINISHED_SIGNAL");
      serviceLoaded_ = false;
      break;

    case Signals.UPDATE_SERVICE_SIGNAL:
      console.log("UPDATE_SERVICE_SIGNAL");
    //Need to discover how many theads are supported
      if (parsed.message.instance_id){
        serviceLoaded_ = true;
        service_instance_id = parsed.message.instance_id;
        service_instance_name = parsed.message.instance_name;

        config_db.user= parsed.message.log_database.PG_USER;
        config_db.password= parsed.message.log_database.PG_PASSWORD;
        config_db.host= parsed.message.log_database.PG_HOST;
        config_db.port= parsed.message.log_database.PG_PORT;
        config_db.database= parsed.message.log_database.PG_DB_NAME;

        var obj={
          serviceLoaded_ : true,
          instance_id : service_instance_id,
          instance_name : service_instance_name
        }
      }
      else{
        var buffer =  await TcpManager.makebuffer_be(Signals.UPDATE_SERVICE_SIGNAL, parsed.message) ;
        console.log("UPDATE_SERVICE_SIGNAL", buffer);
        clientSocket.write(buffer);
      }
      break;

    case Signals.VALIDATE_PROCESS_SIGNAL:
      var buffer =  await TcpManager.makebuffer_be(Signals.VALIDATE_PROCESS_SIGNAL, obj) ;
      console.log("VALIDATE_PROCESS_SIGNAL", buffer);
      clientSocket.write(buffer);
      break;
    }
}

let pool = new pg.Pool(config_db);

pool.connect().then(client_Terrama2_db => {
  server.on('connection', async function(clientSocket) {
    console.log('CONNECTED: ' + clientSocket.remoteAddress + ':' + clientSocket.remotePort);

    start_time = moment().format();

    clientSocket.on('data', async function(byteArray) {

      console.log("RECEIVED: ", byteArray);

      clientSocket.answered = true;

      // append and check if the complete message has arrived
      tempBuffer = storageUtils._createBufferFrom(tempBuffer, byteArray);

      let completeMessage = true;
      // process all messages in the buffer or until we get a incomplete message
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
            parsed = storageUtils.parseByteArray(byteArray);
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
            var end_pos = tempBuffer.indexOf(endOfMessage);
            const eom1 = tempBuffer.toString('ascii', end_pos, end_pos+endOfMessage.length);
          //  const eom = tempBuffer.toString('ascii', expectedLength + beginOfMessage.length, expectedLength+headerSize);
            if(eom1 !== endOfMessage) {
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
            parsed = storageUtils.parseByteArray(tempBuffer);
            // get next message in the buffer for processing
            tempBuffer = extraData;
          }
          
          console.log("Size: " + parsed.size + " Signal: " + parsed.signal + " Message: " + JSON.stringify(parsed.message, null, 4));
      
          await messageTreatment(clientSocket, parsed, client_Terrama2_db);
        }
        catch(e){
          logger.error(e);
          throw (e);
        }
      }
    });
    clientSocket.on('error', function(err) {
      console.log(err);
    });
  });
});

// detecting sigint error and then close server
process.on('SIGINT', async () => {
  TcpService.finalize();
  TcpService.disconnect();

   server.close(() => {
     logger.log('info','Storage finalized');

     process.exit(0);
   });

  for (var key in connections)
    connections[key].destroy();
});

process.on('message', async (msg) =>{
  console.log(msg);
})


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
