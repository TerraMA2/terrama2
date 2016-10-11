"use strict";

var Proxy = function(io){
  var memberSockets = io.sockets;
  var memberHttp = require('http');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // Proxy request event
    client.on('proxyRequest', function(json) {

      // Http request to the received url
      memberHttp.get(json.url, function(resp) {
        var body = '';

        // Data receiving event
        resp.on('data', function(chunk) {
          body += chunk;
        });

        // End of request event
        resp.on('end', function() {
          if(json.format === 'xml') {
            body = body.replace(/>\s*/g, '>');
            body = body.replace(/\s*</g, '<');
          } else if(json.format === 'json') {
            try {
              body = JSON.parse(body);
            } catch(ex) {
              body = {};
            }
          }
          // Socket response
          client.emit('proxyResponse', { msg: body, requestId: json.requestId });
        });

      }).on("error", function(e) {
        console.error(e.message);
      });
    });
  });
};

module.exports = Proxy;
