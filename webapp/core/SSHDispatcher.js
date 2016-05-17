var Client = require('ssh2').Client;
var Promise = require("bluebird");
var fs = require('fs');
var util = require('util');
var Utils = require("./Utils");


var SSHDispatcher = module.exports = function() {
  this.client = new Client();
  this.connected = false;
};

SSHDispatcher.prototype.connect = function(serviceInstance) {
  var self = this;

  return new Promise(function(resolve, reject) {
    self.serviceInstance = serviceInstance;
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
      privateKey: require('fs').readFileSync(Utils.getUserHome() + '/.ssh/id_rsa')
    })
  });
}

SSHDispatcher.prototype.disconnect = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!self.connected)
      return reject(new Error("Could not disconnect. There is no such active connection"));

    self.client.end();
    resolve();
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
        console.log("ssh-EXIT: ", code, signal);
        if (code == 0)
          return resolve(code);
        return reject(new Error("Error occurred while remote command: code \"" + code + "\", signal: \"" + signal + "\""));
      });

      stream.on('close', function(code, signal) {
        console.log('code: ' + code + ', signal: ' + signal);
      }).on('data', function(data) {
        console.log('ssh-STDOUT: ' + data);
      }).stderr.on('data', function(data) {
        console.log('ssh-STDERR: ' + data);
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
      var executable = self.serviceInstance.pathToBinary;
      var port = self.serviceInstance.port.toString();
      var command;
      if (process.plataform == 'win32') {
        command = "start " + util.format("%s %s", executable.endsWith(".exe") ? executable : executable + ".exe", port);
      } else {
        command = "nohup " + util.format("%s %s %s", executable, port, (!self.serviceInstance.pathToBinary.endsWith("&") ? " &" : ""));
      }

      console.log(command);
      
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