var Client = require('ssh2').Client;
var Promise = require("bluebird");
var fs = require('fs');
var util = require('util');

var SSHDispatcher = module.exports = function(serviceInstance) {
  this.client = new Client();
  this.serviceInstance = serviceInstance;
  this.connected = false;
};

SSHDispatcher.prototype.connect = function(privatekeyPath) {
  var self = this;

  return new Promise(function(resolve, reject) {
    self.client.on('ready', function() {
      self.connected = true;
      return resolve()
    });

    self.client.on('error', function(err) {
      console.log(err);
      self.connected = false;
      return reject(new Error("Error while connecting. " + err.message));
    });

    self.client.connect({
      host: self.serviceInstance.host,
      port: self.serviceInstance.sshPort,
      username: self.serviceInstance.sshUser,
      privateKey: require('fs').readFileSync(privatekeyPath || '/home/raphael/.ssh/id_rsa'), //TODO: fix it. Check the way without pass
    })
  });
}

SSHDispatcher.prototype.execute = function(command) {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!self.connected)
      return reject(new Error("Could not start service. There is no such active connection"));

    self.client.exec(command, function(err, stream) {
      if (err)
        return reject(err);

      stream.on('exit', function(code, signal) {
        console.log("EXIT: ", code, signal);
        if (code == 0)
          return resolve(code);
        return reject(new Error("Error occurred while remote command: code \"" + code + "\", signal: \"" + signal + "\""));
      });

      stream.on('close', function(code, signal) {
        console.log('code: ' + code + ', signal: ' + signal);
      }).on('data', function(data) {
        console.log('STDOUT: ' + data);
      }).stderr.on('data', function(data) {
        console.log('STDERR: ' + data);
      });
    });
  });
};

SSHDispatcher.prototype.startService = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!self.connected)
      return reject(new Error("Could not start service. There is no such active connection"));

    try {
      // nohup &
      var command = util.format("%s %s", self.serviceInstance.pathToBinary, self.serviceInstance.port.toString());
      command += (!self.serviceInstance.pathToBinary.endsWith("&") ? " &" : "");
      
      self.execute(command).then(function(code) {
        resolve(code);
      }).catch(function(err, code) {
        reject(err, code)
      })
    } catch (e) {
      reject(e);
    }

  });
};

SSHDispatcher.prototype.startServiceAsync = function() {
  var self = this;
  // return new Promise(function(resolve, reject) {
    if (!self.connected)
      return reject(new Error("Could not start service. There is no such active connection"));

    var command = util.format("%s %s", self.serviceInstance.pathToBinary, self.serviceInstance.port.toString());
    console.log("Command: " + command);
    // command += (!self.serviceInstance.pathToBinary.endsWith("&") ? " &" : "");
    this.execute(command);
  // });
};