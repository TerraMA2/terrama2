module.exports = function(io) {
  var sockets = io.sockets,
      http = require('http');

  sockets.on('connection', function(client) {
    client.on('proxyRequest', function(json) {
      http.get(json.url, function(resp){
        var xml = '';

        resp.on('data', function(chunk) {
          xml += chunk;
        });

        resp.on('end', function() {
          xml = xml.replace(/>\s*/g, '>');
          xml = xml.replace(/\s*</g, '<');

          sockets.emit('proxyResponse', { msg: xml, requestId: json.requestId });
        });

      }).on("error", function(e){
        console.log("Got error: " + e.message);
      });
    });
  });
}
