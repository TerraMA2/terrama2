"use strict";

var ViewsHandlers = function(io) {

  var memberSockets = io.sockets;
  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');
  // 'request' module
  var memberRequest = require('request');
  // WebMonitor configuration
  var memberConfig = JSON.parse(memberFs.readFileSync(memberPath.join(__dirname, "../config/config.terrama2monitor"), "utf-8"));

  // Socket connection event
  memberSockets.on('connection', function(client) {

    var retrieveFunction = function(json, responseEvent) {
      switch(responseEvent) {
        case 'retrieveNotifiedViewsResponse':
          var action = 'retrieve-notified-views';
          break;
        case 'retrieveRemovedViewsResponse':
          var action = 'retrieve-removed-views';
          break;
        default:
          var action = 'retrieve-views';
          break;
      };

      var options = {
        url: memberConfig.webadmin.protocol + memberConfig.webadmin.host + ":" + memberConfig.webadmin.port + memberConfig.webadmin.basePath + action,
        form: {
          clientId: json.clientId,
          initialRequest: json.initialRequest
        }
      };

      memberRequest.post(options, function(err, httpResponse, body) {
        if(err) {
          console.error(err);
          return client.emit(responseEvent, {});
        }

        try {
          body = JSON.parse(body);
        } catch(ex) {
          body = {};
        }

        return client.emit(responseEvent, body);
      });
    };

    client.on('retrieveNotifiedViews', function(json) {
      retrieveFunction(json, 'retrieveNotifiedViewsResponse');
    });

    client.on('retrieveRemovedViews', function(json) {
      retrieveFunction(json, 'retrieveRemovedViewsResponse');
    });

    client.on('retrieveViews', function(json) {
      retrieveFunction(json, 'retrieveViewsResponse');
    });
  });
};

module.exports = ViewsHandlers;
