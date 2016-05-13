var net = require('net');
var Signals = require('./Signals.js');
var SSH = require("./SSHDispatcher");
var Promise = require('bluebird');
var Utils = require('./Utils');

var TcpManager = module.exports = {};


function parseByteArray(byteArray) {
  var messageSizeReceived = byteArray.readUInt32BE(0);
  var signalReceived = byteArray.readUInt32BE(4);
  var rawData = byteArray.slice(8, byteArray.length);
  console.log(rawData.toString());

  // validate signal
  var signal = Utils.getTcpSignal(signalReceived);
  var jsonMessage = JSON.parse(rawData);

  return {
    size: messageSizeReceived,
    signal: signal,
    message: jsonMessage
  }
}

// todo: connection cache. it should be used in terrama2 shutdown
/** The structure looks like: 
* {
*   servicename..: {socket: clientSocket, service: ServiceInstance}
* }
*/
var clients = {};


function _getClient(connection, buffer) {
  return new Promise(function(resolve, reject) {
    var client;
    // checking if exists
    for (var key in clients) {
      if (clients.hasOwnProperty(key)) {
        client = clients[key];
        break;
      }
    }

    var closeCallbackCalled = false;

    if (!client) {
      client = {
        socket: new net.Socket(),
        service: connection
      };

      clients[connection.name] = client;

      // connect
      client.socket.connect(connection.port, connection.host, function() {

        return resolve(client);
      });

      client.socket.on('error', function(err) {
        console.log("\n\n[ERROR] ", err);

        reject(new Error(err));
        closeCallbackCalled = true;
      });

      client.socket.on('close', function() {
        console.log('\n\n[CLIENT] Connection closed');
        if (!closeCallbackCalled)
          resolve();
      });
    } else {
      if (client.socket.readyState == "open")
        return resolve(client);
      else {
        client.socket.connect(connection.port, connection.host, function () {

          return resolve(client);
        });
      }
    }
  });
}

/**
This method provides a client to communicate in tcp channel. It saves a service instance in memory

@param {ServiceInstance} connection - a terrama2 service instance
@param {Buffer} buffer - a buffer containing message to send
@param {Boolean} waitForResponse - a bool value to define if it will wait for receive any data
*/
function _makeClient(connection, buffer, waitForResponse) {
  return new Promise(function(resolve, reject) {
    try {
      var client;
      // checking if exists
      for (var key in clients) {
        if (clients.hasOwnProperty(key)) {
          client = clients[key];
          break;
        }
      }

      if (!client) {
        client = {
          socket: new net.Socket(),
          service: connection
        };

        clients[connection.name] = client;

        // connect
        client.socket.connect(connection.port, connection.host, function() {
          client.socket.write(buffer);

          if (!waitForResponse)
            return resolve(null);
        });

        client.socket.on('data', function(data) {
          console.log('\n\n[CLIENT] Received:\n');
          console.log("BYTES: ", data);
          console.log("JSON: ", data.toString());
          try {
            var parsedMessage = parseByteArray(data);

            // if (waitForResponse)
            return resolve(parsedMessage);
          } catch(e) {
            return reject(e);
          }
        });

        client.socket.on('close', function() {
          console.log('\n\n[CLIENT] Connection closed');
        });
      } else {
        if (client.socket.readyState != "open")
          client.socket.connect(connection.port, connection.host, function () {
            client.socket.write(buffer);

            if (!waitForResponse)
              return resolve(null);
          });
      }

    } catch (e) {
      return reject(e)
    }
  });
}

/**
This method prepares a bytearray to send in tcp socket.
@param {Signals} signal - a valid terrama2 tcp signal
@param {Object} object - a javascript object message to send
*/
function makeBuffer(signal, object) {
  try {
    if(isNaN(signal))
     throw TypeError(signal + " is not a valid signal!");

    var jsonMessage = JSON.stringify(object).replace(/\":/g, "\": ");
    console.log(jsonMessage);
    // The size of the message plus the size of two integers, 4 bytes each
    var totalSize = jsonMessage.length + 4;

    // Creates the buffer and fills it with zeros
    var buffer = new Buffer(totalSize + 4);

    // Writes the message (string) in the buffer with UTF-8 encoding
    buffer.write(jsonMessage, 8, jsonMessage.length);

    // Writes the buffer size (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(totalSize, 0);

    // // Writes the signal (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(signal, 4);
  } catch (e) {
    throw e;
  }

  return buffer;
}

// async
TcpManager.sendData = function(serviceInstance, data) {
  try {
    var buffer = makeBuffer(Signals.ADD_DATA_SIGNAL, data);
    
    _getClient(serviceInstance).then(function(client) {
      client.socket.write(buffer);
    }).catch(function(err) {
      throw err;
    });

  } catch (e) {
    console.log(e);
    throw new Error("Could not send data to service", e);
  }
};


