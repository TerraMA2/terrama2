"use strict";

var Proxy = function(io){
  var memberSockets = io.sockets;
  const common = require('./../utils/common');
  var memberXmlParser = require('../utils/XmlParser');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // check connection event
    client.on('checkConnection', function(json) {
      // Http request to check connection
      common.getHttpHandler(json.url).get(json.url, function(resp) {
        // If success to access the url, send an object with connected propertie with true value
        client.emit('connectionResponse', { connected: true, requestId: json.requestId, url: json.url});
      })
      // if an error occurs, send a object with connected propertie with false value
      .on("error", function(e) {
        client.emit('connectionResponse', { connected: false, requestId: json.requestId, url: json.url});
      });
    });

    // Proxy request event
    client.on('proxyRequest', function(json) {
      // Http request to the received url
      try {
        common.getHttpHandler(json.url).get(json.url, function(resp) {
          var body = '';
          // Data receiving event
          resp.on('data', function(chunk) {
            body += chunk;
          });
          // End of request event
          resp.on('end', function() {
            if (json.format === 'xml') {
              body = memberXmlParser(body);
            } else if (json.format === 'json') {
              try {
                body = JSON.parse(body);
              } catch(ex) {
                body = {};
              }
            }
            // Socket response
            client.emit('proxyResponse', { msg: body, requestId: json.requestId, params: json.params });
          });
        }).on("error", function(e) {
          console.error(e.message);
        });
      } catch(err) {
        client.emit('proxyResponse', { msg: null, requestId: json.requestId, params: json.params });
      }
    });

    // Proxy request event
    client.on('proxyRequestCapabilities', function(json) {
      // Http request to the received url
      common.getHttpHandler(json.url).get(json.url, function(resp) {
        var body = '';
        // Data receiving event
        resp.on('data', function(chunk) {
          body += chunk;
        });
        // End of request event
        resp.on('end', function() {
          if (json.format === 'xml') {
            body = memberXmlParser(body);
          } else if (json.format === 'json') {
            try {
              body = JSON.parse(body);
            } catch(ex) {
              body = {};
            }
          }
          // Socket response
          client.emit('proxyResponseCapabilities', {
            msg: body,
            requestId: json.requestId,
            layerId: json.layerId,
            parent: json.parent,
            layerName: json.layerName,
            update: json.update
           });
        });

      }).on("error", function(e) {
        console.error(e.message);
      });
    });
  });
};

module.exports = Proxy;
