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
   * @property {TcpService} TcpService - TerraMA² Tcp Manager
   * @property {DataManager} DataManager - TerraMA² Data Manager
   * @property {Object} Utils - TerraMA² global utility module
   * @property {ServiceType} ServiceType - TerraMA² Service Type enum  
   */
  var ViewSocket = function(io) {
    // Sockets object
    var ioSocket = io.sockets;
    // TerraMA² Tcp Manager
    var TcpService = require("./../core/facade/tcp-manager/TcpService");
    var DataManager = require("./../core/DataManager");
    var ServiceType = require("./../core/Enums").ServiceType;
    var Utils = require("./../core/Utils");

    /**
     * Defines a registered view listener. Triggered when C++ services done view execution and TerraMA² WebApp re-save in database
     * 
     * @param {RegisteredView} registeredView - TerraMA² RegisteredView
     */
    TcpService.on("viewReceived", function(registeredView) {
      ioSocket.emit("viewReceived", registeredView.toObject());
    });

    /**
     * Defines an alert notify. Triggered when C++ services done alert execution and TerraMA² WebApp send info to WebMonitor
     * 
     * @param {Object} viewInfo - View info to notify
     */
    TcpService.on("notifyView", function(viewInfo){
      ioSocket.emit("notifyView", viewInfo);
    })

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
        return DataManager.listRegisteredViews()
          .then(function(views) {
            return client.emit("viewResponse", views.map(function(view) {
              return view.toObject();
            }));
          })

          .catch(function(err) {
            return client.emit("error", err.toString());
          });
      }

      // Registering front end client listeners
      client.on("viewRequest", onViewRequest);
    });
  };

  module.exports = ViewSocket;
} ());