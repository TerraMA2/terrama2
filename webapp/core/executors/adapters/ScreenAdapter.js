"use strict";

// Dependencies
var util = require('util');
var logger = require("./../../Logger");
var Promise = require('./../../Promise');

/**
 * It defines a generic interface to handle command executors
 * @type {BaseAdapter}
 */
var BaseAdapter = require("./BaseAdapter");

/**
 * An adapter of Screen. It executes a screen command from executor in both SSH or Local.
 * 
 * @inherits BaseAdapter
 * @class ScreenAdapter
 */
var ScreenAdapter = function() {
  BaseAdapter.call(this);
};

// javascript inheritance way
ScreenAdapter.prototype = Object.create(BaseAdapter.prototype);
ScreenAdapter.prototype.constructor = ScreenAdapter;

/**
 * It formats a terrama2 service command. 
 * Syntax is: screen -dmS pathToExecutable serviceType servicePort
 * 
 * @param {ServiceInstance} serviceInstance - A TerraMA² service instance data model
 * @param {string} command - A brief command. "ANALYSIS 10000"
 * @return {string} a command formatted.
 */
ScreenAdapter.prototype.make = function(serviceInstance, command) {
  var commandId = util.format("%s_%s", serviceInstance.id, serviceInstance.port);
  return util.format("screen -dmS %s %s", commandId, command);
};

/**
 * It executes a screen command from given executor
 * @param {Executor} executor - An executor
 * @param {string} command - A command to run
 * @param {ServiceInstance} serviceInstance - A TerraMA² service instance data model
 * @param {Object} extra - A javascript object with helper properties
 */
ScreenAdapter.prototype.executeCommand = function(executor, command, serviceInstance, extra) {
  var self = this;
  return new Promise(function(resolve, reject) {
    var screenCommand = self.make(serviceInstance, command);
    logger.debug(screenCommand);
    var localOptions = {};

    return executor.execute(screenCommand, [], localOptions)
      .then(function(code) {
        return resolve(code);
      })
      .catch(function(err) {
        return reject(err, -1);
      });
  });
};

module.exports = ScreenAdapter;
