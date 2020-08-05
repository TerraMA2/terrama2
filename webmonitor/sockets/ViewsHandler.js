"use strict";

var ViewsHandlers = function(io) {

  var memberSockets = io.sockets;
  // 'request' module
  var memberRequest = require('request');
  // WebMonitor configuration
  var Application = require('./../core/Application');
  var memberConfig = Application.getContextConfig();

  const common = require('./../utils/common');

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
        url: common.urlResolve(memberConfig.webadmin.internal_uri, action),
        form: {
          clientId: json.clientId,
          userToken: json.token,
          initialRequest: json.initialRequest,
          onlyPrivate: json.onlyPrivate,
          views: (json.views ? json.views : null)
        }
      };

      memberRequest.post(options, function(err, httpResponse, body) {
        if(err) {
          return client.emit('retrieveViewsError', `Could not connect to WebApplication: ${err.code} - ${err.address}:${err.port}`);
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
