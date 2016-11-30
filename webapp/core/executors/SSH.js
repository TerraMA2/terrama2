"use strict";

var Client = require('ssh2').Client;
var logger = require("./../Logger");
var Promise = require("./../Promise");
var fs = require('fs');
var util = require('util');
var Utils = require("./../Utils");
var Enums = require('./../Enums');
var ScreenAdapter = require('./adapters/ScreenAdapter');
var LocalSystemAdapter = require("./adapters/LocalSystemAdapter");


/**
 * Class responsible for handling ssh connection.
 * @class SSHDispatcher
 */
var SSHDispatcher = module.exports = function(adapter) {
  this.client = new Client();
  this.connected = false;
  this.platform = null;
  this.adapter = adapter;
};

SSHDispatcher.prototype.connect = function(serviceInstance) {
  var self = this;

  return new Promise(function(resolve, reject) {
    self.serviceInstance = serviceInstance;
    self.client.on('ready', function() {
      self.connected = true;

      // detecting OS
      // win
      return self.execute("uname")
        .catch(function(err) {
          self.platform = Enums.OS.WIN;
          return null;
        }).finally(function() {
          return resolve();
        });
    });

    self.client.on('keyboard-interactive', function(name, instructions, instructionsLang, prompts, finish) {
      finish([]);
    });

    self.client.on('error', function(err) {
      logger.error("Error in SSH connection.", err);
      self.connected = false;
      return reject(new Error("Error while connecting. " + err.message));
    });

    var defaultDir = Utils.getUserHome() + '/.ssh/';
    Utils.findFiles(defaultDir, 'id_*').then(function(files) {
      var privateKey;
      files.some(function(file) {
        if (!file.endsWith('.pub')) {
          privateKey = file;
          return true;
        }
      });

      if (!privateKey) {
        return reject(new Error("Could not find private key in \"" + defaultDir + "\""));
      }

      self.client.connect({
        host: self.serviceInstance.host,
        port: self.serviceInstance.sshPort,
        username: self.serviceInstance.sshUser,
        privateKey: require('fs').readFileSync(privateKey),
        tryKeyboard: true
      });
    }).catch(function(err) {
      reject(err);
    });
  });
};

SSHDispatcher.prototype.disconnect = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!self.connected) {
      return reject(new Error("Could not disconnect. There is no such active connection"));
    }

    self.client.end();
    resolve();
  });
};

SSHDispatcher.prototype.execute = function(command, commandArgs, options) {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!self.connected) {
      return reject(new Error("Could not start service. There is no such active connection"));
    }

    self.client.exec(command + " " + (commandArgs || []).join(" "), function(err, stream) {
      if (err) { return reject(err); }

      var responseMessage = "";

      stream.on('exit', function(code, signal) {
        logger.debug("ssh-EXIT: ", code, signal);
      });

      stream.on('close', function(code, signal) {
        logger.debug('code: ' + code + ', signal: ' + signal);

        if (code === 0) {
          return resolve({code: code, data: responseMessage.replace("\n", "")});
        } else {
          reject(new Error("Error occurred while remote command: code \"" + code + "\", signal: \"" + signal + "\""));
        }
      }).on('data', function(data) {
        var dataStr = data.toString();
        // detecting OS
        if (command === "uname") {
          var platform = dataStr.substring(0, dataStr.indexOf('\n'));
          switch(platform) {
            case Enums.OS.LINUX:
              self.platform = data;
              self.adapter = new ScreenAdapter();
              break;
            case Enums.OS.MACOSX:
              self.platform = platform;
              self.adapter = new LocalSystemAdapter();
            default:
              logger.debug("Unknown platform");
              self.platform = Enums.OS.UNKNOWN;
          }
        }
        responseMessage = dataStr;

        logger.debug('ssh-STDOUT: ' + data);
      }).stderr.on('data', function(data) {
        logger.debug('ssh-STDERR: ' + data);
      });
    });
  });
};

SSHDispatcher.prototype.startService = function(commandType) {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!self.connected) {
      return reject(new Error("Could not start service. There is no such active connection"));
    }

    try {
      var serviceInstance = self.serviceInstance;
      var executable = serviceInstance.pathToBinary;
      var port = serviceInstance.port.toString();
      var serviceTypeString = Utils.getServiceTypeName(serviceInstance.service_type_id);
      var enviromentVars = serviceInstance.runEnviroment;

      var command = util.format("%s %s %s", executable, serviceTypeString, port);

      var _handleError = function(err, code) {
        reject(err, code);
      };

      var _executeCommand = function() {
        if (self.platform === Enums.OS.WIN) {
          self.execute(util.format("start %s", command)).then(function(code) {
            resolve(code);
          }).catch(function(err, code) {
            reject(err, code);
          });
        } else {
          self.adapter.executeCommand(self, command, serviceInstance, {
            serviceType: serviceTypeString
          }).then(function(code) {
            resolve(code);
          }).catch(_handleError);
        }
      };
      // TODO: Should the user configure 'EXPORT PATH=....', 'SET PATH='%PATH%...'
      // checking if there enviromentVars to be exported
      if (enviromentVars) {
        self.execute(enviromentVars).then(function(code) {
          logger.debug("Success setting enviroment vars");
          logger.debug(code);
          _executeCommand();
        }).catch(function(err, code) {
          logger.error("**Could not export enviroment vars** ", enviromentVars);
          reject(err, code);
        });
      } else {
        _executeCommand();
      }
    } catch (e) {
      reject(e);
    }
  });
};
