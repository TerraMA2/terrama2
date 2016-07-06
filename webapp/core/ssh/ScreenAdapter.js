var util = require('util');

var ScreenAdapter = {};

ScreenAdapter.executeCommand = function(ssh, command, serviceInstance, extra) {
  return new Promise(function(resolve, reject) {
    var commandId = util.format("%s_%s", serviceInstance.id, serviceInstance.port);
    var screenCommand = util.format("screen -dmS %s %s", commandId, command);

    // var screenCommand = util.format(
    //   "screen -s \"%s\" -X stuff $'%s\\n'", commandId, command);
    console.log(screenCommand);

    // var screenInitialization = util.format("screen -S \"%s\" -d -m", commandId);

    // console.log("Screen initialization ", screenInitialization);
    // var stopCommand = util.format("screen -X -S %s quit", commandId);

    // ssh.execute(screenInitialization).then(function() {
      ssh.execute(screenCommand).then(function(code) {
        resolve(code);
      }).catch(function(err) {
        // ssh.execute(stopCommand).finally(function() {
          reject(err, -1);
        // });
      });
    // }).catch(function(err) {
    //   reject(err, code);
    // })
  })
};

module.exports = ScreenAdapter;
