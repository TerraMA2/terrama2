var Promise = require('bluebird');
var execAsync = require('child_process').exec;
var OS = require('./Enums').OS;
var ScreenAdapter = require('./ssh/ScreenAdapter');


/**
 * This class allows to run commands like terminal style (local)
 * @class Executor
 *
 * @author Raphael Willian da Costa
 *
 * @property {object} adapter - An adapter command to run (ScreenAdapter).
 * @property {OS} platform - An enum value to represent Operational System
 */
var Executor = module.exports = function(adapter) {
  this.platform = null;
  this.adapter = adapter;
};

Executor.prototype.connect = function(serviceInstance) {
  var self = this;
  return new Promise(function(resolve, reject) {
    self.serviceInstance = serviceInstance;
    // detecting platform
    self.execute("ipconfig").then(function(code) {
      self.platform = OS.WIN;
    }).catch(function(err) {
      return self.execute("uname");
    }).finally(function() {
      resolve();
    })
  });
};

Executor.prototype.execute = function(command) {
  var self = this;
  return new Promise(function(resolve, reject) {
    var child = execAsync(command);

    child.on('close', function(code, signal) {
      console.log("Executor close ", code, signal);
      if (code !== 0)
        return reject(new Error("Error: exit code " + code));

      resolve(code);
    });

    child.on('error', function(err) {
      console.log(err);
    });

    child.stdout.on('data', function(data) {
      if (command === "uname") {
        var dataStr = data.toString();
        var platform = dataStr.substring(0, dataStr.indexOf('\n'));
        switch(platform) {
          case OS.LINUX:
          case OS.MACOSX:
            self.platform = platform;
            self.adapter = ScreenAdapter;
            break;
          default:
            console.log("Unknown platform");
            self.platform = OS.UNKNOWN;
        }
      }
    });

    child.stderr.on('data', function(data) {
      console.log("Executor Error: ", data.toString());
    })
  });
};

Executor.prototype.startService = function(command) {
  var self = this;
  return new Promise(function(resolve, reject) {
    self.adapter.executeCommand(self, command, self.serviceInstance, {}).then(function(code) {
      resolve(code);
    }).catch(function(err) {
      reject(err);
    });
  })
};

Executor.prototype.disconnect = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    resolve();
  })
};