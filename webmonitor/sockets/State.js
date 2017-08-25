"use strict";

var State = function(io) {

  var memberSockets = io.sockets;
  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');
  // 'request' module
  var memberRequest = require('request');
  // WebMonitor configuration
  var memberConfig = JSON.parse(memberFs.readFileSync(memberPath.join(__dirname, "../config/monitor.json"), "utf-8"));

  var userToken = require('../config/UserToken');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // check connection event
    client.on('saveState', function(json) {
      var options = {
        url: memberConfig.webadmin.protocol + memberConfig.webadmin.host + ":" + memberConfig.webadmin.port + memberConfig.webadmin.basePath + "save-state",
        form: {
          userToken: userToken.getToken(),
          state: json.state
        }
      };

      memberRequest.post(options, function(err, httpResponse, body) {
        if(err) {
          console.error(err);
          return client.emit('saveStateResponse', {});
        }

        try {
          body = JSON.parse(body);
        } catch(ex) {
          body = {};
        }

        return client.emit('saveStateResponse', { result: body });
      });
    });

    // check connection event
    client.on('getState', function(json) {
      var options = {
        url: memberConfig.webadmin.protocol + memberConfig.webadmin.host + ":" + memberConfig.webadmin.port + memberConfig.webadmin.basePath + "get-state",
        form: {
          userToken: userToken.getToken()
        }
      };

      memberRequest.post(options, function(err, httpResponse, body) {
        if(err) {
          console.error(err);
          return client.emit('getStateResponse', { verifyState: (json !== undefined ? json.verifyState : false) });
        }

        try {
          body = JSON.parse(body);
        } catch(ex) {
          body = {};
        }

        return client.emit('getStateResponse', { result: body, verifyState: (json !== undefined ? json.verifyState : false) });
      });
    });
  });
};

module.exports = State;
