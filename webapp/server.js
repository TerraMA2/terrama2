const net = require('net');
const server = net.createServer();

server.listen(6548, 'localhost', () => {
  console.log('TCP Server is running on port ' + 6548 +'.');
});

server.on('connection', function(clientSocket) {
  console.log('CONNECTED: ' + clientSocket.remoteAddress + ':' + clientSocket.remotePort);

  clientSocket.write('chego');

  clientSocket.on('data', function(data) {
      // parseBytearray
    //console.log("clientsocket: "+ data.toString())

   const buff = parser(data);

    //   switch(buff.signal) {
    //     case Signals.TERMINATE_SERVICE_SIGNAL:
    //        stop();
    //        break;
    //      case Signals.START_PROCESS_SIGNAL:
    //        console.log("COMEÇOU");
    //        break;
    //      default:
    //        clientSocket.write()
    //    }
  });
});