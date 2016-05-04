"use strict";

/**
 * Socket responsible for checking the analysis script.
 * @class AnalysisScriptChecker
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberScriptChecker - Script checker class.
 */
var AnalysisScriptChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Script checker class
  var memberScriptChecker = new (require('../core/ScriptChecker.js'))();

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // Python script request event
    client.on('checkPythonScriptRequest', function(json) {
      var returnObject = memberScriptChecker.checkPythonScript(json.script);

      client.emit('checkPythonScriptResponse', returnObject);
    });
  });
};

module.exports = AnalysisScriptChecker;
