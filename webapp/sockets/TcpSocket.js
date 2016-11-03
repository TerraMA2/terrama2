'use strict';

/**
 * Socket responsible for handling Tcp events from TcpManager in front end application.
 * @class TcpSocket
 *
 * @author Raphael Willian da Costa
 *
 * @property {object} iosocket - Sockets object.
 * @property {object} memberScriptChecker - Script checker class.
 */
var TcpSocket = function(io) {

  // Sockets object
  var iosocket = io.sockets;

  // TcpManager
  var TcpManager = require('./../core/TcpManager');

  // TerraMA2 Utils
  var Utils = require('./../core/Utils');

  // TerraMA2 Enums
  var ServiceType = require('./../core/Enums').ServiceType;

  // DataManager
  var DataManager = require('./../core/DataManager');

  // common TcpService module
  var TcpService = require("./../core/facade/tcp-manager/TcpService");

  /**
   * It describes when service is ready to start and notify all listeners
   * 
   * @param {Object} resp - Response object
   * @param {number} resp.service - TerraMA² Service ID
   */
  TcpService.on("serviceStarting", (resp) => {
    iosocket.emit("statusResponse", {
      status: 200,
      loading: true,
      service: resp.service
    });
  });

  /**
   * Defines a status listener. Once received, it emits to front end socket.
   * 
   * @param {Object} resp - A response object with these values 
   * @param {number} resp.status - A code status
   * @param {number} resp.service - A TerraMA² Service identifier
   * @param {boolean} resp.shutting_down - Flag to handle if service is stopping
   * @param {boolean} resp.loading - Flag to determines service loading. Useful in GUI pages
   * @param {boolean} resp.online - Flag to determines if service is running properly
   */
  TcpService.on("serviceStatus", function(resp) {
    iosocket.emit("statusResponse", resp);
  });

  TcpService.on("serviceLog", function(resp) {
    iosocket.emit("logResponse", resp);
  });

  TcpService.on("serviceStop", function(resp) {
    iosocket.emit("stopResponse", resp);
  });

  TcpService.on("serviceClose", function(resp) {
    iosocket.emit("closeResponse", resp);
  });

  TcpService.on("serviceError", function(resp) {
    iosocket.emit("errorResponse", resp);
  });

  // Socket connection event
  iosocket.on('connection', function(client) {
    /**
     * Listener for handling Process Run from TerraMA² TcpService.
     * 
     * @todo It will emits serviceError in order to notify user that service is not running.
     * @param {Object} resp - A response object
     * @param {number} resp.service - TerraMA² service identifier
     */
    function onProcessRun(resp) {
      client.emit("runResponse", resp);
    }

    /** 
     * Register the process run listener. It is the only one listener registered on each user, since it does not need to notify all
     * It must be removed on socket disconnection
     */
    TcpService.on("processRun", onProcessRun);

    /**
     * It just define on front-end socket disconnection. It remove a process run listener due it is the only one registered each one user
     * 
     * @returns {void}
     */
    function onDisconnect() {
      TcpService.removeListener ("processRun", onProcessRun);
    }

    /**
     * Listener for handling client start request. When called,
     * it retrieves a service instance and tries to start TerraMA² service executable.
     * Once success during start, it performs a socket connection using Service configurations.
     * After that, it sends status service signal to communicate
     * 
     * @param {Object} json - A given arguments sent by client
     * @param {number} json.service - A TerraMA² service instance id
     */
    function onStartRequest(json) {
      return TcpService.start(json);
    }

    /**
     * Listener for handling start process signal. When it called, it tries to send START_PROCESS signal
     * in order to forcing a process to execute in TerraMA².
     * 
     * @param {Object} processObject - A given arguments sent by client
     * @param {number} processObject.service_instance - A TerraMA² service instance id
     */
    function onRunRequest(processObject){
      return TcpService.run(processObject);
    }

    /**
     * Listener for handling status signal. When it called, it tries to connect to the socket and retrieve a
     * life time using STATUS_SIGNAL.
     * 
     * @param {Object} json - A given arguments sent by client
     * @param {number} json.service - A TerraMA² service instance id
     */
    function onStatusRequest(json) {
      return TcpService.status(json);
    }

    /**
     * Listener for handling STOP service signal. When called, it sends a STOP_SERVICE signal followed by a STATUS_SERVICE.
     * Once TerraMA² executable receives STOP_SERVICE, it starts changing shutdown the running active processes, so it may
     * take a few seconds/minutes to finish. 
     * 
     * @param {Object} json - A given arguments sent by client
     * @param {number} json.service - A TerraMA² service instance id
     */
    function onStopRequest(json) {
      return TcpService.stop(json);
    }

    /**
     * Listener for handling Log request signal. When called, it maps the cached logs and if necessary request
     * for others through LOG_SIGNAL in socket communication.
     * 
     * @param {Object} json - A given arguments sent by client
     * @param {number} json.begin - A begin offset to retrieve logs
     * @param {number} json.end - An end offset to retrieve logs
     */
    function onLogRequest(json) {
      return TcpService.log(json);
    }

    // registering socket io listeners
    client.on("start", onStartRequest);
    client.on("run", onRunRequest);
    client.on("status", onStatusRequest);
    client.on("stop", onStopRequest);
    client.on("log", onLogRequest);
    client.on("disconnect", onDisconnect);
  });
};

module.exports = TcpSocket;
