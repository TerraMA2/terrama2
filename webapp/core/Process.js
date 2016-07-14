var Promise = require('bluebird');
var util = require('util');
var Utils = require('./Utils');

var Process = module.exports = function(adapter) {
  // TODO: validate it
  this.adapter = adapter;
};

Process.prototype.setAdapter = function(adapter) {
  this.adapter = adapter;
};

Process.prototype.connect = function(serviceInstance) {
  var self = this;
  return new Promise(function(resolve, reject) {
    self.serviceInstance = serviceInstance;
    self.adapter.connect(serviceInstance).then(function(code) {
      resolve(code);
    }).catch(function(err) {
      reject(err);
    })
  });
};

Process.prototype.disconnect = function() {
  var self = this;

  return new Promise(function(resolve, reject) {
    self.adapter.disconnect().then(function() {
      resolve();
    }).catch(function(err) {
      reject(err);
    })
  });
};

Process.prototype.startService = function(command) {
  var self = this;

  return new Promise(function(resolve, reject) {
    var serviceInstance = self.serviceInstance;
    var executable = serviceInstance.pathToBinary;
    var port = serviceInstance.port.toString();
    var serviceTypeString = Utils.getServiceTypeName(serviceInstance.service_type_id);

    var command = util.format("%s %s %s", executable, serviceTypeString, port);

    self.adapter.startService(command).then(function(code) {
      resolve(code);
    }).catch(function(err) {
      reject(err);
    });
  });
};