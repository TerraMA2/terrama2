var util = require('util');
var Promise = require('bluebird');

var ScreenAdapter = {};

ScreenAdapter.make = function(serviceInstance, command) {
  var commandId = util.format("%s_%s", serviceInstance.id, serviceInstance.port);
  return util.format("screen -dmS %s %s", commandId, command);
};

ScreenAdapter.executeCommand = function(ssh, command, serviceInstance, extra) {
  var self = this;
  return new Promise(function(resolve, reject) {
    var screenCommand = self.make(serviceInstance, command);
    console.log(screenCommand);

    ssh.execute(screenCommand).then(function(code) {
      resolve(code);
    }).catch(function(err) {
      reject(err, -1);
    });
  })
};

module.exports = ScreenAdapter;
