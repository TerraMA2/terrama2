'use strict';

/**
 * Socket responsible for handling Tcp events from TcpService in front end application.
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

  // TerraMA2 Utils
  var Utils = require('./../core/Utils');

  // TerraMA2 Enums
  var ServiceType = require('./../core/Enums').ServiceType;

  // common TcpService module
  var TcpService = require("./../core/facade/tcp-manager/TcpService");

  // TODO: remove it, since It also include TcpService. It must be changed
  var AnalysisFacade = require("./../core/facade/Analysis");

  /**
   * It describes when service is ready to start and notify all listeners
   *
   * @param {Object} resp - Response object
   * @param {number} resp.service - TerraMA² Service ID
   */
  TcpService.on("serviceStarting", function(resp) {
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

  /**
   * Defines log listener, containing the log array and service metadata
   *
   * @param {Object} resp - A response object with these values
   * @param {number} resp.status - A code status
   * @param {number} resp.service - A TerraMA² Service identifier
   * @param {any[]} resp.logs - Log values
   */
  TcpService.on("serviceLog", function(resp) {
    iosocket.emit("logResponse", resp);
  });

  /**
   * Defines stop listener. It does not represents that TerraMA² C++ has been closed. It defines that TERMINATE signal were sent
   * and C++ service replied successfully. To ensure service stopped, send status service after that.
   *
   * @param {Object} resp - Response object
   */
  TcpService.on("serviceStop", function(resp) {
    iosocket.emit("stopResponse", resp);
  });

  /**
   * Defines listener when socket closes
   *
   * @param {Object} resp - A response object with these values
   * @param {number} resp.status - A code status
   * @param {number} resp.service - A TerraMA² Service identifier
   * @param {boolean} resp.loading - Flag to indicates service loading. Always false
   * @param {boolean} resp.online - Flag to indicates current status of service. Always false.
   */
  TcpService.on("serviceClose", function(resp) {
    iosocket.emit("closeResponse", resp);
  });

  /**
   * Defines socket error listener. If there is any error during commutication, this listener will be triggered.
   *
   * @param {Object} resp - A response object with these values
   * @param {number} resp.status - A code status
   * @param {number} resp.service - A TerraMA² Service identifier
   * @param {string} resp.message - An error message
   */
  TcpService.on("serviceError", function(resp) {
    var errMessage = serviceErrorMessageHandler(resp);
    iosocket.emit("errorResponse", errMessage);
  });

  /**
   * Function to handle service error messages
   */
  var serviceErrorMessageHandler = function(err){
    var errMessageResponse = {
      message: "Unknown service error"
    };
    if (err){
      errMessageResponse.service = err.service;
      if (err.exception && err.exception.code){
        switch(err.exception.code){
          case "ECONNREFUSED":
          case "ECONNRESET":
            errMessageResponse.message = "Connection with service refused, check if the service is running!";
            break;
          case "EACCES":
            errMessageResponse.message = "Permission denied to service!";
            break;
          case "ETIMEDOUT":
            errMessageResponse.message = "Service connection time out!";
          default:
            break;
        }
      } else {
        if(err.message)
          errMessageResponse.message = err.message;
      }
    }
    return errMessageResponse;
  }

  /**
   * Defines a status listener to broadcast everyone.
   *
   * @param {Object} resp - A response object with these values
   * @param {number} resp.status - A code status
   * @param {number} resp.service - A TerraMA² Service identifier
   * @param {boolean} resp.checking - Identifies if service is checking
   * @param {boolean} resp.online - Flag to determines if service is running properly
   */
  TcpService.on("serviceRequestingStatus", function(resp) {
    iosocket.emit("statusResponse", resp);
  });

  /**
   * Defines a listener for TerraMA² Service version. Triggered only when starting TerraMA² service
   */
  TcpService.on("serviceVersion", function(resp) {
    iosocket.emit("serviceVersion", resp);
  });

  /**
   * It defines a listener to broadcast everyone when a process has been finished. It also retrieves a metadata of process.
   *
   * @param {any}                resp - A Response object
   * @param {string}             resp.name - Context Name. Use it to display in GUI interface
   * @param {number}             resp.service - TerraMA² Service identifier
   * @param {Analysis|Collector} resp.process - TerraMA2 Process Metadata
   */
  TcpService.on("processFinished", function handleProcessFinished(resp) {
    iosocket.emit("processFinished", resp);
  });

  // Socket connection event
  iosocket.on('connection', function(client) {
    /**
     * Listener for handling Analysis Validation from TerraMA² services (front)
     */
    function onProcessValidated(resp) {
      client.emit("processValidated", resp);
    }

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
     * Listener for handling Analysis Validation from Front-User user. It emits signal to C++ TcpService in order to validate
     *
     * @param {Object} json - TerraMA² Api request
     * @param {Object} json.analysis - TerraMA² Analysis Instance Values
     * @param {Object} json.storager - TerraMA² Analysis Storager
     * @param {Object} json.schedule - TerraMA² Schedule Values
     * @param {number} json.projectId - TerraMA² current project id
     */
    function onValidateAnalysisRequest(json) {
      var analysis = json.analysis;
      var storager = json.storager;
      var schedule = json.schedule;
      var projectId = json.projectId;
      return AnalysisFacade.validate(analysis, storager, schedule, projectId)
        .then(function(dummyAnalysis) {
          TcpService.validateProcess({
            "Analysis": [dummyAnalysis.toObject()],
            "DataSeries": [dummyAnalysis.dataSeries.toObject()]
          }, dummyAnalysis.instance_id);
        })
        .catch(function(err) {
          return client.emit("processValidatedError", {
            error: err.toString()
          });
        });
    }

    /**
     * Register the process run listener. It is the only one listener registered on each user, since it does not need to notify all
     * It must be removed on socket disconnection
     */
    TcpService.on("processRun", onProcessRun);

    /**
     * Register the analysis validation listener.
     * It must be removed on socket disconnection
     */
    TcpService.on("processValidated", onProcessValidated);

    /**
     * It just define on front-end socket disconnection. It remove a process run listener due it is the only one registered each one user
     *
     * @returns {void}
     */
    function onDisconnect() {
      TcpService.removeListener("processRun", onProcessRun);
      TcpService.removeListener("processValidated", onProcessValidated);
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
     * Listener for handling status signal when try to delete an object.
     *
     * **This method does not throw exception.**
     *
     * @emits #statusToDeleteResponse
     * @param {Object} json - A given arguments sent by client
     * @param {number} json.service - A TerraMA² service instance id
     */
    async function onStatusToDeleteRequest(json){
      const result = await TcpService.statusToDelete(json);

      client.emit("statusToDeleteResponse", result);
    }

    /**
     * Listener for handling STOP service signal. When called, it sends a STOP_SERVICE signal followed by a STATUS_SERVICE.
     * Once TerraMA² executable receives STOP_SERVICE, the thread handler waits for the completition and the internal state
     * will be changed to "shutting down", so it may take a few minutes to finalize properly.
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

    /**
     * It stops all running TerraMA² services
     */
    function handleStopAll() {
      TcpService.stopAll();
    }

    // registering socket io listeners
    client.on("start", onStartRequest);
    client.on("run", onRunRequest);
    client.on("status", onStatusRequest);
    client.on("statusToDelete", onStatusToDeleteRequest);
    client.on("stop", onStopRequest);
    client.on("log", onLogRequest);
    client.on("disconnect", onDisconnect);
    client.on("validateAnalysis", onValidateAnalysisRequest);
    client.on("stopAll", handleStopAll);
  });
};

module.exports = TcpSocket;
