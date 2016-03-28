var net = require('net');
var server = net.createServer();

server.socket = null;

var TcpManager = module.exports = {};

TcpManager.emit = function(signal, object) {
  try {
    if(isNaN(signal)) throw TypeError(signal + " is not a valid signal!");

    // Stringifies the message
    var jsonMessage = JSON.stringify(object);

    // The size of the message plus the size of two integers, 4 bytes each
    var totalSize = jsonMessage.length + 8;

    // Creates the buffer and fills it with zeros
    var buffer = new Buffer(totalSize).fill(0);

    // Writes the message (string) in the buffer with UTF-8 encoding
    buffer.write(jsonMessage, 8, jsonMessage.length, 'utf8');

    // Writes the signal (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(signal, 0);

    // Writes the buffer size (unsigned 32-bit integer) in the buffer with big endian format
    buffer.writeUInt32BE(totalSize, 4);

    //var client = new net.Socket();

    //client.connect(1337, '127.0.0.1', function() {
      //client.write(buffer);
      //client.end();
    //});

    this.server.socket.write(buffer);

    client.on('data', function(data) {
      console.log('\n\n[CLIENT] Received:\n');
      console.log(data);
    });

    client.on('close', function() {
      console.log('\n\n[CLIENT] Connection closed');
    });
  } catch(error) {
    throw error;
  }
};

server.on('connection', function(socket) {

  server.socket = socket;

  socket.on('data', function(buffer) {
    try {
      var signal = buffer.readUInt32BE(0);
      var bufferSize = buffer.readUInt32BE(4);

      if(!Buffer.isBuffer(buffer) || bufferSize !== buffer.length) throw TypeError("Invalid buffer!");

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

  socket.on("close", function(state){
    console.log("Disconnected: " + state);
    server.socket = null;
  });
});

server.listen(1337, '127.0.0.1');
