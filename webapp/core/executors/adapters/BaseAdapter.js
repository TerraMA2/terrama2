'use strict';

/**
 * Generic interface for handling command executors
 */
var BaseAdapter = module.exports = function() { };

/**
 * @returns {string}
 */
BaseAdapter.prototype.make = function(service, command) {
  throw new Error("It must be implemented");
};

/**
 * @returns {Promise<number>}
 */
BaseAdapter.prototype.executeCommand = function(executor, command, service, extra) {
  throw new Error("It must be implemented");
};