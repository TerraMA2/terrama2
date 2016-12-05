'use strict';

/**
 * It defines a generic interface to handle command executors
 * @type {BaseAdapter}
 */
var BaseAdapter = require("./BaseAdapter");

/**
 * Enable to run TerraMA² service using local resource.
 * 
 * For Linux/MacOSX enviroments, it uses "nohup" and background processes "&"
 * For Windows enviroment, it uses "start" background process
 * 
 * @class LocalSystemAdapter
 * @inherits BaseAdapter
 */
var LocalSystemAdapter = module.exports = function() { };

// dependencies
var Utils = require("./../../Utils");
var PromiseClass = require("./../../Promise");
var logger = require("./../../Logger");

/**
 * It will generates command to use in executor. The output might be: nohup COMMAND &
 * 
 * @example
 * ... collectorService...
 * var local = new LocalSystemAdapter()
 * local.make(collectorService, "./terrama2_service COLLECTOR 6543");
 * >> nohup ./terrama2_service COLLECTOR 6543 &
 * 
 * @param {ServiceInstance} service - A TerraMA² service to execute
 * @param {string} command - A built command to run terrama2_service
 * @returns {string}
 */
LocalSystemAdapter.prototype.make = function(service, command) {
  return Utils.format("nohup %s > terrama2.out > terrama2.err < /dev/null &", command);
};

/**
 * It prepares a command as array of string in order to spawn as nodejs process
 * 
 * @param {string} command - A command to split
 * @returns {string[]}
 */
LocalSystemAdapter.prototype.commandArgs = function(command) {
  return BaseAdapter.prototype.commandArgs.call(this, command);
};

/**
 * It performs command execution on executor
 * 
 * @param {Executor} executor - A TerraMA² enviroment executor where command should run.
 * @param {string} command - A built command to run terrama2_service
 * @param {ServiceInstance} service - A TerraMA² service to execute
 * @param {Object} extra - An extra object with enviroment data
 */
LocalSystemAdapter.prototype.executeCommand = function(executor, command, service, extra) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    var commandArgs = self.commandArgs(command);
    var localCommand = "nohup";
    var localOptions = {stdio: "ignore"};

    logger.debug(localCommand, commandArgs);

    return executor
      .execute(localCommand, commandArgs, localOptions)
      .then(function(code) {
        return resolve(code);
      })
      .catch(function(err) {
        return reject(err, -1);
      });
  });
};