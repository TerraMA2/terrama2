"use strict";

var UriPattern = require('./../core/Enums').Uri;

/**
 * Socket responsible for checking the ssh connection with a given host.
 * @class SSHConnectionChecker
 *
 * @author Raphael Willian da Costa
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberPostgis - Postgis class.
 */
var DbConnectionChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Ssh class
  var memberPostgis = require("../core/PostgisRequest.js");

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // Postgis connection request event
    client.on('testDbConnection', function(json) {
      var returnObject = {
        error: false,
        message: ""
      };

      var params = {};
      params[UriPattern.SCHEME] = "POSTGIS";
      params[UriPattern.HOST] = json.host;
      params[UriPattern.PORT] = json.port;
      params["database"] = json.database;
      params[UriPattern.USER] = json.user;
      params[UriPattern.PASSWORD] = json.password;

      try {
        var dbRequester = new memberPostgis(params);

        dbRequester.request().then(function() {
          returnObject.message = "Success";
          client.emit('testDbConnectionResponse', returnObject);
        }).catch(function(err) {
          returnObject.error = true;
          returnObject.message = err.toString();

          client.emit('testDbConnectionResponse', returnObject);
        })
      } catch (e) {
        returnObject.error = true;
        returnObject.message = "Invalid connection parameters";

        client.emit('testDbConnectionResponse', returnObject);
      }
    });
  });
};

module.exports = DbConnectionChecker;
