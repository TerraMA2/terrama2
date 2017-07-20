"use strict";

var PromiseClass = require('./Promise');
var util = require('util');
var Utils = require('./Utils');
var config = require('./Application').getContextConfig();

var Process = module.exports = function(adapter) {
  // TODO: validate it
  this.adapter = adapter;
};

Process.prototype.setAdapter = function(adapter) {
  this.adapter = adapter;
};

Process.prototype.connect = function(serviceInstance) {
  var self = this;
  return new PromiseClass(function(resolve, reject) {
    self.serviceInstance = serviceInstance;
    self.adapter.connect(serviceInstance).then(function(code) {
      resolve(code);
    }).catch(function(err) {
      reject(err);
    });
  });
};

Process.prototype.disconnect = function() {
  var self = this;

  return new PromiseClass(function(resolve, reject) {
    return self.adapter.disconnect()
      .then(function() {
        return resolve();
      })
      .catch(function(err) {
        return reject(err);
      });
  });
};

Process.prototype.startService = function(command) {
  var self = this;

  return new PromiseClass(function(resolve, reject) {
    var serviceInstance = self.serviceInstance;
    var executable = serviceInstance.pathToBinary;
    var port = serviceInstance.port.toString();
    var serviceTypeString = Utils.getServiceTypeName(serviceInstance.service_type_id);

    if(config.disablePDF) {
      var command = util.format("%s %s %s %s %s", executable, serviceTypeString, port, '-platform', 'minimal');
    } else {
      var command = util.format("%s %s %s", executable, serviceTypeString, port);
    }

    return self.adapter.startService(command)
      .then(function(code) {
        return resolve(code);
      }).catch(function(err) {
        return reject(err);
      });
  });
};