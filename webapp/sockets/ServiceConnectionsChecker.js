"use strict";

var ServiceTypeEnum = require('./../core/Enums').ServiceType;
const url = require('url');
/**
 * Socket responsible for checking the service connections.
 * @class ServiceConnectionsChecker
 *
 * @author Francisco Vinhas [francisco.neto@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 */
var ServiceConnectionsChecker = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // Member ssh connection checker
  var memberSSHConnectionChecker = require('./../core/SSHConnectionChecker');
  // GeorServerConnectionChecker member
  var memberSMTPConnectionChecker = require('./../core/SMTPConnectionChecker');
  // Service Port number checker member
  var memberServicePortNumberChecker = require("./../core/ServicePortNumberChecker");
  // GeorServerConnectionChecker member
  var memberGeoServerConnection = require('./../core/GeoServerConnectionChecker');
  // memberDbConnectionChecker member
  var memberDbConnection = require('./../core/DbConnectionChecker');

  // Socket connection event
  memberSockets.on('connection', function(client) {
    client.on('testServiceConnectionsRequest', function(serviceToCheck) {
      var serviceObject = serviceToCheck.service;
      var responseObject = { 
        service_id: serviceObject.id 
      };
      var paramsToCheckSSH = {
        host: serviceObject.host,
        port: serviceObject.sshPort,
        username: serviceObject.sshUser,
        isLocal: serviceObject.sshUser == null ,
        pathToBinary: serviceObject.pathToBinary
      };
      memberSSHConnectionChecker(paramsToCheckSSH, function(response){
        responseObject.sshConnectionResponse = response;
        var paramsToCheckDb = serviceObject.log;
        memberDbConnection(paramsToCheckDb, function(response){
          responseObject.dbConnectionResponse = response;
          var paramsToCheckPort = {
            port: serviceObject.port,
            service: serviceObject.id,
            checkConnection: true
          };
          memberServicePortNumberChecker(paramsToCheckPort, function(response){
            responseObject.portNumberConnectionResponse = response;
            if (serviceObject.service_type_id == ServiceTypeEnum.ALERT){
              if (serviceObject.metadata.email_server){
                var emailServerUrl = url.parse(serviceObject.metadata.email_server);
                var username = emailServerUrl.auth.split(":")[0];
                var password = emailServerUrl.auth.split(":")[1];
                var paramsToCheckEmailServer = {
                  host: emailServerUrl.hostname,
                  port: emailServerUrl.port,
                  username: username,
                  password: password
                };
                memberSMTPConnectionChecker(paramsToCheckEmailServer, function(response){
                  responseObject.smtpConnectionResponse = response;
                  client.emit('testServiceConnectionsResponse', responseObject);
                });
              } else {
                var smtpConnectionResponse = {
                  error: true,
                  message: 'Invalid email server URL'
                };
                responseObject.smtpConnectionResponse = smtpConnectionResponse;
                client.emit('testServiceConnectionsResponse', responseObject);
              }
            } else if (serviceObject.service_type_id == ServiceTypeEnum.VIEW){
              if (serviceObject.metadata.maps_server){
                var mapServerUrl = url.parse(serviceObject.metadata.maps_server);
                var username = mapServerUrl.auth.split(":")[0];
                var password = mapServerUrl.auth.split(":")[1];
                var paramsToCheckMapServer = {
                  host: mapServerUrl.hostname + mapServerUrl.path,
                  port: mapServerUrl.port,
                  username: username,
                  password: password
                };
                memberGeoServerConnection(paramsToCheckMapServer, function(response){
                  responseObject.mapServerConnectionResponse = response;
                  client.emit('testServiceConnectionsResponse', responseObject);
                });
              } else {
                var mapServerConnectionResponse = {
                  error: true,
                  message: 'Invalid map server URL'
                };
                responseObject.mapServerConnectionResponse = mapServerConnectionResponse;
                client.emit('testServiceConnectionsResponse', responseObject);
              }
            } else {
              client.emit('testServiceConnectionsResponse', responseObject);
            }
          });
        });
      });

    });
  });
};

module.exports = ServiceConnectionsChecker;
