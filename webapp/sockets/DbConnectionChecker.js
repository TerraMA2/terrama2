"use strict";


/**
 * Socket responsible for checking the database connection with a given host.
 * @class SSHConnectionChecker
 *
 * @author Raphael Willian da Costa
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberDbConnection - DbConnectionChecker class.
 */
var DbConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // memberDbConnectionChecker member
  var memberDbConnection = require('./../core/DbConnectionChecker');

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // Postgis connection request event
    client.on('testDbConnection', function(json) {
      memberDbConnection(json, function(response){
        client.emit('testDbConnectionResponse', response);
      })
    });
  });
};

module.exports = DbConnectionChecker;
