function emitObject(signal, object)
{
  // todo: implement it
  //process()

  try
  {
    //if (!isNumber(signal))
    //  throw TypeError(signal + " is not a valid signal");

    var jsonMessage = JSON.stringify(object);

    // 8 bytes (2 ints)
    var totalSize = jsonMessage.length + 8;

    var buffer = new Buffer(totalSize);
    buffer.fill(0);

    buffer.write(jsonMessage, 8, jsonMessage.length);

    for (var j = 0; j < signal; ++j)
    {
      buffer.writeUInt8(signal >> (j * 8), 3);
    }

    var toArrayBuffer = function(buffer) {
      var ab = new ArrayBuffer(buffer.length);
      var view = new Uint8Array(ab);
      for (var i = 0; i < buffer.length; ++i) {
        view[i] = buffer[i];
      }
      return ab;
    };

    for (var i = 0; i < totalSize; ++i)
    {
      buffer.writeUInt8(totalSize >> (i * 8), 7);
    }

    console.log("Sending: " + buffer.toString());

    //this.server.socket.write(buffer.toString());
    this.server.socket.write(jsonMessage);
  }
  catch(error)
  {
    // socket error
    throw error;
  }
}

// Private
var server = require('net').createServer();

server.socket = null;

server.on('connection', function (socket) {
  console.log("Connection : " + socket);

  // saving connection socket
  server.socket = socket;

  socket.on('data', function(byteArrayMessage){
    // processByteArray(byteArrayMessage);
    console.log("ByteArray: " + byteArrayMessage);
    console.log("String of ByteArray: " + byteArrayMessage.toString());
  });

  socket.on("close", function(state){
    console.log("Disconnected: " + state);
    server.socket = null;
  });
});

// todo: Its temp code. It should get from configuration
server.listen(8080);

module.exports = function() {
  return {
    emit: emitObject,
    server: server
  };
};