"use strict";

var AttributesTable = function(io) {
  var memberSockets = io.sockets;
  var memberHttp = require('http');

  var describeFeatureTypeTemplateURL = "http://localhost:8080/geoserver/wms?service=WFS&version=1.0.0&request=DescribeFeatureType&outputFormat=application/json&typename={{LAYER_NAME}}";
  var getFeatureTemplateURL = "http://localhost:8080/geoserver/wfs?service=wfs&version=2.0.0&request=GetFeature&outputFormat=application/json&typeNames={{LAYER_NAME}}&propertyName={{PROPERTIES}}&sortBy={{SORT}}+A&startIndex={{START_INDEX}}&count={{COUNT}}";

  var getValidProperties = function(layer, callback) {
    memberHttp.get(describeFeatureTypeTemplateURL.replace('{{LAYER_NAME}}', layer), function(resp) {
      var body = '';
      var fields = [];

      resp.on('data', function(chunk) {
        body += chunk;
      });

      resp.on('end', function() {
        try {
          body = JSON.parse(body);

          for(var i = 0, propertiesLength = body.featureTypes[0].properties.length; i < propertiesLength; i++) {
            var type = body.featureTypes[0].properties[i].type.split(':');

            if(type[0] === "gml")
              fields.push(body.featureTypes[0].properties[i].name);
          }
        } catch(ex) {
          body = {};
        }

        callback(fields);
      });
    }).on("error", function(e) {
      console.error(e.message);
      callback([]);
    });
  };

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // check connection event
    client.on('checkConnection', function(json) {
      // Http request to check connection
      memberHttp.get(json.url, function(resp) {
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
      memberHttp.get(json.url, function(resp) {
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
    });

    // Proxy request event
    client.on('proxyRequestCapabilities', function(json) {
      // Http request to the received url
      memberHttp.get(json.url, function(resp) {
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

module.exports = AttributesTable;
