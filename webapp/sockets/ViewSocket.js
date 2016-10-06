(function() {
  'use strict';

  /**
   * Socket responsible for delivery real time views
   * @class ViewSocket
   *
   * @author Raphael Willian da Costa [raphael.costa@dpi.inpe.br]
   *
   * @param {}
   * @property {Object} ioSocket - Sockets IO object.
   * @property {TcpManager} TcpManager - TerraMA² Tcp Manager
   */
  var ViewSocket = function(io) {
    // Sockets object
    var ioSocket = io.sockets;
    // TerraMA² Tcp Manager
    var TcpManager = require("./../core/TcpManager");
    var DataManager = require("./../core/DataManager");
    var ServiceType = require("./../core/Enums").ServiceType;
    var Utils = require("./../core/Utils");

    // retrieving all view services before client connection
    DataManager.listServiceInstances({service_type_id: ServiceType.VIEW})
      .then(function(services) {
        // register listener
        services.forEach(function(service) {
          TcpManager.registerListener(service);
        });
      })
      
      .catch(function(err) {
        console.log("error", Utils.format(
          "Could not retrieve available view Service due %s.\nPlease reload page",
          err.toString()
        ));
      });

    // Socket connection event
    ioSocket.on('connection', function(client) {
      /**
       * It handles client view request. Once requested, it checks in database for cached views and tries to return
       * @check If there is not, send Views to the service and wait for a response. Once available, it delivery
       * in real-time the changes in schedule.
       * 
       * @returns {void} 
       */
      function onViewRequest() {
        // TODO: filter user permission
        DataManager.listView()
          .then(function(views) {

          })

          .catch(function(err) {

          });
      }

      function onDisconnect() {
        console.log("Disconnected");
      }

      client.on("request", onViewRequest);
      client.on("disconnect", onDisconnect);
    });
  };

  module.exports = ViewSocket;
} ());