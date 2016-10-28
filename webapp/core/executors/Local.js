'use strict';

var Promise = require('bluebird');
var execAsync = require('child_process').exec;
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
 * @property {object} adapter - An adapter command to run (ScreenAdapter|LocalSystemAdapter).
 * @property {OS} platform - An enum value to represent Operational System
 */
var LocalExecutor = module.exports = function(adapter) {
  this.platform = null;
  this.adapter = adapter;
};

LocalExecutor.prototype.connect = function(serviceInstance) {
  var self = this;
  return new Promise(function(resolve) {
    self.serviceInstance = serviceInstance;
    // detecting platform
    self.execute("ipconfig").then(function() {
      self.platform = OS.WIN;
    }).catch(function() {
      return self.execute("uname");
    }).finally(function() {
      resolve();
    });
  });
};
/**
 * It executes a string command in process. 
 * 
 * @param {string} command - Command to execute
 * @returns {Promise<string>}
 */
LocalExecutor.prototype.execute = function(command) {
  var self = this;
  return new Promise(function(resolve, reject) {
    var child = execAsync(command);

    child.on('close', function(code, signal) {
      console.log("LocalExecutor close ", code, signal);
      if (code !== 0) {
        return reject(new Error("Error: exit code " + code));
      }

      resolve(code);
    });

    child.on('error', function(err) {
      console.log(err);
    });

    child.stdout.on('data', function(data) {
      if (command === "uname") {
        var dataStr = data.toString();
        var platform = dataStr.substring(0, dataStr.indexOf('\n'));

        self.platform = platform;
        switch(platform) {
          case OS.LINUX:
            self.adapter = ScreenAdapter;
            break;
          case OS.MACOSX:
            self.adapter = new LocalSystemAdapter();
            break;
          default:
            console.log("Unknown platform");
            self.platform = OS.UNKNOWN;
        }
      }
    });

    child.stderr.on('data', function(data) {
      console.log("LocalExecutor Error: ", data.toString());
    });
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