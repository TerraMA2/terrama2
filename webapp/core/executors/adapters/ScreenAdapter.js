"use strict";

// Dependencies
var util = require('util');
var Promise = require('bluebird');

/**
 * An adapter of Screen. It executes a screen command from executor.
 */
var ScreenAdapter = {};

/**
 * It formats a terrama2 service command. 
 * Syntax is: screen -dmS pathToExecutable serviceType servicePort
 * 
 * @param {ServiceInstance} serviceInstance - A TerraMA² service instance data model
 * @param {string} command - A brief command. "ANALYSIS 10000"
 * @return {string} a command formatted.
 */
ScreenAdapter.make = function(serviceInstance, command) {
  var commandId = util.format("%s_%s", serviceInstance.id, serviceInstance.port);
  return util.format("screen -dmS %s %s", commandId, command);
};

/**
 * It executes a screen command from given executor
 * @param {Executor} ssh - An executor
 * @param {string} command - A command to run
 * @param {ServiceInstance} serviceInstance - A TerraMA² service instance data model
 * @param {Object} extra - A javascript object with helper properties
 */
ScreenAdapter.executeCommand = function(ssh, command, serviceInstance, extra) {
  var self = this;
  return new Promise(function(resolve, reject) {
    var screenCommand = self.make(serviceInstance, command);
    console.log(screenCommand);

    ssh.execute(screenCommand).then(function(code) {
      resolve(code);
    }).catch(function(err) {
      reject(err, -1);
    });
  });
};

module.exports = ScreenAdapter;
