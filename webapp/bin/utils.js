"use strict";

const pg = require('pg');
var Signals = require('./../core/Signals');
const Regex = require('xregexp');

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
  },

  terramaMask2Regex : async function (mask){
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
  
}
  
  