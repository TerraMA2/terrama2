const net = require('net');
const client = new net.Socket();
const port = 5000;
const host = '127.0.0.1';
try{
    client.connect(port, host, function() {
        console.log('Connected');
        client.write("Hello From Client " + client.address().address);
    });

    client.on('data', function(data) {
        console.log('Server Says : ' + data);
        client.write("To no data");
    });

    client.on('close', function() {
        console.log('Connection closed');
    });
}
catch(err){
    console.log(err);
}
