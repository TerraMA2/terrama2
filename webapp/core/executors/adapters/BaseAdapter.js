'use strict';

/**
 * Generic interface for handling command executors
 */
var BaseAdapter = module.exports = function() { };

var Utils = require("./../../Utils");

/**
 * It retrieves a entire command to execute
 * 
 * @param {Service} service - TerraMA² Service object with destination values
 * @param {string} command - command brief ("PATH/terrama2_service TYPE port")
 * @returns {string}
 */
BaseAdapter.prototype.make = function(service, command) {
  throw new Error("It must be implemented");
};

/**
 * It prepares a command string into a array of values. When handling NodeJS child spawn, you must specify commandName and arguments
 * of that command. This functions aims to prepare and retrieve command arguments
 * 
 * @todo Improve it, removing dash values from arguments
 * @param {string} command - command brief ("PATH/terrama2_service TYPE port")
 * @returns {string[]} An array with values. Useful for pass to executor as arguments
 */
BaseAdapter.prototype.commandArgs = function(command) {
  if (Utils.isString(command)) {
    return command.split(" ");
  }
  return [];
};

/**
 * It performs a command execution on executor.
 * 
 * @param {Executor} executor - Executor where is to run
 * @param {string} command - command brief to execute
 * @param {Service} service - TerraMA² service
 * @param {Object} extra - An extra values with enviroment values
 * @returns {Promise<number>}
 */
BaseAdapter.prototype.executeCommand = function(executor, command, service, extra) {
  throw new Error("It must be implemented");
};