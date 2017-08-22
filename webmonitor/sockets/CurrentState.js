"use strict";

var CurrentState = function(io) {

  var memberSockets = io.sockets;
  var memberFs = require('fs');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // check connection event
    client.on('saveState', function(json) {
      memberFs.writeFile('myjsonfile.json', JSON.stringify(json.content), 'utf8');
    });
  });
};

module.exports = CurrentState;
