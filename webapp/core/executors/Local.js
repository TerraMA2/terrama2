'use strict';

var Promise = require('./../Promise');
var logger = require("./../Logger");
var spawnAsync = require('child_process').spawn;
var execAsync = require("child_process").exec;
var OS = require('./../Enums').OS;
var ScreenAdapter = require('./adapters/ScreenAdapter');
var LocalSystemAdapter = require("./adapters/LocalSystemAdapter");


/**
 * This class allows to run commands like terminal style (local).
 * - For Linux enviroment, it uses screen command by default.
 * 
 * - For MacOSX enviroment, screen command does not work properly since the version is different than Linux Version. So, by default,
 * LocalSystemAdapter is applied in order to start TerraMA² service.
 * 
 * - For Windows enviroment, it uses LocalSystemAdapter in order to execute "start PATH_TO/terrama2_service".
 * 
 * @class LocalExecutor
 *
 * @author Raphael Willian da Costa
 *
 * @property {BaseAdapter} adapter - An adapter command to run (ScreenAdapter|LocalSystemAdapter).
 * @property {OS} platform - An enum value to represent Operational System
 */
var LocalExecutor = module.exports = function(adapter) {
  this.platform = null;
  this.adapter = adapter;
};

/**
 * It pretends to make a connection.
 * 
 * @param {Service} serviceInstance - TerraMA² Service instance where to run
 * @returns {Promise<null>}
 */
LocalExecutor.prototype.connect = function(serviceInstance) {
  var self = this;
  return new Promise(function(resolve) {
    self.serviceInstance = serviceInstance;
    // detecting platform
    return self.execute("uname", [], {})
      .catch(function(err) {
        self.platform = OS.WIN;
        return null;
      })
      .finally(function() {
        return resolve();
      });
  });
};
/**
 * It executes a string command in process. 
 * 
 * @param {string} command - Command to execute
 * @returns {Promise<string>}
 */
LocalExecutor.prototype.execute = function(command, commandArgs, options) {
  var self = this;
  return new Promise(function(resolve, reject) {
    options.detached = true;

    /**
     * It defines a executor type handler. For SSH, use exec. For nohup, use spawn 
     * @type {Executor}
     */
    var child;

    /**
     * Helper to define platform and respective adapter
     * 
     * @param {string} data - Command execution output
     */
    var defineAdapter = function(data) {
      var dataStr = data;
      var platform = dataStr.substring(0, dataStr.indexOf('\n'));

      self.platform = platform;
      switch(platform) {
        case OS.LINUX:
          self.adapter = new ScreenAdapter();
          break;
        case OS.MACOSX:
          self.adapter = new LocalSystemAdapter();
          break;
        default:
          logger.debug("Unknown platform. Setting default to " + OS.UNKNOWN);
          self.platform = OS.UNKNOWN;
      }
    };

    if (self.adapter instanceof LocalSystemAdapter) {

      child = spawnAsync(command, commandArgs, options);
      
      if (options.stdio != 'ignore'){
        var responseMessage = "";

        child.stdout.on('data', function(data) {
          responseMessage = data.toString();
        });

        child.on('close', function(code, signal) {
          logger.debug("LocalExecutor close ", code, signal);
          if (code !== 0) {
            return reject(new Error("Error: exit code " + code));
          }
          return resolve({code: code, data: responseMessage.replace("\n", "")});
        });

        child.on('error', function(err) {
          logger.error(err);
        });

      } else {
        //todo: remove it, since it just forcing resolve promise
        resolve({code: 0});
      }
      child.unref();

    } else {
      child = execAsync(command + " " + (commandArgs || []).join(" "));

      var responseMessage = "";

      child.on('close', function(code, signal) {
        logger.debug("LocalExecutor close ", code, signal);
        if (code !== 0) {
          return reject(new Error("Error: exit code " + code));
        }

        return resolve({code: code, data: responseMessage.replace("\n", "")});
      });

      child.on('error', function(err) {
        logger.error(err);
      });

      child.stdout.on('data', function(data) {
        if (command === "uname") {
          defineAdapter(data.toString());
        }

        responseMessage = data.toString();
      });

      // stream for handling errors data
      child.stderr.on('data', function(data) {
        logger.error("LocalExecutor Error: ", data.toString());
        responseMessage = data.toString();
      });
    } 
  });
};
/**
 * It executes a command in child process in order to start a TerraMA² service
 * 
 * @param {string} command - A TerraMA² command service to execute
 * @returns {Promise<number>} A promise with exit code
 */
LocalExecutor.prototype.startService = function(command) {
  var self = this;
  return new Promise(function(resolve, reject) {
    return self.adapter
      .executeCommand(self, command, self.serviceInstance, {})
      .then(function(code) {
        return resolve(code);
      })
      .catch(function(err) {
        return reject(err);
      });
  });
};
/**
 * It simulates LocalExecutor disconnect
 * 
 * @returns {Promise<null>}
 */
LocalExecutor.prototype.disconnect = function() {
  return new Promise(function(resolve) {
    return resolve();
  });
};