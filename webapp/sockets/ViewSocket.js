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
   * @property {DataManager} DataManager - TerraMA² Data Manager
   * @property {Object} Utils - TerraMA² global utility module
   * @property {ServiceType} ServiceType - TerraMA² Service Type enum  
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
        // register listener to each view service
        services.forEach(function(service) {
          TcpManager.registerListeners(service);
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
        DataManager.listRegisteredViews()
          .then(function(views) {
            return client.emit("viewResponse", views.map(function(view) {
              return view.toObject();
            }));
          })

          .catch(function(err) {
            return client.emit("error", err.toString());
          });
      }
      /**
       * It handles client disconnect. 
       * 
       * @todo Un-register listeners 
       */
      function onDisconnect() {
        console.log("Disconnected");
        TcpManager.removeListener('processFinished', onProcessFinished);
      }
      /**
       * It emits a received registered view to the listeners
       * 
       * @param {RegisteredView}
       * @returns {void}
       */
      function onProcessFinished(registeredView) {
        // broadcast to everyone
        console.log(registeredView.toObject());
        ioSocket.emit("viewReceived", registeredView.toObject());
      }

      // registering Tcp Manager events listeners
      TcpManager.on("processFinished", onProcessFinished);

      // Registering front end client listeners
      client.on("viewRequest", onViewRequest);
      client.on("disconnect", onDisconnect);
    });
  };

  module.exports = ViewSocket;
} ());