// async
TcpManager.updateService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    try {
      var buffer = makeBuffer(Signals.UPDATE_SERVICE_SIGNAL, serviceInstance);
      _getClient(serviceInstance).then(function(client) {
        // sending buffer (async)
        client.socket.write(buffer);

        resolve();
      }).catch(function(err) {
        reject(err);
      });

    } catch (e) {
      reject(e)
    }
  });
};

// sync
TcpManager.startService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    var ssh = new SSH(serviceInstance);
    ssh.connect().then(function() {

      ssh.startService().then(function(code) {
        resolve(code)
      }).catch(function(err, errCode) {
        reject(err, errCode);
      });

    }).catch(function(err) {
      console.log('ssh connect')
      reject(err);
    });
  })
}

// ping: sync
TcpManager.statusService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    try {
      var buffer = makeBuffer(Signals.STATUS_SIGNAL, {});

      _getClient(serviceInstance).then(function(client) {
        // sending buffer
        client.socket.write(buffer);

        client.socket.on('data', function(data) {
          console.log('\n\n[CLIENT] Received:\n');
          console.log("BYTES: ", data);
          console.log("JSON: ", data.toString());
          try {
            var parsedMessage = parseByteArray(data);

            switch(parsedMessage.signal) {
              case Signals.ADD_DATA_SIGNAL:
            }

            // if (waitForResponse)
            return resolve(parsedMessage);
          } catch(e) {
            return reject(e);
          }
        });

      }).catch(function(err) {
        reject(err);
      });

    } catch (e) {
      reject(e)
    }

  });
}

// async
TcpManager.stopService = function(serviceInstance) {
  return new Promise(function(resolve, reject) {
    try {
      var buffer = makeBuffer(Signals.TERMINATE_SERVICE_SIGNAL, {});
      _getClient(serviceInstance).then(function(client) {

        resolve();
        // sending buffer
        client.socket.write(buffer);
      }).catch(function(err) {
        reject(err);
      });

    } catch(e) {
      reject(e);
    }
  });
};

var emit = function(signal, object) {
  try {
    if(isNaN(signal)) throw TypeError(signal + " is not a valid signal!");

    // Stringifies the message
    // var jsonMessage = '\x00\x00\x01\x01{"DataProviders": [{"active": true,"class": "DataProvider","data_provider_type": "FILE","description": "Testing provider","id": 1,"intent": 0,"name": "Provider","project_id": 1,"uri": "file:///home/jsimas/MyDevel/dpi/terrama2-build/data/PCD_serrmar_INPE"}]}';
    
    //home/jsimas/MyDevel/dpi/terrama2-build/data/fire_system
    //'\x00\x00\x01\x01'
    var jsonMessage = JSON.stringify(object).replace(/\":/g, "\": ");

    console.log(jsonMessage);

    // The size of the message plus the size of two integers, 4 bytes each
    var totalSize = jsonMessage.length + 4;

    // Creates the buffer and fills it with zeros
    var buffer = new Buffer(totalSize + 4);

    // Writes the message (string) in the buffer with UTF-8 encoding
    buffer.write(jsonMessage, 8, jsonMessage.length);

    // Writes the buffer size (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(totalSize, 0);
    
    // // Writes the signal (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(signal, 4);
    
    var client = new net.Socket();
    console.log(buffer);
    console.log("Total size: ", totalSize);
    console.log("");

    //150.163.17.179
    client.connect(30000, '150.163.17.179', function() {
      // writing data in socket
      client.write(buffer);
    });

    client.on('data', function(data) {
      console.log('\n\n[CLIENT] Received:\n');
      console.log(data);
    });

    client.on('close', function() {
      console.log('\n\n[CLIENT] Connection closed');
    });

    client.on('error', function(err) {
      console.log("\n\n[ERROR] ", err);
    })
  } catch(error) {
    throw error;
  }
};

var server = net.createServer();

server.on('connection', function(socket) {

  socket.on('data', function(buffer) {
    try {
      console.log(buffer);
      console.log(buffer.toString());
      var bufferSize = buffer.readUInt32BE(0);
      var signal = buffer.readUInt32BE(4);
      console.log(bufferSize);
      console.log(signal);

      if(!Buffer.isBuffer(buffer) || bufferSize !== socket.bytesRead) throw TypeError("Invalid buffer!");

      var message = buffer.toString('utf8', 8);

      console.log("\n\n[SERVER] Signal:\n");
      console.log(signal);

      console.log("\n\n[SERVER] Buffer Size:\n");
      console.log(bufferSize);

      console.log("\n\n[SERVER] Message:\n");
      console.log(message);
    } catch(error) {
      throw error;
    }
  });

  socket.on("close", function(hasError){
    console.log("Has error: " + hasError);
  });
});

server.listen(1337, '0.0.0.0');
server.on('close', function() {
  console.log("TcpManager server closed");
});

TcpManager.close = function(callback) {
  server.close(function() {
    callback();
  });
};
