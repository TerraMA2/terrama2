var Client = require('ssh2').Client;
var Promise = require("bluebird");
var fs = require('fs');
var util = require('util');
var Utils = require("./Utils");
var Enums = require('./Enums');
var ScreenAdapter = require('./ssh/ScreenAdapter');


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
      self.execute("ipconfig").then(function(code) {
        self.platform = Enums.OS.WIN;
      }).catch(function(err) {
        // detecting MAC or Linux
        return self.execute("uname");
      }).finally(function() {
        resolve()
      })
    });

    self.client.on('keyboard-interactive', function(name, instructions, instructionsLang, prompts, finish) {
      finish([]);
    });

    self.client.on('error', function(err) {
      console.log(err);
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

      if (!privateKey)
        return reject(new Error("Could not find private key in \"" + defaultDir + "\""));

      self.client.connect({
        host: self.serviceInstance.host,
        port: self.serviceInstance.sshPort,
        username: self.serviceInstance.sshUser,
        privateKey: require('fs').readFileSync(privateKey),
        tryKeyboard: true
      })
    }).catch(function(err) {
      reject(err);
    })
  });
};

SSHDispatcher.prototype.disconnect = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!self.connected)
      return reject(new Error("Could not disconnect. There is no such active connection"));

    self.client.end();
    resolve();
  });
};

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
      });

      stream.on('close', function(code, signal) {
        console.log('code: ' + code + ', signal: ' + signal);

        if (code == 0) {
          resolve(code);
        } else {
          reject(new Error("Error occurred while remote command: code \"" + code + "\", signal: \"" + signal + "\""));
        }
      }).on('data', function(data) {
        // detecting OS
        if (command === "uname") {
          var dataStr = data.toString();
          var platform = dataStr.substring(0, dataStr.indexOf('\n'));
          switch(platform) {
            case Enums.OS.LINUX:
            case Enums.OS.MACOSX:
              self.platform = data;
              self.adapter = ScreenAdapter;
              break;
            default:
              console.log("Unknown platform");
              self.platform = Enums.OS.UNKNOWN;
          }
        }

        console.log('ssh-STDOUT: ' + data);
      }).stderr.on('data', function(data) {
        console.log('ssh-STDERR: ' + data);
      });
    });
  });
};

SSHDispatcher.prototype.startService = function(commandType) {
  var self = this;
  return new Promise(function(resolve, reject) {
    if (!self.connected)
      return reject(new Error("Could not start service. There is no such active connection"));

    try {
      var serviceInstance = self.serviceInstance;
      var executable = serviceInstance.pathToBinary;
      var port = serviceInstance.port.toString();
      var serviceTypeString = Utils.getServiceTypeName(serviceInstance.service_type_id);
      var enviromentVars = serviceInstance.runEnviroment;

      var command = util.format("%s %s %s", executable, serviceTypeString, port);

      console.log("Platform " + self.platform);
      // if (process.plataform == 'win32') {
      //   command = "start " + util.format(
      //     "%s %s %s", executable.endsWith(".exe") ? executable : executable + ".exe",
      //     serviceTypeString,
      //     port);
      // } else {
        // avoiding nohup lock ssh session
        // command = "nohup " + util.format(
        //   "%s %s %s  > terrama2.out 2> terrama2.err < /dev/null %s",
        //   executable,
        //   serviceTypeString,
        //   port,
        //   (!self.serviceInstance.pathToBinary.endsWith("&") ? " &" : ""));
        // command = util.format("screen -dmS %s %s %s %s", commandId,
        //                                                  executable,
        //                                                  serviceTypeString,
        //                                                  port)
        // command = util.format("screen -s \"%s\" -X stuff $'%s %s %s\n'", commandId,
        //                                                                  executable,
        //                                                                  serviceTypeString,
        //                                                                  port);
      // }

      var _handleError = function(err, code) {
        reject(err, code);
      };

      var _executeCommand = function() {
        if (self.platform === Enums.OS.WIN) {
          self.execute(util.format("start %s", command)).then(function(code) {
            resolve(code);
          }).catch(function(err, code) {
            reject(err, code);
          })
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
          console.log("Success setting enviroment vars")
          console.log(code);
          _executeCommand();
        }).catch(function(err, code) {
          console.log("**Could not export enviroment vars**");
          reject(err, code);
        })
      } else {
        _executeCommand();
      }
    } catch (e) {
      reject(e);
    }

  });
};
