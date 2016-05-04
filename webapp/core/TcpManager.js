var net = require('net');
var Signals = require('./Signals.js');

var TcpManager = module.exports = {};

TcpManager.sendData = function(data) {
  emit(Signals.ADD_DATA_SIGNAL, data);
};

TcpManager.checkStatus = function() {
  emit(Signals.STATUS_SIGNAL, {});
}

TcpManager.startService = function(data) {
  emit(Signals.START_PROCESS_SIGNAL, {});
}

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
