"use strict";

const pg = require('pg');
var Signals = require('./../core/Signals');

  
function getTcpSignal  (value) {
    switch(value) {
      case Signals.TERMINATE_SERVICE_SIGNAL:
      case Signals.STATUS_SIGNAL:
      case Signals.ADD_DATA_SIGNAL:
      case Signals.START_PROCESS_SIGNAL:
      case Signals.LOG_SIGNAL:
      case Signals.REMOVE_DATA_SIGNAL:
      case Signals.PROCESS_FINISHED_SIGNAL:
      case Signals.UPDATE_SERVICE_SIGNAL:
      case Signals.VALIDATE_PROCESS_SIGNAL:
        return value;
      default:
        return -1;
    }
  };


var Utils = module.exports = {
    clone: function(object) {
      return cloneDeep(object);
    },
  
/**
 * Creates a new Buffer based on any number of Buffer
 *
 * @private
 * @param {Buffer} arguments Any number of Buffer as arguments, may have undefined itens.
 * @return {Buffer} The new Buffer created out of the list.
 */
_createBufferFrom : function () {
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
  },
  
  /**
   This method parses the bytearray received.
   @param {Buffer} byteArray - a nodejs buffer with bytearray received
   @return {Object} object - a javascript object with signal, message and size
  
   */
  parseByteArray : function (byteArray) {
    var messageSizeReceived = byteArray.readUInt32BE(0);
    var signalReceived = byteArray.readUInt32BE(4);
    var len = byteArray.length;
    var rawData = byteArray.slice(8, byteArray.length);
   // console.log(rawData.toString());
  
    // validate signal
    var signal = getTcpSignal(signalReceived);
    if (signal == -1){
      return {
        size: 0,
        signal: Signals.STATUS_SIGNAL,
        message: {}
      };
    }
    
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
  },

  makebuffer : async function  (signal, object) {
    try {
      if(isNaN(signal)) { throw TypeError(signal + " is not a valid signal!"); }
  
      var totalSize;
      var jsonMessage = "";
  
      if (object) {
        jsonMessage = JSON.stringify(object).replace(/\":/g, "\": ");
  
        // The size of the message plus the size of two integers, 4 bytes each
        totalSize = jsonMessage.length + 4;
      } else { totalSize = 4; }
  
      // creating buffer to store message
      var bufferMessage = new Buffer(jsonMessage);
  
      // Creates the buffer to be sent
      var buffer = Buffer.alloc(bufferMessage.length + 8 + 12);
  
      if (object) {
        // Writes the message (string) in the buffer with UTF-8 encoding
        bufferMessage.copy(buffer, 14, 0, bufferMessage.length);
      }
  
      // checking bufferMessage length. If it is bigger than jsonMessage,
      // then there are special chars and the message size must be adjusted.
      if (bufferMessage.length > jsonMessage.length) { totalSize = bufferMessage.length + 4; }
  
      // Writes the buffer size (unsigned 32-bit integer) in the buffer with big endian format
      buffer.write(beginOfMessage, 0);
      buffer.writeUInt32BE(totalSize, 6);
  
      // // Writes the signal (unsigned 32-bit integer) in the buffer with big endian format
      buffer.writeUInt32BE(signal, 10);
      buffer.write(endOfMessage, buffer.length - 6);
  
      console.log("makebuffer", buffer);
      return buffer;
    } catch (e) {
      throw e;// reject(e);
    }
  },
  
  
  databaseConnect : async function (database, logger){
    try{
      const config = {
        user: database.PG_USER,
        password: database.PG_PASSWORD,
        host: database.PG_HOST,
        port: database.PG_PORT,
        database: database.PG_DB_NAME,
        ssl: true
      };
  
      logger.info("Initializing database " + database.PG_DB_NAME);
      var pool = new pg.Pool(config);
      pool.connect().then(client =>{
        return Promise.resolve(client);
      });
    }
    catch (e){
      console.log(e);
      logger.error(e);
      return Promise.reject(e); 
    }
  }
}
  
  